#pragma once
#include "BaldLion/ECS/ECSSystem.h"

#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/ECS/Components/ECSMeshRendererComponent.h"


namespace BaldLion {

	namespace ECS {

		class ECSRenderSystem : public ECSSystem
		{
		public:
			ECSRenderSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager, bool blockSystemsLoop) :
				ECSSystem(systemName, signature, ecsManager, blockSystemsLoop, false) {}

			virtual void OnStart() override;
			virtual void OnUpdate(TimeStep timeStep) override;
			virtual void OnStop() override {};

		private:

			 void AddToBatch(ECSMeshRendererComponent* mesh);
			 void ProcessFrustrumCullingParallel(ui32 initialMeshIndex, ui32 finalMeshIndex);

			 void CreateShadowMap();
			 void RenderStatictGeometry();
			 void RenderDynamicGeometry();
		};

	}
}

