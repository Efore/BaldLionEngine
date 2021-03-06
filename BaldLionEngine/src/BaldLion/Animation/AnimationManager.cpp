#include "blpch.h"
#include "AnimationManager.h"
#include "BaldLion/Core/JobManagement/JobManager.h"

namespace BaldLion
{
	namespace Animation
	{
		DynamicArray<Animator*> AnimationManager::s_registeredAnimators;
		std::mutex AnimationManager::s_animationManagerMutex;

		bool AnimationManager::s_initialized = false;

		void AnimationManager::Init()
		{
			if (!s_initialized)
			{
				s_initialized = true;
				s_registeredAnimators = DynamicArray<Animator*>(AllocationType::FreeList_Renderer, 10);
			}
		}

		void AnimationManager::Stop()
		{	
			s_initialized = false;
			for (ui32 i = 0; i < s_registeredAnimators.Size(); ++i)
			{
				MemoryManager::DeleteNoDestructor(s_registeredAnimators[i]);
			}
			s_registeredAnimators.Clear();
		}

		void AnimationManager::OnUpdate(float timeStep)
		{
			for (ui32 i = 0; i < s_registeredAnimators.Size(); ++i)
			{
				s_registeredAnimators[i]->OnUpdate(timeStep);
			}
		}

		void AnimationManager::OnParallelUpdate(float timeStep, StringId& paralellJobID)
		{
			JobManagement::Job animationParallelUpdate("AnimationManagerParallelUpdate",0u, s_registeredAnimators.Size());			
			paralellJobID = animationParallelUpdate.JobID;

			animationParallelUpdate.Task = [paralellJobID, timeStep] {

				for (ui32 i = 0; i < s_registeredAnimators.Size(); ++i)
				{
					Animator* animatorToUpdate = s_registeredAnimators[i];

					const std::string animatorName = "Animator" + std::to_string(i);

					JobManagement::Job animationJob(animatorName.c_str(), paralellJobID);
					animationJob.Task = [animatorToUpdate, timeStep] { 
						BL_PROFILE_SCOPE("AnimationManager::OnUpdateAnimation");
						animatorToUpdate->OnUpdate(timeStep); 
					};

					JobManagement::JobManager::QueueJob(animationJob);
				}
			};

			JobManagement::JobManager::QueueJob(animationParallelUpdate);
		}

		void AnimationManager::GenerateAnimator(const aiScene *scene, const HashTable<StringId, ui32>& jointMapping, SkinnedMesh* animatedMesh)
		{
			if (scene->HasAnimations())
			{
				DynamicArray<AnimationData> animations(AllocationType::FreeList_Renderer, scene->mNumAnimations);
				for (ui32 i = 0; i < scene->mNumAnimations; ++i)
				{
					AnimationData animationData(
						STRING_TO_ID(scene->mAnimations[i]->mName.data), 
						DynamicArray<KeyFrame>(AllocationType::FreeList_Renderer, (int)scene->mAnimations[i]->mChannels[0]->mNumPositionKeys), 
						(float)(scene->mAnimations[i]->mDuration / scene->mAnimations[i]->mTicksPerSecond)
					);

					float TimeStamp = (float)(1.0f / scene->mAnimations[i]->mTicksPerSecond);

					for (size_t j = 0; j < (int)scene->mAnimations[i]->mChannels[0]->mNumPositionKeys; ++j)
					{
						KeyFrame keyFrame(DynamicArray<JointTransform>(AllocationType::FreeList_Renderer, glm::max((int)scene->mAnimations[i]->mNumChannels, (int)jointMapping.Size())), glm::min(TimeStamp * j, animationData.AnimationLength));
						keyFrame.JointTranforms.Fill();

						for (size_t k = 0; k < scene->mAnimations[i]->mNumChannels; ++k)
						{
							const ui32 jointNodeIndex = jointMapping.Get(STRING_TO_ID(scene->mAnimations[i]->mChannels[k]->mNodeName.data));
							keyFrame.JointTranforms[jointNodeIndex] = JointTransform(
								glm::vec3(scene->mAnimations[i]->mChannels[k]->mPositionKeys[j].mValue.x, scene->mAnimations[i]->mChannels[k]->mPositionKeys[j].mValue.y, scene->mAnimations[i]->mChannels[k]->mPositionKeys[j].mValue.z),
								glm::quat(scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.w, scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.x, scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.y, scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.z)
							);
						}

						animationData.AnimationFrames.EmplaceBack(std::move(keyFrame));
					}

					animations.EmplaceBack(std::move(animationData));
				}

				glm::mat4 rootTransform = SkinnedMesh::AiMat4ToGlmMat4(scene->mRootNode->mTransformation);

				Animator* animator = MemoryManager::New<Animator>(STRING_TO_ID("Animator"), AllocationType::FreeList_Renderer, animatedMesh, animations, rootTransform);
				RegisterAnimator(animator);
			}
		}

		void AnimationManager::RegisterAnimator(Animator* animator)
		{
			std::lock_guard<std::mutex> lockGuard(s_animationManagerMutex);

			if (!s_registeredAnimators.Exists(animator))
			{
				s_registeredAnimators.PushBack(animator);
			}
		}

		void AnimationManager::UnregisterAnimator(Animator* animator)
		{			
			std::lock_guard<std::mutex> lockGuard(s_animationManagerMutex);

			if (s_registeredAnimators.Exists(animator))
			{				
				s_registeredAnimators.RemoveFast(animator);
			}
		}
	}
}