#include "blpch.h"
#include "ECSAnimationSystem.h"
#include "BaldLion/Animation/Animator.h"
#include "BaldLion/Animation/AnimationManager.h"

#include "BaldLion/ECS/Components/ECSAnimationComponent.h"
#include "BaldLion/ECS/Components/ECSSkeletonComponent.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace BaldLion {

	namespace ECS {
		
		void ECSAnimationSystem::UpdateOperation(TimeStep timeStep, ECSComponentLookUp* componentLookUp)
		{
			BL_PROFILE_FUNCTION();

			ECSSkeletonComponent* entitySkeleton = componentLookUp->Write<ECSSkeletonComponent>(ECSComponentType::Skeleton);
			ECSAnimationComponent* entityAnimation = componentLookUp->Write<ECSAnimationComponent>(ECSComponentType::Animation);
			
			const Animator* entityAnimator = AnimationManager::GetAnimator(entityAnimation->animatorID);
			const AnimationData* currentAnimation = entityAnimator->GetAnimation(entityAnimation->currentAnimationID);

			entityAnimation->currentAnimationProgress = glm::mod(entityAnimation->currentAnimationProgress + timeStep, currentAnimation->AnimationLength);			

			DynamicArray<JointTransform> transforms;
			CalculateInterpolatedTransforms(entityAnimation->currentAnimationProgress, currentAnimation, transforms);

			for (ui32 i = 0; i < transforms.Size(); ++i)
			{
				const i32 parentID = entitySkeleton->joints[i].parentID;

				const glm::mat4& parentTransform = parentID == -1 ? glm::mat4(1.0f) : entitySkeleton->joints[parentID].jointModelSpaceTransform;

				const glm::mat4& animationTransform = glm::translate(glm::mat4(1.0f), transforms[i].GetDecompressedPosition()) * glm::mat4_cast(transforms[i].GetDecompressedRotation()) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

				entitySkeleton->joints[i].UpdateJointTransforms(currentAnimation->InverseRootTransform, parentTransform, animationTransform);
			}
		}

		void ECSAnimationSystem::CalculateInterpolatedTransforms(float progress, const AnimationData* animation, DynamicArray<JointTransform>& result)
		{
			BL_PROFILE_FUNCTION();

			int prevFrameIndex = 0;
			int nextFrameIndex = 0;

			for (ui32 i = 1; i < animation->AnimationFrames.Size(); ++i)
			{
				if (progress > animation->AnimationFrames[i].TimeStamp)
					continue;

				prevFrameIndex = i - 1;
				nextFrameIndex = i;

				break;
			}

			const float interpolant = (progress - animation->AnimationFrames[prevFrameIndex].TimeStamp) / (animation->AnimationFrames[nextFrameIndex].TimeStamp - animation->AnimationFrames[prevFrameIndex].TimeStamp);

			result = DynamicArray<JointTransform>(AllocationType::Linear_Frame, animation->AnimationFrames[prevFrameIndex].JointTranforms);

			for (ui32 i = 0; i < result.Size(); ++i)
			{
				result[i].SetPosition(glm::mix(animation->AnimationFrames[prevFrameIndex].JointTranforms[i].GetDecompressedPosition(), animation->AnimationFrames[nextFrameIndex].JointTranforms[i].GetDecompressedPosition(), interpolant));
				result[i].SetRotation(glm::mix(animation->AnimationFrames[prevFrameIndex].JointTranforms[i].GetDecompressedRotation(), animation->AnimationFrames[nextFrameIndex].JointTranforms[i].GetDecompressedRotation(), interpolant));
			}
		}
	}
}
