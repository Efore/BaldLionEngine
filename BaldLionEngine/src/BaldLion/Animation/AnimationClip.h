/*****************************************************************
 * \file   AnimationData.h
 * \brief  This file contains the definition for the KeyFrames and the Animation Data itself
 * 
 * \author Iván León Santiago
 * \date   April 2021
 *********************************************************************/

#pragma once

#include "Joint.h"
#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/ResourceManagement/Resource.h"
#include "BaldLion/Utils/Compression.h"
#include <glm/gtc/matrix_transform.hpp>

namespace BaldLion
{
	namespace Animation
	{
		using namespace Compression;

		#define ROTATION_STRIDE 3
		#define POSITION_STRIDE 3
		#define SCALE_STRIDE 3

		struct KeyFrame {

			JointTransform JointTransforms[(ui32)JointType::Count];			
			float TimeStamp;

			//---------------------------------------------------------------------------------------

			KeyFrame() = default;

			//---------------------------------------------------------------------------------------
			
			KeyFrame(const float timeStamp):
				TimeStamp(timeStamp)
			{				
			}

			//---------------------------------------------------------------------------------------

			KeyFrame(KeyFrame &&other) : 
				TimeStamp(other.TimeStamp)
			{
				for (ui32 i = 0; i < (ui32)JointType::Count; ++i)
				{
					JointTransforms[i] = other.JointTransforms[i];
				}
			}

			//---------------------------------------------------------------------------------------

			KeyFrame& operator=(const KeyFrame& other)
			{
				for (ui32 i = 0; i < (ui32)JointType::Count; ++i)
				{
					JointTransforms[i] = other.JointTransforms[i];
				}
				TimeStamp = other.TimeStamp;

				return *this;
			}

			//---------------------------------------------------------------------------------------

			KeyFrame& operator=(KeyFrame &&other)
			{
				TimeStamp = other.TimeStamp;
				for (ui32 i = 0; i < (ui32)JointType::Count; ++i)
				{
					JointTransforms[i] = other.JointTransforms[i];
				}

				return *this;
			}
		};
		
		//---------------------------------------------------------------------------------------

		struct AnimationTrackSettings {

			QuantizationRange PositionRangeX;
			QuantizationRange PositionRangeY;
			QuantizationRange PositionRangeZ;

			QuantizationRange ScaleRangeX;
			QuantizationRange ScaleRangeY;
			QuantizationRange ScaleRangeZ;

			i32 TrackStartIndex = -1;
			bool PositionIsStatic = false;
			bool ScaleIsStatic = false;
		};

		//---------------------------------------------------------------------------------------

		struct AnimationClip : public ResourceManagement::Resource {

			glm::mat4 InverseRootTransform;
			DynamicArray<ui16> AnimationData;
			AnimationTrackSettings AnimationTracks[(ui32)JointType::Count];
			ui32 NumFrames = 0;
			float AnimationTimeLength = 0.0f;

			//---------------------------------------------------------------------------------------
			
			AnimationClip() = default;
			
			//---------------------------------------------------------------------------------------

			AnimationClip(const std::string& animationPath, const glm::mat4& inverseRootTransform, ui32 animationFramesCount, const float animationTimeLenght) :
				ResourceManagement::Resource (BL_STRING_TO_STRINGID(animationPath), animationPath, ResourceManagement::ResourceType::Animation),
				InverseRootTransform(inverseRootTransform),				
				AnimationData(DynamicArray<ui16>(Memory::MemoryManager::GetAllocatorType(this), animationFramesCount * (ui32)JointType::Count * 3)),
				AnimationTimeLength(animationTimeLenght),
				NumFrames(animationFramesCount)
			{
			}		

			//---------------------------------------------------------------------------------------

			AnimationClip& operator=(const AnimationClip& other)
			{
				InverseRootTransform = other.InverseRootTransform;				
				AnimationTimeLength = other.AnimationTimeLength;
				AnimationData = other.AnimationData;
				for (ui32 i = 0; i < (ui32)JointType::Count; ++i)
				{
					AnimationTracks[i] = other.AnimationTracks[i];
				}

				return *this;
			}	

			//---------------------------------------------------------------------------------------

			void GetFramePosition(ui32 jointIndex, ui32 frame, ui32 nextFrame, float interpolant, glm::vec3& result) const {

				const AnimationTrackSettings* trackSetting = &(AnimationTracks[jointIndex]);

				ui32 positionStartIndex = trackSetting->TrackStartIndex + (ROTATION_STRIDE * NumFrames);

				if (trackSetting->PositionIsStatic)
				{
					float framePosX = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->PositionRangeX.minValue, trackSetting->PositionRangeX.maxValue);
					float framePosY = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->PositionRangeY.minValue, trackSetting->PositionRangeY.maxValue);
					float framePosZ = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->PositionRangeZ.minValue, trackSetting->PositionRangeZ.maxValue);

