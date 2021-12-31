/*****************************************************************//**
 * \file   AnimationManager.cpp
 * \brief  Definition for AnimationManager's methods
 * 
 * \author Iván León Santiago
 * \date   April 2021
 *********************************************************************/

#include "blpch.h"
#include "AnimationManager.h"
#include "BaldLion/Core/JobManagement/JobManager.h"
#include "BaldLion/Utils/MathUtils.h"

namespace BaldLion
{
	namespace Animation
	{
	
		HashTable<StringId, Animator*> AnimationManager::s_registeredAnimators;
		std::mutex AnimationManager::s_animationManagerMutex;

		bool AnimationManager::s_initialized = false;

		void AnimationManager::Init()
		{
			if (!s_initialized)
			{
				s_initialized = true;
				s_registeredAnimators = HashTable<StringId, Animator*>(AllocationType::FreeList_Main, 10);
			}
		}

		void AnimationManager::Stop()
		{	
			s_initialized = false;
			s_registeredAnimators.DeleteNoDestructor();
		}

		void AnimationManager::GenerateAnimator(const aiScene *scene, const std::string& animatorName, const HashTable<StringId, ui32>& jointMapping)
		{
			if (scene->HasAnimations())
			{
				DynamicArray<AnimationData> animations(AllocationType::FreeList_Main, scene->mNumAnimations);

				glm::mat4 rootTransform = MathUtils::AiMat4ToGlmMat4(scene->mRootNode->mTransformation);

				for (ui32 i = 0; i < scene->mNumAnimations; ++i)
				{
					AnimationData animationData(
						glm::inverse(rootTransform),
						STRING_TO_STRINGID(scene->mAnimations[i]->mName.data), 
						DynamicArray<KeyFrame>(AllocationType::FreeList_Main, (int)scene->mAnimations[i]->mChannels[0]->mNumPositionKeys),
						(float)(scene->mAnimations[i]->mDuration / scene->mAnimations[i]->mTicksPerSecond)
					);

					const float timeStamp = (float)(1.0f / scene->mAnimations[i]->mTicksPerSecond);

					for (size_t j = 0; j < (int)scene->mAnimations[i]->mChannels[0]->mNumPositionKeys; ++j)
					{
						KeyFrame keyFrame(DynamicArray<JointTransform>(AllocationType::FreeList_Main, glm::max((int)scene->mAnimations[i]->mNumChannels, (int)jointMapping.Size())), glm::min(timeStamp * j, animationData.AnimationLength));
						keyFrame.JointTranforms.Populate();

						for (size_t k = 0; k < scene->mAnimations[i]->mNumChannels; ++k)
						{
							const ui32 jointNodeIndex = jointMapping.Get(STRING_TO_STRINGID(scene->mAnimations[i]->mChannels[k]->mNodeName.data));
							keyFrame.JointTranforms[jointNodeIndex] = JointTransform(
								glm::vec3(scene->mAnimations[i]->mChannels[k]->mPositionKeys[j].mValue.x, scene->mAnimations[i]->mChannels[k]->mPositionKeys[j].mValue.y, scene->mAnimations[i]->mChannels[k]->mPositionKeys[j].mValue.z),
								glm::quat(scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.w, scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.x, scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.y, scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.z)
							);
						}

						animationData.AnimationFrames.EmplaceBack(std::move(keyFrame));
					}

					animations.EmplaceBack(std::move(animationData));
				}				

				if (!s_registeredAnimators.Contains(STRING_TO_STRINGID(animatorName))) {

					Animator* animator = MemoryManager::New<Animator>("Animator", AllocationType::FreeList_Main, animations, STRING_TO_STRINGID(animatorName));		

					RegisterAnimator(animator);
				}
			}
		}

		BaldLion::Animation::Animator* AnimationManager::GetAnimator(const StringId animatorID)
		{
			Animator* result = nullptr;

			if (s_registeredAnimators.TryGet(animatorID, result)) {
				return result;
			}

			return nullptr;
		}

		void AnimationManager::RegisterAnimator(Animator* animator)
		{
			std::lock_guard<std::mutex> lockGuard(s_animationManagerMutex);

			if (!s_registeredAnimators.Contains(animator->GetAnimatorID()))
			{
				s_registeredAnimators.Emplace(animator->GetAnimatorID(), std::move(animator));
			}
		}

		void AnimationManager::UnregisterAnimator(Animator* animator)
		{			
			std::lock_guard<std::mutex> lockGuard(s_animationManagerMutex);

			if (!s_registeredAnimators.Contains(animator->GetAnimatorID()))
			{				
				s_registeredAnimators.Remove(animator->GetAnimatorID());
			}
		}
	}
}