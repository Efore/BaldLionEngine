#include "BaldLion/ECS/ECSSystem.h"

namespace BaldLion
{
	namespace ECS
	{
		class ECSHTNPlannerSystem : public ECSSystem
		{
		public:
			ECSHTNPlannerSystem(const char* systemName, ECSSystemType systemType, const ECSSignature& signature, class ECSManager* ecsManager);

			virtual void OnStart() override {};
			virtual void OnStop() override {}
			virtual void UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime) override;
		};
	}
}
