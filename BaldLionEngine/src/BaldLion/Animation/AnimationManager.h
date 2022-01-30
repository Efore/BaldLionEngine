/*****************************************************************//**
 * \file   AnimationManager.h
 * \brief  This Manager manages the animations for the registered animators
 * 
 * \author Iván León Santiago
 * \date   April 2021
 *********************************************************************/
#pragma once
#include "Animator.h"
#include <assimp/scene.h>
#include <mutex>

namespace BaldLion
{
	namespace Animation
	{
		class AnimationManager
		{
		public:

			static void Init();
			static void Stop();

			static void GenerateAnimator(const aiScene *scene, const std::string& animatorName, const HashTable<StringId, ui32>& jointMapping);
			static const HashTable<StringId, Animator*>& GetAnimators() { return s_registeredAnimators; }

			static Animator* GetAnimator(const StringId animatorID);

			static void RegisterAnimator(Animator* animator);
			static void UnregisterAnimator(Animator* animator);

		private:			
			static bool s_initialized;
			static HashTable<StringId,Animator*> s_registeredAnimators;			
			static std::mutex s_animationManagerMutex;
		};
	}
}