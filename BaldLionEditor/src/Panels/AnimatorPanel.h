#pragma once
#include "BaldLion/SceneManagement/Scene.h"
#include "EditorPanel.h"
#include "BaldLion/Animation/AnimationManager.h"

using namespace BaldLion::SceneManagement;

namespace BaldLion {

	namespace Editor {

		class AnimatorPanel : public EditorPanel {

		public:
			AnimatorPanel() = default;

			virtual void OnImGuiRender() override;

		private:

			 Animator* m_currentAnimator = nullptr;
			 AnimationData* m_selectedAnimation = nullptr;
			 AnimatorTransition* m_selectedTransition = nullptr;
		};
	}
}
