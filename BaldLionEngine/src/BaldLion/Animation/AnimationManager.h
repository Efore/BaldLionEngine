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

			static void GenerateAnimator(const aiScene *scene, std::string& animatorPath, const HashTable<StringId, ui32>& jointMapping);
			static const HashTable<ui32, Animator*>& GetAnimators() { return s_registeredAnimators; }

			static Animator* GetAnimator(const ui32 animatorID);

			static void RegisterAnimator(Animator* animator);
			static void UnregisterAnimator(Animator* animator);

			static void SerializeAnimator(const ui32 animatorID);
			static void DeserializeAnimator(const char* animatorMetaFilePath);


		private:			
			static bool s_initialized;
			static HashTable<ui32,Animator*> s_registeredAnimators;
		};
	}
}