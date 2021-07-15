#include "blpch.h"
#include "ECSSystem.h"
#include "ECSManager.h"
#include "BaldLion/Core/Containers/HashTable.h"

namespace BaldLion {

	namespace ECS {

		ECSSystem::ECSSystem(ECSSignature signature, ECSManager* ecsManager) : m_signature(signature), m_ecsManager(ecsManager)
		{
			m_componentLookUps = DynamicArray<ECSComponentLookUp*>(AllocationType::FreeList_Main, 6);
			
			for (HashTable<ECSEntityID, ECSSignature>::Iterator iterator = m_ecsManager->GetEntitySignatures().Begin(); iterator != m_ecsManager->GetEntitySignatures().End(); ++iterator)
			{
				if ((iterator.GetValue() & signature) == signature)
				{
					ECSComponentLookUp* componentLookUp = &(m_ecsManager->GetEntityComponents().Get(iterator.GetKey()));
					m_componentLookUps.PushBack(componentLookUp); 
				}
			}
		}

		ECSSystem::~ECSSystem()
		{
			m_componentLookUps.Clear();
		}
	}
}