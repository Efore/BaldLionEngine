#include "blpch.h"
#include "Animator.h"
#include <glm/gtc/matrix_transform.hpp>

namespace BaldLion
{
	namespace Animation
	{
		Animator::Animator(DynamicArray<AnimationData>& animations, const StringId animatorID) : m_animatorID(animatorID)
		{
			m_animations = HashTable<StringId, AnimationData*>(AllocationType::FreeList_Renderer, animations.Size() * 2);
			for (ui32 i = 0; i < animations.Size(); ++i)
			{
				if (!m_animations.Contains(animations[i].AnimationID))
				{
					m_animations.Emplace(animations[i].AnimationID, &animations[i]);
				}
			}
			m_initAnimationID = animations[0].AnimationID;
			m_animationDataContainer = &animations;
		}

		Animator::~Animator()
		{
			m_animationDataContainer->Delete();
			m_animations.Delete();
		}

		const AnimationData* Animator::GetAnimation(const StringId animationID) const
		{
			AnimationData* result = nullptr;

			if (m_animations.TryGet(animationID, result)) {

				return result;
			}

			return nullptr;
		}

	}
}

