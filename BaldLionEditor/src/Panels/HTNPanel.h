#pragma once
#include "SceneHierarchyPanel.h"
#include "BaldLion/AI/HTN/HTNManager.h"
#include "BaldLion/AI/HTN/HTNTypes.h"

using namespace BaldLion::AI::HTN;
namespace BaldLion {

	namespace Editor {

		class HTNPanel : public EditorPanel {

		public:
			HTNPanel();
			virtual void OnImGuiRender(float deltaTime) override;

		private:
			void RenderWorldStateBlackboards();

			void RenderTasks();
			void RenderEditTask(HTNTask& task);
			void RenderMethod(HTNMethod& method);

			void RenderDomains();
			void RenderEditDomain(StringId domainID, HTNDomain& domain);

			//void RenderWorldStateCondition(HTNWorldStateCondition& condition);
			
			int imguiID = 0;
		};
	}
}

