#include "blpch.h"
#include "HTNSerializer.h"
#include "BaldLion/AI/HTN/HTNManager.h"

#define YAML_KEY_DOMAINS							"Domains"
#define YAML_KEY_DOMAIN_ID							"DomainID"
#define YAML_KEY_DOMAIN_MAIN_TASK					"DomainMainTask"

#define YAML_KEY_TASKS								"Tasks"
#define YAML_KEY_TASK_ID							"TaskID"
#define YAML_KEY_TASK_TYPE							"TaskType"
#define YAML_KEY_TASK_OPERATOR_TYPE					"TaskOperatorType"
#define YAML_KEY_TASK_METHODS						"TaskMethods"
#define YAML_KEY_TASK_EFFECTS						"TaskEffects"

#define YAML_KEY_METHOD_CONDITIONS					"MethodConditions"
#define YAML_KEY_METHOD_SUBTASKS					"MethodSubtaskIndices"
#define YAML_KEY_METHOD_SUBTASK_INDEX				"MethodSubtaskIndex"

#define YAML_KEY_EFFECT_ID							"EffectID"
#define YAML_KEY_EFFECT_VARIANT						"EffectVariant"

#define YAML_KEY_CONDITION_BLACKBOARD_KEY			"ConditionBlackboardKey"
#define YAML_KEY_CONDITION_VARIANT					"ConditionVariant"
#define YAML_KEY_CONDITION_COMPARISON_TYPE			"ConditionComparisonType"


namespace BaldLion::AI::HTN
{
	void HTNSerializer::SerializeHTNData()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;

		SerializeDefinedHTNTasks(out);		
		SerializeHTNDomains(out);

		out << YAML::EndMap;

