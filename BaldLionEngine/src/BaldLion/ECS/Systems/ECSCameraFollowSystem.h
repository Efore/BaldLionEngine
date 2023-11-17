#pragma once
#include "BaldLion/ECS/ECSSystem.h"

namespace BaldLion
{
	namespace ECS
	{
		class ECSCameraFollowSystem : public ECSSystem
		{
		public:
			ECSCameraFollowSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager);

			virtual void OnStart() override {};
			virtual void OnStop() override {}			
			virtual void UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime) override;

		private:
			void CenterCamera(float cameraRotationSpeed, float deltaTime, float& prevX, float& prevY);
		};
	}
}
