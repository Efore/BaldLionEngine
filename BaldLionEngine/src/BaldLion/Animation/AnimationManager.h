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
			void GenerateAnimator(const aiScene *scene, const std::map<std::string, uint32_t>& jointMapping, const Ref<AnimatedMesh>& animatedMesh);

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