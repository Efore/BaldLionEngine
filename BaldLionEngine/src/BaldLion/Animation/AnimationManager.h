#pragma once
#include "Animator.h"
#include <vector>

namespace BaldLion
{
	namespace Animation
	{
		class AnimationManager
		{
		public:
			~AnimationManager();
			void OnUpdate(float timeStep);

			static AnimationManager* GetInstance();

			void RegisterAnimator(Ref<Animator> animator);
			void UnregisterAnimator(Ref<Animator> animator);

		private:
			AnimationManager();

		private:
			static AnimationManager* s_instance;
			std::vector<Ref<Animator>> m_registeredAnimators;
		};
	}
}