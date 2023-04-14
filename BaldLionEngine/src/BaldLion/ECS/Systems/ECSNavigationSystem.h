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
			virtual void OnUpdate() override;
			virtual void UpdateComponents(ECSComponentLookUp* componentLookUp) override;			
		};
	}
}
