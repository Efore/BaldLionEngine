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

		HashTable<ui32, Animator*> AnimationManager::s_registeredAnimators;

		bool AnimationManager::s_initialized = false;

		void AnimationManager::Init()
		{
			if (!s_initialized)
			{
				s_initialized = true;
				s_registeredAnimators = HashTable<ui32, Animator*>(AllocationType::FreeList_Main, 10);
			}
		}

		void AnimationManager::Stop()
		{	
			s_initialized = false;
			s_registeredAnimators.DeleteNoDestructor();
		}

		void AnimationManager::GenerateAnimator(const aiScene *scene, std::string& animatorPath, const HashTable<StringId, JointType>& jointMapping)
		{
			if (scene->HasAnimations())
			{	
				std::string animatorName = animatorPath + ResourceManager::GetResourceSuffixFromType(ResourceType::Animator);
				
				glm::mat4 rootTransform = MathUtils::AiMat4ToGlmMat4(scene->mRootNode->mTransformation);

				Animator* animator = nullptr;

				if (!s_registeredAnimators.TryGet(BL_STRING_TO_STRINGID(animatorName), animator))
				{
					animator = MemoryManager::New<Animator>(animatorName.c_str(), AllocationType::FreeList_Resources, animatorName);

					ResourceManagement::ResourceManager::AddResource(animator);
					RegisterAnimator(animator);
				}

				for (ui32 i = 0; i < scene->mNumAnimations; ++i)
				{
					const char* animationName = strlen(scene->mAnimations[i]->mName.C_Str()) == 0 ? "unnamedAnimation" : scene->mAnimations[i]->mName.C_Str();

					std::string animationPath = animatorPath + "\\" + animationName + ResourceManager::GetResourceSuffixFromType(ResourceType::Animation);

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

						if (jointMapping.TryGet(BL_STRING_TO_STRINGID(scene->mAnimations[i]->mChannels[j]->mNodeName.data), jointNodeIndex)) {

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

					animator->AddAnimation(animationClip);
				}		

				if (!ResourceManagement::ResourceManager::HasMetafile(animator))
				{
					SerializeAnimator(animator->GetResourceID());
				}
			}
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