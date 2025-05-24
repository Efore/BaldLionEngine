#include "blpch.h"
#include "AnimatorSerializer.h"
#include "BaldLion/Core/Containers/HashTable.h"
#include "BaldLion/ResourceManagement/ResourceManager.h"

#define YAML_KEY_ANIMATOR_ID						"AnimatorPath"
#define YAML_KEY_INITIAL_ANIMATION_ID				"InitialAnimationID"

#define YAML_KEY_ANIMATIONS							"Animations"
#define YAML_KEY_ANIMATION_PATH						"AnimationPath"

#define YAML_KEY_TRANSITIONS						"Transitions"
#define YAML_KEY_TRANSITION_INIT_ANIMATION_ID		"TransitionInitAnimationID"
#define YAML_KEY_TRANSITION_FINAL_ANIMATION_ID		"TransitionFinalAnimationID"
#define YAML_KEY_TRANSITION_TIME					"TransitionTime"
#define YAML_KEY_TRANSITION_EXIT_TIME				"TransitionExitTime"

#define YAML_KEY_CONDITIONS							"Conditions"
#define YAML_KEY_CONDITION_PARAMETER_A				"ConditionsParameterA"
#define YAML_KEY_CONDITION_PARAMETER_B				"ConditionsParameterB"
#define YAML_KEY_CONDITION_COMPARISON_TYPE			"ConditionsComparisonType"

#define YAML_KEY_PARAMETERS							"Parameters"
#define YAML_KEY_PARAMETER_ID						"ParameterID"
#define YAML_KEY_PARAMETER_VALUE					"ParameterValue"
#define YAML_KEY_PARAMETER_TYPE						"ParameterType"

