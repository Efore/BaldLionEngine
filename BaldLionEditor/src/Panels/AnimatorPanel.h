#pragma once
#include "BaldLion/SceneManagement/Scene.h"
#include "EditorPanel.h"
#include "BaldLion/Animation/AnimationManager.h"

using namespace BaldLion::SceneManagement;

namespace BaldLion {

	namespace Editor {

		class AnimatorPanel : public EditorPanel {

		public:
			AnimatorPanel();

			virtual void OnImGuiRender() override;

		private:

			void RenderParameters();
			void RenderAnimationNodes();
			void RenderAddTransitionPopup(const char* popupName, const char* initialAnimationPath, ui32 initialAnimationID);
			void RenderSetupTransitionPopup(const char* popupID, const char* initialAnimationPath, const char* finalAnimationPath, AnimatorTransition* transition);
			void RenderParameter(AnimatorParameter& parameter, StringId parameterName);
			void RenderConditionPopup(AnimatorTransition* transition);

		private:

			 Animator* m_currentAnimator = nullptr;
			 AnimationData* m_selectedAnimation = nullptr;
			 AnimatorTransition* m_selectedTransition = nullptr;
		};
	}
}
