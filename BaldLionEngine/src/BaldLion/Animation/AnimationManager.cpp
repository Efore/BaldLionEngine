/*****************************************************************//**
 * \file   AnimationManager.cpp
 * \brief  Definition for AnimationManager's methods
 * 
 * \author Iván León Santiago
 * \date   April 2021
 *********************************************************************/

#include "blpch.h"
#include "AnimationManager.h"
#include "BaldLion/Utils/MathUtils.h"
#include "BaldLion/ResourceManagement/ResourceManager.h"
#include "Serialization/AnimatorSerializer.h"


namespace BaldLion
{
	using namespace ResourceManagement;

	namespace Animation
	{
		HashTable<StringId, JointType> AnimationManager::s_jointMapping;
		HashTable<ui32, Animator*> AnimationManager::s_registeredAnimators;

		const char* AnimationManager::s_jointNames[65] = {
			"Hips",
			"Spine",
			"Spine1",
			"Spine2",
			"Neck",
			"Head",
			"HeadTop_End",
			"LeftShoulder",
			"LeftArm",
			"LeftForearm",
			"LeftHand",
			"LeftHandThumb1",
			"LeftHandThumb2",
			"LeftHandThumb3",
			"LeftHandThumb4",
			"LeftHandIndex1",
			"LeftHandIndex2",
			"LeftHandIndex3",
			"LeftHandIndex4",
			"LeftHandMiddle1",
			"LeftHandMiddle2",
			"LeftHandMiddle3",
			"LeftHandMiddle4",
			"LeftHandRing1",
			"LeftHandRing2",
			"LeftHandRing3",
			"LeftHandRing4",
			"LeftHandPinky1",
			"LeftHandPinky2",
			"LeftHandPinky3",
			"LeftHandPinky4",
			"RightShoulder",
			"RightArm",
			"RightForearm",
			"RightHand",
			"RightHandThumb1",
			"RightHandThumb2",
			"RightHandThumb3",
			"RightHandThumb4",
			"RightHandIndex1",
			"RightHandIndex2",
			"RightHandIndex3",
			"RightHandIndex4",
			"RightHandMiddle1",
			"RightHandMiddle2",
			"RightHandMiddle3",
			"RightHandMiddle4",
			"RightHandRing1",
			"RightHandRing2",
			"RightHandRing3",
			"RightHandRing4",
			"RightHandPinky1",
			"RightHandPinky2",
			"RightHandPinky3",
			"RightHandPinky4",
			"LeftUpLeg",
			"LeftLeg",
			"LeftFoot",
			"LeftToeBase",
			"LeftToe_End",
			"RightUpLeg",
			"RightLeg",
			"RightFoot",
			"RightToeBase",
			"RightToe_End"
		};

		bool AnimationManager::s_initialized = false;

		void AnimationManager::Init()
		{
			if (!s_initialized)
			{
				s_initialized = true;
				s_registeredAnimators = HashTable<ui32, Animator*>(AllocationType::FreeList_Main, 16);
				s_jointMapping = HashTable<StringId, JointType>(AllocationType::FreeList_Main,128);

				for (ui32 i = 0; i < 65; ++i)
				{					
					s_jointMapping.Emplace(BL_STRING_TO_STRINGID(s_jointNames[i]), (JointType)i);
				}
			}
		}

		void AnimationManager::Stop()
		{	
			s_initialized = false;
			s_registeredAnimators.DeleteNoDestructor();
		}

		void AnimationManager::GenerateAnimator(const aiScene* scene, std::string& animatorPath)
		{
			Animator* animator = nullptr;
			std::string animatorName = animatorPath + ResourceManager::GetResourceSuffixFromType(ResourceType::Animator);
			if (!s_registeredAnimators.TryGet(BL_STRING_TO_STRINGID(animatorName), animator))
			{
				animator = MemoryManager::New<Animator>(animatorName.c_str(), AllocationType::FreeList_Resources, animatorName);

				ResourceManagement::ResourceManager::AddResource(animator);
				RegisterAnimator(animator);
			}

			if (!ResourceManagement::ResourceManager::HasMetafile(animator))
			{
				SerializeAnimator(animator->GetResourceID());
			}
		}