namespace BaldLion
{
	namespace Animation
	{
		void AnimatorSerializer::SerializeAnimator(Animator* animator, const char* filepath)
		{
			YAML::Emitter out;

			out << YAML::BeginMap;

			out << YAML::Key << YAML_KEY_ANIMATOR_ID << YAML::Value << BL_STRINGID_TO_STR_C(animator->GetResourcePath());
			out << YAML::Key << YAML_KEY_INITIAL_ANIMATION_ID << YAML::Value << animator->GetInitialAnimationID();

			out << YAML::Key << YAML_KEY_PARAMETERS << YAML::Value << YAML::BeginSeq;

			BL_HASHTABLE_FOR(animator->GetAllParameters(), it)
			{
				out << YAML::BeginMap;
				out << YAML::Key << YAML_KEY_PARAMETER_ID << YAML::Value << BL_STRINGID_TO_STR_C(it.GetKey());
				SerializeParameter(out, it.GetValue());
				out << YAML::EndMap;
			}

			out << YAML::EndSeq;

			out << YAML::Key << YAML_KEY_ANIMATIONS << YAML::Value << YAML::BeginSeq;

			BL_HASHTABLE_FOR(animator->GetAllAnimations(), it)
			{	
				SerializeAnimation(out, it.GetValue());
			}

			out << YAML::EndSeq;
			
			out << YAML::Key << YAML_KEY_TRANSITIONS << YAML::Value << YAML::BeginSeq;

			BL_HASHTABLE_FOR(animator->GetAllTransitions(), it)
			{
				BL_DYNAMICARRAY_FOR(i, it.GetValue(), 0)
				{
					SerializeTransition(out, it.GetValue()[i]);
				}	
			}
			out << YAML::EndSeq;

			out << YAML::EndMap;

			const std::string parentFolderPath = StringUtils::GetPathWithoutFile(filepath);

			if (CreateDirectoryA(parentFolderPath.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
			{
				std::ofstream fout(filepath);
				fout << out.c_str();
				fout.close();
			}
		}

		bool AnimatorSerializer::DeserializeAnimator(const char* filepath)
		{
			std::ifstream stream(filepath);
			std::stringstream strStream;

			strStream << stream.rdbuf();

			YAML::Node data = YAML::Load(strStream.str());

			if (!data[YAML_KEY_ANIMATOR_ID])
				return false;

			std::string animatorPath = data[YAML_KEY_ANIMATOR_ID].as<std::string>();

			Animator* animator = ResourceManagement::ResourceManager::LoadResource<Animator>(animatorPath);

			if (animator == nullptr)
				return false;

			auto parameters = data[YAML_KEY_PARAMETERS];

			for (auto yamlParamter : parameters)
			{
				std::string paramterName = yamlParamter[YAML_KEY_PARAMETER_ID].as<std::string>();
				AnimatorParameter parameter = DeserializeParameterer(yamlParamter);
				animator->AddParameter(BL_STRING_TO_STRINGID(paramterName), parameter);
			}

			auto animations = data[YAML_KEY_ANIMATIONS];

			for (auto yamlAnimation : animations)
			{
				AnimationClip* animation = DeserializeAnimation(yamlAnimation);
				if (animation != nullptr)
				{
					animator->AddAnimation(animation);
				}
			}

			ui32 initialAnimationID = data[YAML_KEY_INITIAL_ANIMATION_ID].as<ui32>();

			if (animator->GetAnimationClip(initialAnimationID) != nullptr)
			{
				animator->SetInitialAnimation(initialAnimationID);
			}

			auto transitions = data[YAML_KEY_TRANSITIONS];

			for (auto yamlTransition : transitions)
			{
				AnimatorTransition* transition = DeserializeTransition(yamlTransition, animator);
				if (transition != nullptr)
				{
					animator->AddAnimationTransition(std::move(transition));
				}
			}

			return true;
		}

		void AnimatorSerializer::SerializeAnimation(YAML::Emitter &out, const AnimationClip* animation)
		{
			out << YAML::BeginMap;
			out << YAML::Key << YAML_KEY_ANIMATION_PATH << YAML::Value << BL_STRINGID_TO_STR_C(animation->GetResourcePath());
			//TODO SERIALIZE EVENTS?
			out << YAML::EndMap;
		}

		AnimationClip* AnimatorSerializer::DeserializeAnimation(const YAML::detail::iterator_value& yamlEntity)
		{
			std::string animationPath = yamlEntity[YAML_KEY_ANIMATION_PATH].as<std::string>();
			return ResourceManagement::ResourceManager::LoadResource<AnimationClip>(animationPath);
		}

		void AnimatorSerializer::SerializeTransition(YAML::Emitter &out, const AnimatorTransition* transition)
		{
			out << YAML::BeginMap;
			out << YAML::Key << YAML_KEY_TRANSITION_INIT_ANIMATION_ID << YAML::Value << BL_STRINGID_TO_STR_C(transition->GetInitialAnimationID());
			out << YAML::Key << YAML_KEY_TRANSITION_FINAL_ANIMATION_ID << YAML::Value << BL_STRINGID_TO_STR_C(transition->GetFinalAnimationID());
			out << YAML::Key << YAML_KEY_TRANSITION_TIME << YAML::Value << transition->GetTransitionTime();
			out << YAML::Key << YAML_KEY_TRANSITION_EXIT_TIME << YAML::Value << transition->GetExitTime();

			out << YAML::Key << YAML_KEY_CONDITIONS << YAML::Value << YAML::BeginSeq;

			BL_DYNAMICARRAY_FOR(i, transition->m_conditions, 0)
			{
				SerializeCondition(out, transition->m_conditions[i]);
			}

			out << YAML::EndSeq;
			out << YAML::EndMap;
		} 

		AnimatorTransition* AnimatorSerializer::DeserializeTransition(const YAML::detail::iterator_value& yamlEntity, const Animator* animator)
		{
			AnimatorTransition* result = nullptr;

			ui32 initialAnimationID = BL_STRING_TO_STRINGID(yamlEntity[YAML_KEY_TRANSITION_INIT_ANIMATION_ID].as<std::string>());
			ui32 finalAnimationID = BL_STRING_TO_STRINGID(yamlEntity[YAML_KEY_TRANSITION_FINAL_ANIMATION_ID].as<std::string>());
			float transitiontime = yamlEntity[YAML_KEY_TRANSITION_TIME].as<float>();
			float exitTime = yamlEntity[YAML_KEY_TRANSITION_EXIT_TIME].as<float>();
			
			result = MemoryManager::New<AnimatorTransition>("Animator Transition", MemoryManager::GetAllocatorType(animator), initialAnimationID, finalAnimationID, exitTime, transitiontime);

			auto conditions = yamlEntity[YAML_KEY_CONDITIONS];

			for (auto serializedCondition : conditions) 
			{
				AnimatorCondition condition = DeserializeCondition(serializedCondition);
				result->AddCondition(condition);
			}

			return result;
		}

		void AnimatorSerializer::SerializeParameter(YAML::Emitter &out, const AnimatorParameter& parameter)
		{
			out << YAML::Key << YAML_KEY_PARAMETER_TYPE << YAML::Value << (ui32)parameter.Type;

			switch (parameter.Type)
			{
				case AnimatorParameter::ValueType::Bool:
					out << YAML::Key << YAML_KEY_PARAMETER_VALUE << YAML::Value << parameter.Value.boolean;			
					break;
				case AnimatorParameter::ValueType::Int:
					out << YAML::Key << YAML_KEY_PARAMETER_VALUE << YAML::Value << parameter.Value.integer;
					break;
				case AnimatorParameter::ValueType::Float:
					out << YAML::Key << YAML_KEY_PARAMETER_VALUE << YAML::Value << parameter.Value.floating;
					break;
				default:
					break;
			}
		}

		AnimatorParameter AnimatorSerializer::DeserializeParameterer(const YAML::detail::iterator_value& yamlEntity)
		{
			AnimatorParameter result;

			result.Type = (AnimatorParameter::ValueType)yamlEntity[YAML_KEY_PARAMETER_TYPE].as<ui32>();

			switch (result.Type)
			{
			case AnimatorParameter::ValueType::Bool:
				result.Value.boolean = yamlEntity[YAML_KEY_PARAMETER_VALUE].as<bool>();
				break;
			case AnimatorParameter::ValueType::Int:
				result.Value.integer = yamlEntity[YAML_KEY_PARAMETER_VALUE].as<ui32>();
				break;
			case AnimatorParameter::ValueType::Float:
				result.Value.floating = yamlEntity[YAML_KEY_PARAMETER_VALUE].as<float>();
				break;
			default:
				break;
			}

			return result;
		}

		void AnimatorSerializer::SerializeCondition( YAML::Emitter &out, const AnimatorCondition& condition)
		{
			out << YAML::BeginMap;
			out << YAML::Key << YAML_KEY_CONDITION_PARAMETER_A << YAML::Value << BL_STRINGID_TO_STR_C(condition.ParameterAName);
			SerializeParameter(out, condition.ParameterB);
			out << YAML::Key << YAML_KEY_CONDITION_COMPARISON_TYPE << YAML::Value << (ui32)condition.Comparison;
			out << YAML::EndMap;
		}

		AnimatorCondition AnimatorSerializer::DeserializeCondition(const YAML::detail::iterator_value& yamlEntity)
		{
			StringId parameterAName = BL_STRING_TO_STRINGID(yamlEntity[YAML_KEY_CONDITION_PARAMETER_A].as<std::string>());
			AnimatorParameter parameterB = DeserializeParameterer(yamlEntity);
			
			AnimatorCondition::ComparisonType comparisonType = (AnimatorCondition::ComparisonType)yamlEntity[YAML_KEY_CONDITION_COMPARISON_TYPE].as<ui32>();

			return { comparisonType , parameterAName, parameterB };
		}

	}
}
