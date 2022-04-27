#pragma once

#include "BaldLion/Animation/Animator.h"
#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

namespace BaldLion
{
	namespace Animation
	{
		class AnimatorSerializer
		{

		private:
			static void SerializeAnimator(Animator* animator, const char* filepath);		
			static bool DeserializeAnimator(const char* filepath);

			static void SerializeAnimation(YAML::Emitter &out, const AnimationData* animation);
			static AnimationData* DeserializeAnimation(const YAML::detail::iterator_value& yamlEntity);

			static void SerializeTransition(YAML::Emitter &out, const AnimatorTransition& transition);
			static AnimatorTransition* DeserializeTransition(const YAML::detail::iterator_value& yamlEntity, const Animator* animator );

			static void SerializeParameter(YAML::Emitter &out, const AnimatorParameter& parameter);
			static AnimatorParameter DeserializeParameterer(const YAML::detail::iterator_value& yamlEntity);

			static void SerializeCondition(YAML::Emitter &out, const AnimatorCondition& condition);
			static AnimatorCondition DeserializeCondition(const YAML::detail::iterator_value& yamlEntity);

			friend class AnimationManager;
		};
	}
}
