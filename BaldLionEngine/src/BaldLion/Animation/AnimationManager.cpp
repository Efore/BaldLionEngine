#include "blpch.h"
#include "AnimationManager.h"

namespace BaldLion
{
	namespace Animation
	{
		AnimationManager* AnimationManager::s_instance;

		AnimationManager::AnimationManager()
		{
			m_registeredAnimators.reserve(10);
		}

		AnimationManager::~AnimationManager()
		{

		}

		void AnimationManager::OnUpdate(float timeStep)
		{
			for (Ref<Animator> animator : m_registeredAnimators)
			{
				animator->OnUpdate(timeStep);
			}
		}

		AnimationManager* AnimationManager::GetInstance()
		{
			if (s_instance == nullptr)
				s_instance = new AnimationManager();

			return s_instance;
		}

		void AnimationManager::RegisterAnimator(Ref<Animator> animator)
		{
			if (std::find(m_registeredAnimators.begin(), m_registeredAnimators.end(), animator) == m_registeredAnimators.end())
			{
				m_registeredAnimators.push_back(animator);
			}
		}

		void AnimationManager::UnregisterAnimator(Ref<Animator> animator)
		{
			auto it = std::find(m_registeredAnimators.begin(), m_registeredAnimators.end(), animator);				

			if (it != m_registeredAnimators.end())
			{
				std::swap(*it, m_registeredAnimators.back());				
				m_registeredAnimators.pop_back();
			}
		}
	}
}