		void AnimationManager::GenerateAnimations(const aiScene* scene, const std::string& animationFolderPath)
		{
			if (scene->HasAnimations())
			{
				glm::mat4 rootTransform = MathUtils::AiMat4ToGlmMat4(scene->mRootNode->mTransformation);

				for (ui32 i = 0; i < scene->mNumAnimations; ++i)
				{
					const char* animationName = strlen(scene->mAnimations[i]->mName.C_Str()) == 0 ? "unnamedAnimation" : scene->mAnimations[i]->mName.C_Str();

					std::string animationPath = animationFolderPath + animationName + ResourceManager::GetResourceSuffixFromType(ResourceType::Animation);

					AnimationClip* animationClip = MemoryManager::New<AnimationClip>(animationPath.c_str(), AllocationType::FreeList_Resources,
						animationPath,
						glm::inverse(rootTransform),
						(ui32)scene->mAnimations[i]->mChannels[0]->mNumPositionKeys,
						(float)(scene->mAnimations[i]->mDuration / scene->mAnimations[i]->mTicksPerSecond)
					);
					ResourceManagement::ResourceManager::AddResource(animationClip);

					const float timeStamp = (float)(1.0f / scene->mAnimations[i]->mTicksPerSecond);

					for (ui32 j = 0; j < scene->mAnimations[i]->mNumChannels; ++j)
					{
						JointType jointNodeIndex = JointType::Count;

						if (s_jointMapping.TryGet(AnimationManager::BoneNameToJointName(scene->mAnimations[i]->mChannels[j]->mNodeName.data), jointNodeIndex)) {

							//Calculating position ranges;
							float minXPos = scene->mAnimations[i]->mChannels[j]->mPositionKeys[0].mValue.x, maxXPos = scene->mAnimations[i]->mChannels[j]->mPositionKeys[0].mValue.x;
							float minYPos = scene->mAnimations[i]->mChannels[j]->mPositionKeys[0].mValue.y, maxYPos = scene->mAnimations[i]->mChannels[j]->mPositionKeys[0].mValue.y;
							float minZPos = scene->mAnimations[i]->mChannels[j]->mPositionKeys[0].mValue.z, maxZPos = scene->mAnimations[i]->mChannels[j]->mPositionKeys[0].mValue.z;
							float xPosDelta = 0.0f, yPosDelta = 0.0f, zPosDelta = 0.0f;

							for (ui32 k = 0; k < scene->mAnimations[i]->mChannels[j]->mNumPositionKeys; ++k)
							{
								float xValue = scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.x;
								float yValue = scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.y;
								float zValue = scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.z;

								if (xValue < minXPos) minXPos = xValue;
								if (xValue > maxXPos) maxXPos = xValue;

								if (yValue < minYPos) minYPos = yValue;
								if (yValue > maxYPos) maxYPos = yValue;

								if (zValue < minZPos) minZPos = zValue;
								if (zValue > maxZPos) maxZPos = zValue;
							}

							xPosDelta = glm::abs(maxXPos - minXPos);
							yPosDelta = glm::abs(maxYPos - minYPos);
							zPosDelta = glm::abs(maxZPos - minZPos);

							//Calculating scale ranges;
							float minXScale = scene->mAnimations[i]->mChannels[j]->mScalingKeys[0].mValue.x, maxXScale = scene->mAnimations[i]->mChannels[j]->mScalingKeys[0].mValue.x;
							float minYScale = scene->mAnimations[i]->mChannels[j]->mScalingKeys[0].mValue.y, maxYScale = scene->mAnimations[i]->mChannels[j]->mScalingKeys[0].mValue.y;
							float minZScale = scene->mAnimations[i]->mChannels[j]->mScalingKeys[0].mValue.z, maxZScale = scene->mAnimations[i]->mChannels[j]->mScalingKeys[0].mValue.z;
							float xScaleDelta = 0.0f, yScaleDelta = 0.0f, zScaleDelta = 0.0f;

							for (ui32 k = 0; k < scene->mAnimations[i]->mChannels[j]->mNumScalingKeys; ++k)
							{
								float xValue = scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.x;
								float yValue = scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.y;
								float zValue = scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.z;

								if (xValue < minXScale) minXScale = xValue;
								if (xValue > maxXScale) maxXScale = xValue;

								if (yValue < minYScale) minYScale = yValue;
								if (yValue > maxYScale) maxYScale = yValue;

								if (zValue < minZScale) minZScale = zValue;
								if (zValue > maxZScale) maxZScale = zValue;
							}

							xScaleDelta = glm::abs(maxXScale - minXScale);
							yScaleDelta = glm::abs(maxYScale - minYScale);
							zScaleDelta = glm::abs(maxZScale - minZScale);

							animationClip->AnimationTracks[(ui32)jointNodeIndex].TrackStartIndex = animationClip->AnimationData.Size();
							animationClip->AnimationTracks[(ui32)jointNodeIndex].PositionIsStatic = xPosDelta <= glm::epsilon<float>() && yPosDelta <= glm::epsilon<float>() && zPosDelta <= glm::epsilon<float>();
							animationClip->AnimationTracks[(ui32)jointNodeIndex].ScaleIsStatic = xScaleDelta <= glm::epsilon<float>() && yScaleDelta <= glm::epsilon<float>() && zScaleDelta <= glm::epsilon<float>();

							animationClip->AnimationTracks[(ui32)jointNodeIndex].PositionRangeX = { minXPos, maxXPos };
							animationClip->AnimationTracks[(ui32)jointNodeIndex].PositionRangeY = { minYPos, maxYPos };
							animationClip->AnimationTracks[(ui32)jointNodeIndex].PositionRangeZ = { minZPos, maxZPos };

							animationClip->AnimationTracks[(ui32)jointNodeIndex].ScaleRangeX = { minXScale, maxXScale };
							animationClip->AnimationTracks[(ui32)jointNodeIndex].ScaleRangeY = { minYScale, maxYScale };
							animationClip->AnimationTracks[(ui32)jointNodeIndex].ScaleRangeZ = { minZScale, maxZScale };

							ui32 frames = animationClip->NumFrames;
							for (ui32 k = 0; k < frames; ++k)
							{
								//Pushing rotation
								ui16 valueA = 0, valueB = 0, valueC = 0;

								if (k < scene->mAnimations[i]->mChannels[j]->mNumRotationKeys)
								{
									Compression::CompressSmallestThree(
										scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.w,
										scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.x,
										scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.y,
										scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.z,
										valueA,
										valueB,
										valueC);
								}
								else
								{
									//If there are less rotationkeys than frames, we take the last three entries
									valueA = animationClip->AnimationData[animationClip->AnimationData.Size() - 3];
									valueB = animationClip->AnimationData[animationClip->AnimationData.Size() - 2];
									valueC = animationClip->AnimationData[animationClip->AnimationData.Size() - 1];
								}

								animationClip->AnimationData.EmplaceBack(valueA);
								animationClip->AnimationData.EmplaceBack(valueB);
								animationClip->AnimationData.EmplaceBack(valueC);
							}

							for (ui32 k = 0; k < frames; ++k)
							{
								if (k < scene->mAnimations[i]->mChannels[j]->mNumPositionKeys)
								{
									//Pushing position								
									animationClip->AnimationData.EmplaceBack(Compression::CompressFloatRL<16>(scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.x, minXPos, maxXPos));
									animationClip->AnimationData.EmplaceBack(Compression::CompressFloatRL<16>(scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.y, minYPos, maxYPos));
									animationClip->AnimationData.EmplaceBack(Compression::CompressFloatRL<16>(scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.z, minZPos, minZPos));

									if (animationClip->AnimationTracks[(ui32)jointNodeIndex].PositionIsStatic)
									{
										break;
									}
								}
								else
								{

									//If there are less positionkeys than frames, we take the last three entries
									ui16 valueX = animationClip->AnimationData[animationClip->AnimationData.Size() - 3];
									ui16 valueY = animationClip->AnimationData[animationClip->AnimationData.Size() - 2];
									ui16 valueZ = animationClip->AnimationData[animationClip->AnimationData.Size() - 1];

									animationClip->AnimationData.EmplaceBack(valueX);
									animationClip->AnimationData.EmplaceBack(valueY);
									animationClip->AnimationData.EmplaceBack(valueZ);
								}
							}

							for (ui32 k = 0; k < frames; ++k)
							{
								if (k < scene->mAnimations[i]->mChannels[j]->mNumScalingKeys)
								{
									//Pushing scale
									animationClip->AnimationData.EmplaceBack(Compression::CompressFloatRL<16>(scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.x, minXScale, maxXScale));
									animationClip->AnimationData.EmplaceBack(Compression::CompressFloatRL<16>(scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.y, minYScale, maxYScale));
									animationClip->AnimationData.EmplaceBack(Compression::CompressFloatRL<16>(scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.z, minZScale, maxZScale));

									if (animationClip->AnimationTracks[(ui32)jointNodeIndex].ScaleIsStatic)
									{
										break;
									}
								}
								else
								{
									//If there are less positionkeys than frames, we take the last three entries
									ui16 valueX = animationClip->AnimationData[animationClip->AnimationData.Size() - 3];
									ui16 valueY = animationClip->AnimationData[animationClip->AnimationData.Size() - 2];
									ui16 valueZ = animationClip->AnimationData[animationClip->AnimationData.Size() - 1];

									animationClip->AnimationData.EmplaceBack(valueX);
									animationClip->AnimationData.EmplaceBack(valueY);
									animationClip->AnimationData.EmplaceBack(valueZ);
								}
							}
						}
					}
				}

			}
		}