		std::ofstream fout(HTN_DATA_PATH);
		fout << out.c_str();
		fout.close();
	}

	bool HTNSerializer::DeserializeHTNData()
	{
		std::ifstream stream(HTN_DATA_PATH);
		std::stringstream strStream;

		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());		

		if (!data[YAML_KEY_TASKS] || !data[YAML_KEY_DOMAINS])
			return false;

		auto tasks = data[YAML_KEY_TASKS];

		if (tasks)
		{
			for (auto task : tasks)
			{
				DeserializeHTNTask(task);
			}
		}

		auto domains = data[YAML_KEY_TASKS];

		if (domains)
		{
			for (auto domain : domains)
			{
				DeserializeHTNDomain(domain);
			}
		}

		return true;
	}

	void HTNSerializer::SerializeDefinedHTNTasks(YAML::Emitter& out)
	{
		out << YAML::BeginMap;
		out << YAML::Key << YAML_KEY_TASKS << YAML::Value << YAML::BeginSeq;
				
		BL_DYNAMICARRAY_FOREACH(HTNManager::s_definedTasks)
		{
			SerializeHTNTask(HTNManager::s_definedTasks[i], out);
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;
	}

	void HTNSerializer::SerializeHTNTask(const HTNTask& htnTask, YAML::Emitter& out)
	{
		out << YAML::BeginMap;
		out << YAML::Key << YAML_KEY_TASK_ID << YAML::Value << (htnTask.taskID);
		out << YAML::Key << YAML_KEY_TASK_TYPE << YAML::Value << (ui8)(htnTask.taskType);
		out << YAML::Key << YAML_KEY_TASK_OPERATOR_TYPE << YAML::Value << (ui32)(htnTask.taskOperatorType);

		out << YAML::Key << YAML_KEY_TASK_METHODS << YAML::Value << YAML::BeginSeq;

		BL_DYNAMICARRAY_FOREACH(htnTask.methods)
		{
			const HTNMethod& method = htnTask.methods[i];

			out << YAML::BeginMap;
			out << YAML::Key << YAML_KEY_METHOD_CONDITIONS << YAML::Value << YAML::BeginSeq;

			BL_DYNAMICARRAY_FOR(j,htnTask.methods[i].conditions,0)
			{
				const HTNWorldStateCondition& condition = htnTask.methods[i].conditions[j];

				out << YAML::BeginMap;

				out << YAML::Key << YAML_KEY_CONDITION_BLACKBOARD_KEY << YAML::Value << (condition.blackboardKey);
				out << YAML::Key << YAML_KEY_CONDITION_COMPARISON_TYPE << YAML::Value << (ui8)(condition.comparisonType);
				SerializeVariant(out, YAML_KEY_CONDITION_VARIANT, condition.value);

				out << YAML::EndMap;
			}

			out << YAML::EndSeq;

			out << YAML::Key << YAML_KEY_METHOD_SUBTASKS << YAML::Value << YAML::BeginSeq;

			BL_DYNAMICARRAY_FOR(j, htnTask.methods[i].subtasksIndices, 0)
			{
				out << YAML::BeginMap;
				out << YAML::Key << YAML_KEY_METHOD_SUBTASK_INDEX << YAML::Value << htnTask.methods[i].subtasksIndices[j];
				out << YAML::EndMap;
			}

			out << YAML::EndSeq;

			out << YAML::EndMap;
		}

		out << YAML::EndSeq;

		out << YAML::Key << YAML_KEY_TASK_EFFECTS << YAML::Value << YAML::BeginSeq;

		BL_DYNAMICARRAY_FOREACH(htnTask.effects)
		{
			const HTNWorldStateEffect& effect = htnTask.effects[i];
			out << YAML::BeginMap;

			out << YAML::Key << YAML_KEY_EFFECT_ID << YAML::Value << (effect.blackboardKey);
			SerializeVariant(out, YAML_KEY_EFFECT_VARIANT, effect.blackboardValue);

			out << YAML::EndMap;
		}

		out << YAML::EndSeq;

		out << YAML::EndMap;
	}

	void HTNSerializer::DeserializeHTNTask(const YAML::detail::iterator_value& yamlTask)
	{
		HTNTask* newTask = HTNManager::s_definedTasks.EmplaceBack();

		newTask->taskID = yamlTask[YAML_KEY_TASK_ID].as<ui32>();
		newTask->taskType = (HTNTask::TaskType)(yamlTask[YAML_KEY_TASK_TYPE].as<ui8>());
		newTask->taskOperatorType = (HTNOperatorType)(yamlTask[YAML_KEY_TASK_OPERATOR_TYPE].as<ui32>());
		newTask->methods = DynamicArray<HTNMethod>(AllocationType::FreeList_Main, 8);
		newTask->effects = DynamicArray<HTNWorldStateEffect>(AllocationType::FreeList_Main, 8);

		auto methods = yamlTask[YAML_KEY_TASK_METHODS];
		if (methods)
		{
			for (auto method : methods)
			{
				HTNMethod* newMethod = newTask->methods.EmplaceBack();

				newMethod->conditions = DynamicArray<HTNWorldStateCondition>(AllocationType::FreeList_Main, 8);
				auto conditions = method[YAML_KEY_METHOD_CONDITIONS];

				if (conditions)
				{
					for (auto condition : conditions)
					{
						HTNWorldStateCondition* newCondition = newMethod->conditions.EmplaceBack();
						newCondition->blackboardKey = condition[YAML_KEY_CONDITION_BLACKBOARD_KEY].as<ui32>();
						newCondition->comparisonType = (VariantComparisonType)condition[YAML_KEY_CONDITION_COMPARISON_TYPE].as<ui8>();

						DeserializeVariant(condition, YAML_KEY_CONDITION_VARIANT, newCondition->value);
					}
				}

				newMethod->subtasksIndices = DynamicArray<ui32>(AllocationType::FreeList_Main, 8);
				auto subtasks = method[YAML_KEY_METHOD_SUBTASKS];

				if (subtasks)
				{
					for (auto subtask : subtasks)
					{
						newMethod->subtasksIndices.EmplaceBack(subtask[YAML_KEY_METHOD_SUBTASK_INDEX].as<ui32>());
					}
				}
			}
		}		

		auto effects = yamlTask[YAML_KEY_TASK_EFFECTS];
		if (effects)
		{
			for (auto effect : effects)
			{
				HTNWorldStateEffect* newEffect = newTask->effects.EmplaceBack();
				newEffect->blackboardKey = effect[YAML_KEY_EFFECT_ID].as<ui32>();
				DeserializeVariant(effect, YAML_KEY_EFFECT_VARIANT, newEffect->blackboardValue);				
			}
		}
	}

	void HTNSerializer::SerializeHTNDomains(YAML::Emitter& out)
	{
		out << YAML::BeginMap;
		out << YAML::Key << YAML_KEY_DOMAINS << YAML::Value << YAML::BeginSeq;

		BL_HASHMAP_FOR(HTNManager::s_definedDomains, it)
		{
			out << YAML::BeginMap;

			out << YAML::Key << YAML_KEY_DOMAIN_ID << YAML::Value << it.GetKey();
			out << YAML::Key << YAML_KEY_DOMAIN_MAIN_TASK << YAML::Value << (i32)(it.GetValue().mainTask);

			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;
	}

	void HTNSerializer::DeserializeHTNDomain(const YAML::detail::iterator_value& yamlDomain)
	{
		StringId domainID = yamlDomain[YAML_KEY_DOMAIN_ID].as<ui32>();
		i32 mainTaskIndex = yamlDomain[YAML_KEY_DOMAIN_MAIN_TASK].as<i32>();

		HTNManager::s_definedDomains.Emplace(domainID, { mainTaskIndex });
	}

	void HTNSerializer::SerializeVariant(YAML::Emitter& out, const std::string& key, const Variant& variant)
	{
		out << YAML::Key << (key + "_TYPE") << YAML::Value << (ui16)(variant.m_valueType);
		
		unsigned char* data = (unsigned char*)&variant;

		ui64 firstHalf = (ui64)(*data);
		out << YAML::Key << (key + "_VALUE_1") << YAML::Value << firstHalf;

		ui64 secondHalf = (ui64)(data[8]);
		out << YAML::Key << (key + "_VALUE_2") << YAML::Value << firstHalf;
	}

	void HTNSerializer::DeserializeVariant(const YAML::Node& node, const std::string& key, Variant& result)
	{
		result.m_valueType = (VariantType)node[key + "_TYPE"].as<ui16>();
		ui64 firstHalf = node[key + "_VALUE_1"].as<ui64>();
		ui64 secondHalf = node[key + "_VALUE_2"].as<ui64>();

		unsigned char* data = (unsigned char*)&result;
		data[0] = firstHalf;
		data[8] = secondHalf;		
	}

}