					result.x = framePosX;
					result.y = framePosY;
					result.z = framePosZ;
				}
				else {

					positionStartIndex += (frame * POSITION_STRIDE);

					float framePosX = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->PositionRangeX.minValue, trackSetting->PositionRangeX.maxValue);
					float framePosY = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->PositionRangeY.minValue, trackSetting->PositionRangeY.maxValue);
					float framePosZ = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->PositionRangeZ.minValue, trackSetting->PositionRangeZ.maxValue);

					if (frame < nextFrame)
					{
						float nextFramePosX = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->PositionRangeX.minValue, trackSetting->PositionRangeX.maxValue);
						float nextframePosY = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->PositionRangeY.minValue, trackSetting->PositionRangeY.maxValue);
						float nextframePosZ = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->PositionRangeZ.minValue, trackSetting->PositionRangeZ.maxValue);

						glm::vec3 framePos = glm::vec3(framePosX, framePosY, framePosZ);
						glm::vec3 nextFramePos = glm::vec3(nextFramePosX, nextframePosY, nextframePosZ);
						
						result = glm::mix(framePos, nextFramePos, interpolant);
					}
					else {
						result.x = framePosX;
						result.y = framePosY;
						result.z = framePosZ;
					}
				}
			}

			void GetFrameRotation(ui32 jointIndex, ui32 frame, ui32 nextFrame, float interpolant, glm::quat& result) const{

				const AnimationTrackSettings* trackSetting = &(AnimationTracks[jointIndex]);

				ui32 positionStartIndex = trackSetting->TrackStartIndex + (ROTATION_STRIDE * frame);

				ui16 valueA = AnimationData[positionStartIndex++];
				ui16 valueB = AnimationData[positionStartIndex++];
				ui16 valueC = AnimationData[positionStartIndex++];

				Compression::DecompressSmallestThree(valueA, valueB, valueC, result);

				if (frame < nextFrame)
				{
					valueA = AnimationData[positionStartIndex++];
					valueB = AnimationData[positionStartIndex++];
					valueC = AnimationData[positionStartIndex++];					

					glm::quat nextFrameRot;
					Compression::DecompressSmallestThree(valueA, valueB, valueC, nextFrameRot);

					result = glm::slerp(result, nextFrameRot, interpolant);
				}
			}

			void GetFrameScale(ui32 jointIndex, ui32 frame, ui32 nextFrame, float interpolant, glm::vec3& result) const{

				const AnimationTrackSettings* trackSetting = &(AnimationTracks[jointIndex]);
				ui32 positionStartIndex = trackSetting->TrackStartIndex + (ROTATION_STRIDE * NumFrames); 

				if (trackSetting->PositionIsStatic)
				{
					positionStartIndex += POSITION_STRIDE;
				}
				else {
					positionStartIndex += (POSITION_STRIDE * NumFrames);
				}

				if (trackSetting->ScaleIsStatic)
				{
					float frameScaleX = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->ScaleRangeX.minValue, trackSetting->ScaleRangeX.maxValue);
					float frameScaleY = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->ScaleRangeY.minValue, trackSetting->ScaleRangeZ.maxValue);
					float frameScaleZ = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->ScaleRangeZ.minValue, trackSetting->ScaleRangeZ.maxValue);

					result.x = frameScaleX;
					result.y = frameScaleY;
					result.z = frameScaleZ;
				}
				else {					

					positionStartIndex += (frame * SCALE_STRIDE);

					float frameScaleX = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->ScaleRangeX.minValue, trackSetting->ScaleRangeX.maxValue);
					float frameScaleY = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->ScaleRangeY.minValue, trackSetting->ScaleRangeY.maxValue);
					float frameScaleZ = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->ScaleRangeZ.minValue, trackSetting->ScaleRangeZ.maxValue);

					result.x = frameScaleX;
					result.y = frameScaleY;
					result.z = frameScaleZ;

					if (frame < nextFrame)
					{
						float nextFrameScaleX = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->ScaleRangeX.minValue, trackSetting->ScaleRangeX.maxValue);
						float nextFrameScaleY = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->ScaleRangeY.minValue, trackSetting->ScaleRangeY.maxValue);
						float nextFrameScaleZ = Compression::DecompressFloatRL<16>(AnimationData[positionStartIndex++], trackSetting->ScaleRangeZ.minValue, trackSetting->ScaleRangeZ.maxValue);

						glm::vec3 nextFrameScale = glm::vec3(nextFrameScaleX, nextFrameScaleY, nextFrameScaleZ);
						result = glm::mix(result, nextFrameScale, interpolant);
					}
				}
			}
		};

	}
}