		StringId AnimationManager::BoneNameToJointName(const char* boneName)
		{
			std::string lowerNodeName = boneName;
			std::transform(lowerNodeName.begin(), lowerNodeName.end(), lowerNodeName.begin(), ::tolower);
			size_t pos = lowerNodeName.rfind(':');
			lowerNodeName = lowerNodeName.substr(pos + 1);

			for (i32 i = ((i32)JointType::Count) - 1; i >= 0; --i)
			{
				std::string lowerJointName = AnimationManager::s_jointNames[i];
				std::transform(lowerJointName.begin(), lowerJointName.end(), lowerJointName.begin(), ::tolower);

				if (lowerNodeName == lowerJointName)
				{
					return BL_STRING_TO_STRINGID(AnimationManager::s_jointNames[i]);
				}
			}

			return 0;
		}


		Animator* AnimationManager::GetAnimator(const ui32 animatorID)
		{
			Animator* result = nullptr;

			if (s_registeredAnimators.TryGet(animatorID, result)) {
				return result;
			}

			return nullptr;
		}

		void AnimationManager::RegisterAnimator(Animator* animator)
		{
			if (!s_registeredAnimators.Contains(animator->GetResourceID()))
			{
				s_registeredAnimators.Emplace(animator->GetResourceID(), std::move(animator));
			}
		}

		void AnimationManager::UnregisterAnimator(Animator* animator)
		{	
			if (!s_registeredAnimators.Contains(animator->GetResourceID()))
			{				
				s_registeredAnimators.Remove(animator->GetResourceID());
			}
		}

		void AnimationManager::SerializeAnimator(const ui32 animatorID)
		{
			Animator* animatorToSerialize = nullptr;

			if (s_registeredAnimators.TryGet(animatorID, animatorToSerialize)) {

				std::string animatorResourcePath = BL_STRINGID_TO_STRING(animatorToSerialize->GetResourcePath()) + ResourceManager::GetResourceSuffixFromType(ResourceType::Meta);
				AnimatorSerializer::SerializeAnimator(animatorToSerialize, animatorResourcePath.c_str());
			}
		}

		void AnimationManager::DeserializeAnimator(const char* animatorMetaFilePath)
		{			
			AnimatorSerializer::DeserializeAnimator(animatorMetaFilePath);			
		}

		

	}
}