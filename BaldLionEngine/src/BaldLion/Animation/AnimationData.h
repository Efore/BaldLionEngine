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
#include <glm/gtc/matrix_transform.hpp>

namespace BaldLion
{
	namespace Animation
	{
		struct KeyFrame {

			DynamicArray<JointTransform> JointTranforms;
			float TimeStamp;

			//---------------------------------------------------------------------------------------

			KeyFrame(){}

			//---------------------------------------------------------------------------------------
			
			KeyFrame(const DynamicArray<JointTransform>& jointTransforms, const float timeStamp): 
				JointTranforms(jointTransforms), 
				TimeStamp(timeStamp)
			{}

			//---------------------------------------------------------------------------------------

			KeyFrame(KeyFrame &&other) : 
				TimeStamp(other.TimeStamp), 
				JointTranforms(std::move(other.JointTranforms))
			{
			}

			//---------------------------------------------------------------------------------------

			KeyFrame& operator=(const KeyFrame& other)
			{
				JointTranforms = other.JointTranforms;
				TimeStamp = other.TimeStamp;

				return *this;
			}

			//---------------------------------------------------------------------------------------

			KeyFrame& operator=(KeyFrame &&other)
			{
				TimeStamp = other.TimeStamp;
				JointTranforms = std::move(other.JointTranforms);				

				return *this;
			}
		};
		
		//---------------------------------------------------------------------------------------

		struct AnimationData : public ResourceManagement::Resource {

			glm::mat4 InverseRootTransform;
			DynamicArray<KeyFrame> AnimationFrames;
			float AnimationLength;

			//---------------------------------------------------------------------------------------
			
			AnimationData() = default;
			
			//---------------------------------------------------------------------------------------

			AnimationData(const glm::mat4& inverseRootTransform, const std::string& animationPath, ui32 animationFramesCount, const float animationLenght) : 
				ResourceManagement::Resource (BL_STRING_TO_STRINGID(animationPath), animationPath, ResourceManagement::ResourceType::Animation),
				InverseRootTransform(inverseRootTransform),				
				AnimationFrames(DynamicArray<KeyFrame>(Memory::MemoryManager::GetAllocatorType(this), animationFramesCount)),
				AnimationLength(animationLenght)
			{}

			//---------------------------------------------------------------------------------------

			AnimationData(AnimationData&& other) : 
				InverseRootTransform(other.InverseRootTransform),				
				AnimationLength(other.AnimationLength),
				AnimationFrames(std::move(other.AnimationFrames))
			{
			}

			//---------------------------------------------------------------------------------------

			AnimationData& operator=(const AnimationData& other)
			{
				InverseRootTransform = other.InverseRootTransform;				
				AnimationLength = other.AnimationLength;
				AnimationFrames = other.AnimationFrames;

				return *this;
			}

			//---------------------------------------------------------------------------------------

			AnimationData& operator=(AnimationData&& other)
			{
				InverseRootTransform = other.InverseRootTransform;				
				AnimationLength = other.AnimationLength;
				AnimationFrames = std::move(other.AnimationFrames);				

				return *this;
			}			
		};

	}
}