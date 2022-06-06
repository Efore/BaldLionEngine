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
		
		void ECSAnimationSystem::UpdateOperation(ECSComponentLookUp* componentLookUp)
		{
			BL_PROFILE_FUNCTION();

			ECSSkeletonComponent* entitySkeleton = componentLookUp->Write<ECSSkeletonComponent>(ECSComponentType::Skeleton);
			ECSAnimationComponent* animationComponent = componentLookUp->Write<ECSAnimationComponent>(ECSComponentType::Animation);
			
			const Animator* entityAnimator = AnimationManager::GetAnimator(animationComponent->animatorID);
			const AnimationClip* currentAnimation = entityAnimator->GetAnimationClip(animationComponent->currentAnimationID);
			 
			animationComponent->currentAnimationTime = glm::mod(animationComponent->currentAnimationTime + animationComponent->timer->GetDeltaTime(), currentAnimation->AnimationTimeLength);

			const AnimatorTransition* currentTransition = entityAnimator->CheckTransition(animationComponent->currentAnimationID, animationComponent->currentAnimationTime, animationComponent->animatorParameters);

			float currentTransitionTime = 0.0f;

			if (currentTransition != nullptr)
			{
				animationComponent->currentTransitionTime += animationComponent->timer->GetDeltaTime();
				currentTransitionTime = animationComponent->currentTransitionTime;

				if (animationComponent->currentTransitionTime >= currentTransition->GetTransitionTime())
				{
					animationComponent->currentAnimationTime = animationComponent->currentTransitionTime;
					animationComponent->currentTransitionTime = 0.0f;
					animationComponent->currentAnimationID = currentTransition->GetFinalAnimationID();					
				}
			}

			JointTransform transforms[(ui32)JointType::Count];
			CalculateInterpolatedTransforms(animationComponent->currentAnimationTime, currentTransitionTime, currentAnimation, entityAnimator, currentTransition, transforms);

			for (ui32 i = 0; i < (ui32)JointType::Count; ++i)
			{
				const JointType parentJointType = entitySkeleton->joints[i].parentJointType;
				
				const glm::mat4& parentTransform = parentJointType == JointType::Count ? glm::mat4(1.0f) : entitySkeleton->joints[(ui32)parentJointType].jointLocalSpaceTransform;

				const glm::mat4& animationTransform = glm::translate(glm::mat4(1.0f), transforms[i].position) * glm::mat4_cast(transforms[i].rotation) * glm::scale(glm::mat4(1.0f), transforms[i].scale);
				
				entitySkeleton->joints[i].UpdateJointTransforms(currentAnimation->InverseRootTransform, parentTransform, animationTransform);
			}
		}	

		void ECSAnimationSystem::CalculateInterpolatedTransforms(float currentAnimationTime, float currentTransitionTime, const AnimationClip* animation, const Animator* animator, const AnimatorTransition* transition, JointTransform* result) const
		{
			BL_PROFILE_FUNCTION();

			ui32 frame = (ui32)(glm::floor((currentAnimationTime / animation->AnimationTimeLength) * animation->NumFrames));
			ui32 nextFrame = glm::min(animation->NumFrames - 1, frame + 1);

			float frameTime = (animation->AnimationTimeLength / animation->NumFrames) * frame;
			float nextFrameTime = (animation->AnimationTimeLength / animation->NumFrames) * nextFrame;

			float interpolant = (currentAnimationTime - frameTime) / (nextFrameTime - frameTime);

			//Setting the result as the interpolation between the previous and the next frame to the chosen point in the animation
			for (ui32 i = 0; i < (ui32)JointType::Count; ++i)
			{
				if (animation->AnimationTracks[i].TrackStartIndex == -1)
					continue;

				animation->GetFramePosition(i, frame, nextFrame, interpolant, result[i].position);
				animation->GetFrameRotation(i, frame, nextFrame, interpolant, result[i].rotation);
				animation->GetFrameScale(i, frame, nextFrame, interpolant, result[i].scale);
			}

			//If there is a transition to process
			if (currentTransitionTime > 0.0f)
			{
				//Getting the correct point in the final animation
				const AnimationClip* nextAnimation = animator->GetAnimationClip(transition->GetFinalAnimationID());

				JointTransform nextAnimationTransforms[(ui32)JointType::Count];

				frame = (ui32)(glm::floor((currentTransitionTime / nextAnimation->AnimationTimeLength) * nextAnimation->NumFrames));
				nextFrame = glm::min(nextAnimation->NumFrames - 1, frame + 1);

				frameTime = (nextAnimation->AnimationTimeLength / nextAnimation->NumFrames) * frame;
				nextFrameTime = (nextAnimation->AnimationTimeLength / nextAnimation->NumFrames) * nextFrame;

				interpolant = (currentTransitionTime - frameTime) / (nextFrameTime - frameTime);

				//Setting the transforms for the interpolation between the previous and the next frame to the chosen point in the final animation
				for (ui32 i = 0; i < (ui32)JointType::Count; ++i)
				{
					if (nextAnimation->AnimationTracks[i].TrackStartIndex == -1)
						continue;

					nextAnimation->GetFramePosition(i, frame, nextFrame, interpolant, nextAnimationTransforms[i].position);
					nextAnimation->GetFrameRotation(i, frame, nextFrame, interpolant, nextAnimationTransforms[i].rotation);
					nextAnimation->GetFrameScale(i, frame, nextFrame, interpolant, nextAnimationTransforms[i].scale);
				}

				//Getting the interpolant between both animations
				interpolant = glm::clamp(currentTransitionTime / transition->GetTransitionTime(), 0.0f, 1.0f);

				//Setting the result
				for (ui32 i = 0; i < (ui32)JointType::Count; ++i)
				{
					if (animation->AnimationTracks[i].TrackStartIndex == -1)
						continue;

					result[i].position = (glm::mix(result[i].position, nextAnimationTransforms[i].position, interpolant));
					result[i].rotation = (glm::slerp(result[i].rotation, nextAnimationTransforms[i].rotation, interpolant));
					result[i].scale = (glm::mix(result[i].scale, nextAnimationTransforms[i].scale, interpolant));
				}
			}
		}
	}
}
