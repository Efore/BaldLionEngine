#pragma once
#include "BaldLion/ECS/ECSSystem.h"

namespace BaldLion 
{
	namespace ECS 
	{
		class ECSPathfindingSystem : public ECSSystem
		{
		public:
			ECSPathfindingSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager) :
				ECSSystem(systemName, signature, ecsManager, true, true) {}

			virtual void UpdateComponents(ECSComponentLookUp* componentLookUp) override;			
		};
	}
}
