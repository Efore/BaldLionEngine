#include "blpch.h"
#include "HTNSerializer.h"
#include "BaldLion/AI/HTN/HTNManager.h"

#define YAML_KEY_WORLD_STATE_BLACKBOARDS			"WorldStateBlackboards"
#define YAML_KEY_WORLD_STATE_BLACKBOARD				"WorldStateBlackboard"
#define YAML_KEY_WORLD_STATE_BLACKBOARD_KEY			"WorldStateBlackboardKey"
#define YAML_KEY_WORLD_STATE_BLACKBOARD_VARIANT		"WorldStateBlackboardVariant"

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


namespace BaldLion::AI::HTN
{
	void HTNSerializer::SerializeHTNData()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;

		SerializeHTNTasks(out);		
		SerializeHTNDomains(out);
		SerializeWorldStateBlackboards(out);

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

		if (!data[YAML_KEY_TASKS] || !data[YAML_KEY_DOMAINS] || !data[YAML_KEY_WORLD_STATE_BLACKBOARDS])
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

		auto blackboards = data[YAML_KEY_WORLD_STATE_BLACKBOARDS];

		if (blackboards)
		{
			for (auto blackboard : blackboards)
			{
				DeserializeWorldStateBlackboard(blackboard);
			}
		}

		return true;
	}

	void HTNSerializer::SerializeHTNTasks(YAML::Emitter& out)
	{		
		out << YAML::Key << YAML_KEY_TASKS << YAML::Value << YAML::BeginSeq;
				
		BL_DYNAMICARRAY_FOREACH(HTNManager::s_definedTasks)
		{
			SerializeHTNTask(HTNManager::s_definedTasks[i], out);
		}

		out << YAML::EndSeq;
	}

	void HTNSerializer::SerializeHTNTask(const HTNTask& htnTask, YAML::Emitter& out)
	{
		out << YAML::BeginMap;
		out << YAML::Key << YAML_KEY_TASK_ID << YAML::Value << BL_STRINGID_TO_STRING(htnTask.taskID);
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

				out << YAML::Key << YAML_KEY_CONDITION_BLACKBOARD_KEY << YAML::Value << BL_STRINGID_TO_STRING(condition.blackboardKey);
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

			out << YAML::Key << YAML_KEY_EFFECT_ID << YAML::Value << BL_STRINGID_TO_STRING(effect.blackboardKey);
			SerializeVariant(out, YAML_KEY_EFFECT_VARIANT, effect.blackboardValue);

			out << YAML::EndMap;
		}

		out << YAML::EndSeq;

		out << YAML::EndMap;
	}

	void HTNSerializer::DeserializeHTNTask(const YAML::detail::iterator_value& yamlTask)
	{
		HTNTask* newTask = HTNManager::s_definedTasks.EmplaceBack();

		newTask->taskID = BL_STRING_TO_STRINGID(yamlTask[YAML_KEY_TASK_ID].as<std::string>());
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
						newCondition->blackboardKey = BL_STRING_TO_STRINGID(condition[YAML_KEY_CONDITION_BLACKBOARD_KEY].as<std::string>());
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
				newEffect->blackboardKey = BL_STRING_TO_STRINGID(effect[YAML_KEY_EFFECT_ID].as<std::string>());
				DeserializeVariant(effect, YAML_KEY_EFFECT_VARIANT, newEffect->blackboardValue);				
			}
		}
	}

	void HTNSerializer::SerializeHTNDomains(YAML::Emitter& out)
	{
		out << YAML::Key << YAML_KEY_DOMAINS << YAML::Value << YAML::BeginSeq;

		BL_HASHMAP_FOR(HTNManager::s_definedDomains, it)
		{
			out << YAML::BeginMap;

			out << YAML::Key << YAML_KEY_DOMAIN_ID << YAML::Value << BL_STRINGID_TO_STRING(it.GetKey());
			out << YAML::Key << YAML_KEY_DOMAIN_MAIN_TASK << YAML::Value << (i32)(it.GetValue().mainTask);

			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
	}

	void HTNSerializer::DeserializeHTNDomain(const YAML::detail::iterator_value& yamlDomain)
	{
		StringId domainID = BL_STRING_TO_STRINGID(yamlDomain[YAML_KEY_DOMAIN_ID].as<std::string>());
		i32 mainTaskIndex = yamlDomain[YAML_KEY_DOMAIN_MAIN_TASK].as<i32>();

		HTNManager::s_definedDomains.Emplace(domainID, { mainTaskIndex });
	}

	void HTNSerializer::SerializeWorldStateBlackboards(YAML::Emitter& out)
	{
		out << YAML::Key << YAML_KEY_WORLD_STATE_BLACKBOARDS << YAML::Value << YAML::BeginSeq;

		BL_DYNAMICARRAY_FOREACH(HTNManager::s_definedWorldStateBlackboards)
		{
			const HTNWorldStateBlackboard& woldStateBlackboard = HTNManager::s_definedWorldStateBlackboards[i];

			out << YAML::BeginMap;
			out << YAML::Key << YAML_KEY_WORLD_STATE_BLACKBOARD << YAML::Value << YAML::BeginSeq;

			BL_HASHMAP_FOR(woldStateBlackboard, it)
			{
				out << YAML::BeginMap;

				out << YAML::Key << YAML_KEY_WORLD_STATE_BLACKBOARD_KEY << YAML::Value << BL_STRINGID_TO_STRING(it.GetKey());
				SerializeVariant(out, YAML_KEY_WORLD_STATE_BLACKBOARD_VARIANT, it.GetValue());

				out << YAML::EndMap;
			}

			out << YAML::EndSeq;
			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
	}

	void HTNSerializer::DeserializeWorldStateBlackboard(const YAML::detail::iterator_value& yamlBlackboard)
	{
		HTNWorldStateBlackboard* newBlackboard = HTNManager::s_definedWorldStateBlackboards.EmplaceBack();
		(*newBlackboard) = HashTable<StringId, Variant>(AllocationType::FreeList_Main, 16);

		auto yamlBlackboardEntry = yamlBlackboard[YAML_KEY_WORLD_STATE_BLACKBOARD];

		if (yamlBlackboardEntry)
		{
			for (auto yamlBlackboardEntryValue : yamlBlackboardEntry)
			{
				StringId blackboardKey = BL_STRING_TO_STRINGID(yamlBlackboardEntryValue[YAML_KEY_WORLD_STATE_BLACKBOARD_KEY].as<std::string>());
				Variant blackboardValue;
				DeserializeVariant(yamlBlackboardEntryValue, YAML_KEY_WORLD_STATE_BLACKBOARD_VARIANT, blackboardValue);

				newBlackboard->Emplace(blackboardKey, blackboardValue);
			}
		}
	}

	void HTNSerializer::SerializeVariant(YAML::Emitter& out, const std::string& yamlKey, const Variant& variant)
	{
		out << YAML::Key << (yamlKey + "_TYPE") << YAML::Value << (ui16)(variant.m_valueType);
		
		unsigned char* data = (unsigned char*)&variant.GetValue();

		ui64 firstHalf = (ui64)(*data);
		out << YAML::Key << (yamlKey + "_VALUE_1") << YAML::Value << firstHalf;

		ui64 secondHalf = (ui64)(data[8]);
		out << YAML::Key << (yamlKey + "_VALUE_2") << YAML::Value << secondHalf;
	}

	void HTNSerializer::DeserializeVariant(const YAML::Node& node, const std::string& yamlKey, Variant& result)
	{
		result.m_valueType = (VariantType)node[yamlKey + "_TYPE"].as<ui16>();
		ui64 firstHalf = node[yamlKey + "_VALUE_1"].as<ui64>();
		ui64 secondHalf = node[yamlKey + "_VALUE_2"].as<ui64>();

		unsigned char* data = (unsigned char*)&result.GetValue();
		data[0] = firstHalf;
		data[8] = secondHalf;		
	}	
}