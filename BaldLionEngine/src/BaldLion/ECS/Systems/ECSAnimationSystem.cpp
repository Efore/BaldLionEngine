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
			const AnimationData* currentAnimation = entityAnimator->GetAnimation(entityAnimation->currentAnimationID);
			 
			entityAnimation->currentAnimationTime = glm::mod(entityAnimation->currentAnimationTime + entityAnimation->timer->GetDeltaTime(), currentAnimation->AnimationLength);

			const AnimatorTransition* currentTransition = entityAnimator->CheckTransition(entityAnimation->currentAnimationID, entityAnimation->currentAnimationTime);

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

			DynamicArray<JointTransform> transforms;
			entityAnimator->CalculateInterpolatedTransforms(entityAnimation->currentAnimationTime, entityAnimation->currentTransitionTime, currentAnimation, currentTransition, transforms);

			BL_DYNAMICARRAY_FOR(i, transforms, 0)			
			{
				const i32 parentID = entitySkeleton->joints[i].parentID;

				const glm::mat4& parentTransform = parentID == -1 ? glm::mat4(1.0f) : entitySkeleton->joints[parentID].jointModelSpaceTransform;

				const glm::mat4& animationTransform = glm::translate(glm::mat4(1.0f), transforms[i].GetDecompressedPosition()) * glm::mat4_cast(transforms[i].GetDecompressedRotation()) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

				entitySkeleton->joints[i].UpdateJointTransforms(currentAnimation->InverseRootTransform, parentTransform, animationTransform);
			}
		}	
	}
}
