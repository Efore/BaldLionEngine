#pragma once
#include "ECSUtils.h"
#include "BaldLion/Core/StringId.h"

namespace BaldLion {

	namespace ECS {
	
		class ECSEntity {

		public:

			ECSEntity(const char* entityName, ECSEntityID entityID) : m_entityName(BL_STRING_TO_STRINGID(entityName)), m_entityID(entityID), m_parentID(0)
			{
				m_childrenIDs = DynamicArray<ECSEntityID>(MemoryManager::GetAllocatorType(this), 5);
			}

			StringId GetEntityName() const { return m_entityName; }			
			void SetEntityName(const char* name) { m_entityName = BL_STRING_TO_STRINGID(name); }

			ECSEntityID GetEntityID() const { return m_entityID; } 

			void SetParentID(ECSEntityID parentID) { m_parentID = parentID; }
			ECSEntityID GetParentID() const { return m_parentID; }

			const DynamicArray<ECSEntityID>& GetChildrenIDs() const { return m_childrenIDs; }
			DynamicArray<ECSEntityID>& GetChildrenIDs() { return m_childrenIDs; }

		private: 
			StringId m_entityName;
			ECSEntityID m_entityID;

			ECSEntityID m_parentID;
			DynamicArray<ECSEntityID> m_childrenIDs;

			friend class ECSManager;
		};
	}
}
