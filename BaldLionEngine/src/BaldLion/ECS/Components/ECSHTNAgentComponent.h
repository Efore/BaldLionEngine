#pragma once

#include "BaldLion/ECS/ECSComponent.h"
#include "BaldLion/AI/HTN/HTNManager.h"

namespace BaldLion
{
	namespace ECS
	{
		class ECSHTNAgentComponent : public ECSComponent {

		public:

			ECSHTNAgentComponent(StringId worldStateBlackboardID, StringId initialDomainID) :
				ECSComponent(ECSComponentType::HTNAgent)
			{
				htnAgentIdx = AI::HTN::HTNManager::RequestAgent(worldStateBlackboardID, initialDomainID);
			}

			~ECSHTNAgentComponent()
			{
				AI::HTN::HTNManager::DeactivateAgent(htnAgentIdx);
			}

		public:
			StringId initialDomainID;
			StringId worldStateBlackboardID;
			i32 htnAgentIdx;
		};
	}
}
