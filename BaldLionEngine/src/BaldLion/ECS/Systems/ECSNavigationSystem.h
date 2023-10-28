#pragma once
#include "BaldLion/ECS/ECSSystem.h"

namespace BaldLion 
{
	namespace ECS 
	{
		class ECSNavigationSystem : public ECSSystem
		{
		public:
			ECSNavigationSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager);

			virtual void OnStart() override {};
			virtual void OnStop() override {}
			virtual void OnUpdate(float deltaTime) override;
			virtual void UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime) override;
		};
	}
}
