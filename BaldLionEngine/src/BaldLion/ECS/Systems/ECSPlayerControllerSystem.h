#pragma once
#include "BaldLion/ECS/ECSSystem.h"

namespace BaldLion
{
	namespace ECS
	{
		class ECSPlayerControllerSystem : public ECSSystem
		{
		public:
			ECSPlayerControllerSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager);

			virtual void OnStart() override {};
			virtual void OnStop() override {}			
			virtual void UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime) override;
		};
	}
}