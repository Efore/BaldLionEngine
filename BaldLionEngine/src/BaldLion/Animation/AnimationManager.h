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
			static void Stop();

			static void OnUpdate(float timeStep);

			static void GenerateAnimator(const aiScene *scene, const std::unordered_map<std::string, uint32_t>& jointMapping, SkinnedMesh* animatedMesh);

			static void RegisterAnimator(Animator* animator);
			static void UnregisterAnimator(Animator* animator);

		private:			
			static bool s_initialized;
			static DynamicArray<Animator*> s_registeredAnimators;
		};
	}
}