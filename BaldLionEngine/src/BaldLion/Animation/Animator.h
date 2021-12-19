#pragma once
#include "AnimationData.h"
#include "BaldLion/Core/Core.h"
#include "BaldLion/Core/Containers/HashTable.h"
#include "Skeleton.h"

namespace BaldLion
{
	namespace Animation
	{

		class Animator {
			
		public:
			Animator(DynamicArray<AnimationData>& animations, const StringId animatorID);
			~Animator();

			const AnimationData* GetAnimation(const StringId animationID) const;
			const StringId GetAnimatorID() const { return m_animatorID; } ;

		private:	

			StringId m_animatorID;
			DynamicArray<AnimationData>* m_animationDataContainer;
			HashTable<StringId,AnimationData*> m_animations;
		};
	}
}