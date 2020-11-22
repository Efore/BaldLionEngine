#include "blpch.h"
#include "AnimationManager.h"

namespace BaldLion
{
	namespace Animation
	{
		DynamicArray<Animator*> AnimationManager::s_registeredAnimators;

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
			for (size_t i = 0; i < s_registeredAnimators.Size(); ++i)
			{
				MemoryManager::DeleteNoDestructor(s_registeredAnimators[i]);
			}
			s_registeredAnimators.Clear();
		}

		void AnimationManager::OnUpdate(float timeStep)
		{
			for (size_t i = 0; i < s_registeredAnimators.Size(); ++i)
			{
				s_registeredAnimators[i]->OnUpdate(timeStep);
			}
		}

		void AnimationManager::GenerateAnimator(const aiScene *scene, const std::unordered_map<std::string, uint32_t>& jointMapping, SkinnedMesh* animatedMesh)
		{
			if (scene->HasAnimations())
			{
				DynamicArray<AnimationData> animations(AllocationType::FreeList_Renderer, scene->mNumAnimations);
				for (size_t i = 0; i < scene->mNumAnimations; ++i)
				{
					AnimationData animationData;

					strcpy(animationData.animationName, scene->mAnimations[i]->mName.data);
					animationData.animationLength = (float)(scene->mAnimations[i]->mDuration / scene->mAnimations[i]->mTicksPerSecond);
					animationData.frames = DynamicArray<KeyFrame>(AllocationType::FreeList_Renderer, (int)scene->mAnimations[i]->mChannels[0]->mNumPositionKeys);	
					

					float timeStamp = (float)(1.0f / scene->mAnimations[i]->mTicksPerSecond);

					for (size_t j = 0; j < (int)scene->mAnimations[i]->mChannels[0]->mNumPositionKeys; ++j)
					{
						KeyFrame keyFrame;
						keyFrame.timeStamp = glm::min(timeStamp * j, animationData.animationLength);
						keyFrame.jointTranforms = DynamicArray<JointTransform>(AllocationType::FreeList_Renderer, glm::max((int)scene->mAnimations[i]->mNumChannels, (int)jointMapping.size()));
						keyFrame.jointTranforms.Fill();

						for (size_t k = 0; k < scene->mAnimations[i]->mNumChannels; ++k)
						{
							keyFrame.jointTranforms[jointMapping.at(scene->mAnimations[i]->mChannels[k]->mNodeName.data)] = JointTransform(
								glm::vec3(scene->mAnimations[i]->mChannels[k]->mPositionKeys[j].mValue.x, scene->mAnimations[i]->mChannels[k]->mPositionKeys[j].mValue.y, scene->mAnimations[i]->mChannels[k]->mPositionKeys[j].mValue.z),
								glm::quat(scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.w, scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.x, scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.y, scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.z)
							);
						}

						animationData.frames.PushBack(std::move(keyFrame));
					}

					animations.PushBack(std::move(animationData));
				}

				glm::mat4 rootTransform = SkinnedMesh::AiMat4ToGlmMat4(scene->mRootNode->mTransformation);

				Animator* animator = MemoryManager::New<Animator>("Animator", AllocationType::FreeList_Renderer, animatedMesh, animations, rootTransform);
				RegisterAnimator(animator);
			}
		}

		void AnimationManager::RegisterAnimator(Animator* animator)
		{
			if (!s_registeredAnimators.Exists(animator))
			{
				s_registeredAnimators.PushBack(animator);
			}
		}

		void AnimationManager::UnregisterAnimator(Animator* animator)
		{			
			if (s_registeredAnimators.Exists(animator))
			{				
				s_registeredAnimators.RemoveFast(animator);
			}
		}
	}
}