#pragma once
#include "BaldLion/ECS/ECSSystem.h"

namespace BaldLion
{
	namespace ECS
	{
		class ECSCameraFollowSystem : public ECSSystem
		{
		public:
			ECSCameraFollowSystem(const char* systemName, ECSSystemType systemType,const ECSSignature& signature, class ECSManager* ecsManager);

			virtual void OnStart() override {};
			virtual void OnStop() override {}			
			virtual void UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime) override;

#if BL_PROFILE_ACTIVE
			virtual void OnUpdate(float deltaTime) override
			{
				BL_PROFILE_FUNCTION();
				ECSSystem::OnUpdate(deltaTime);
			}
#endif

		private:			
			void CalculateInputRotation(class ECSCameraFollowComponent* cameraFollowComponent, float deltaTime);
			
		};
	}
}
