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

		void AnimationManager::GenerateAnimator(const aiScene *scene, std::string& animatorPath, const HashTable<StringId, ui32>& jointMapping)
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

					AnimationData* animationData = MemoryManager::New<AnimationData>(animationPath.c_str(), AllocationType::FreeList_Resources,
						glm::inverse(rootTransform),
						animationPath,
						(ui32)scene->mAnimations[i]->mChannels[0]->mNumPositionKeys,
						(float)(scene->mAnimations[i]->mDuration / scene->mAnimations[i]->mTicksPerSecond)
					);
					ResourceManagement::ResourceManager::AddResource(animationData);

					const float timeStamp = (float)(1.0f / scene->mAnimations[i]->mTicksPerSecond);

					for (ui32 j = 0; j < (int)scene->mAnimations[i]->mChannels[0]->mNumPositionKeys; ++j)
					{
						KeyFrame keyFrame(DynamicArray<JointTransform>(Memory::MemoryManager::GetAllocatorType(animator), glm::max((int)scene->mAnimations[i]->mNumChannels, (int)jointMapping.Size())), glm::min(timeStamp * j, animationData->AnimationLength));
						keyFrame.JointTranforms.Populate();

						for (ui32 k = 0; k < scene->mAnimations[i]->mNumChannels; ++k)
						{
							const ui32 jointNodeIndex = jointMapping.Get(BL_STRING_TO_STRINGID(scene->mAnimations[i]->mChannels[k]->mNodeName.data));
							keyFrame.JointTranforms[jointNodeIndex] = JointTransform(
								glm::vec3(scene->mAnimations[i]->mChannels[k]->mPositionKeys[j].mValue.x, scene->mAnimations[i]->mChannels[k]->mPositionKeys[j].mValue.y, scene->mAnimations[i]->mChannels[k]->mPositionKeys[j].mValue.z),
								glm::quat(scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.w, scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.x, scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.y, scene->mAnimations[i]->mChannels[k]->mRotationKeys[j].mValue.z)
							);
						}
						animationData->AnimationFrames.EmplaceBack(std::move(keyFrame));
					}
					animator->AddAnimation(animationData);
				}		

				if (!ResourceManagement::ResourceManager::HasMetafile(animator))
				{
					SerializeAnimator(animator->GetResourceID());
				}
			}
		}

		BaldLion::Animation::Animator* AnimationManager::GetAnimator(const ui32 animatorID)
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