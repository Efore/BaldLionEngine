#pragma once
#include "BaldLion/ECS/ECSSystem.h"

namespace BaldLion
{
	namespace ECS
	{
		class ECSLocomotionSystem : public ECSSystem
		{
		public:
			ECSLocomotionSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager);

			virtual void OnStart() override {};
			virtual void OnStop() override {}			
			virtual void UpdateComponents(ECSComponentLookUp* componentLookUp) override;
		};
	}
}
