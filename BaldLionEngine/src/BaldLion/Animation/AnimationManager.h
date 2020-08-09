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

			static void Init();

			static void OnUpdate(float timeStep);

			static void GenerateAnimator(const aiScene *scene, const std::map<std::string, uint32_t>& jointMapping, const Ref<AnimatedMesh>& animatedMesh);

			static void RegisterAnimator(Ref<Animator> animator);
			static void UnregisterAnimator(Ref<Animator> animator);

		private:			
			static bool s_initialized;
			static std::vector<Ref<Animator>> s_registeredAnimators;
		};
	}
}