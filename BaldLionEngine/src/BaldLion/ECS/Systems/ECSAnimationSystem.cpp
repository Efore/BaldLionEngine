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
			ECSAnimationComponent* entityAnimation = componentLookUp->Write<ECSAnimationComponent>(ECSComponentType::Animation);
			
			const Animator* entityAnimator = AnimationManager::GetAnimator(entityAnimation->animatorID);
			const AnimationClip* currentAnimation = entityAnimator->GetAnimation(entityAnimation->currentAnimationID);
			 
			entityAnimation->currentAnimationTime = glm::mod(entityAnimation->currentAnimationTime + entityAnimation->timer->GetDeltaTime(), currentAnimation->AnimationTimeLength);

			const AnimatorTransition* currentTransition = entityAnimator->CheckTransition(entityAnimation->currentAnimationID, entityAnimation->currentAnimationTime, entityAnimation->animatorParameters);

			if (currentTransition != nullptr)
			{
				entityAnimation->currentTransitionTime += entityAnimation->timer->GetDeltaTime();

				if (entityAnimation->currentTransitionTime >= currentTransition->GetTransitionTime())
				{
					entityAnimation->currentAnimationTime = entityAnimation->currentTransitionTime;
					entityAnimation->currentTransitionTime = -1.0f;
					entityAnimation->currentAnimationID = currentTransition->GetFinalAnimationID();					
				}
			}
			else {
				entityAnimation->currentTransitionTime = -1.0f;
			}

			JointTransform transforms[(ui32)JointType::Count];
			entityAnimator->CalculateInterpolatedTransforms(entityAnimation->currentAnimationTime, entityAnimation->currentTransitionTime, currentAnimation, currentTransition, transforms);

			for (ui32 i = 0; i < (ui32)JointType::Count; ++i)
			{
				const JointType parentJointType = entitySkeleton->joints[i].parentJointType;
				
				const glm::mat4& parentTransform = parentJointType == JointType::Count ? glm::mat4(1.0f) : entitySkeleton->joints[(ui32)parentJointType].jointLocalSpaceTransform;

				const glm::mat4& animationTransform = glm::translate(glm::mat4(1.0f), transforms[i].position) * glm::mat4_cast(transforms[i].rotation) * glm::scale(glm::mat4(1.0f), transforms[i].scale);
				
				entitySkeleton->joints[i].UpdateJointTransforms(currentAnimation->InverseRootTransform, parentTransform, animationTransform);
			}
		}	
	}
}
