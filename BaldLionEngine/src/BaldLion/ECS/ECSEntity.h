#pragma once
#include "ECSUtils.h"
#include "BaldLion/Core/StringId.h"

namespace BaldLion {

	namespace ECS {
	
		class ECSEntity {

		public:
			StringId GetEntityName() const { return m_entityName; }			
			void SetEntityName(const char* name) { m_entityName = STRING_TO_STRINGID(name); }

			ECSEntityID GetEntityID() const { return m_entityID; } 

		private:

			ECSEntity(const char* entityName, ECSEntityID entityID) : m_entityName(STRING_TO_STRINGID(entityName)), m_entityID(entityID) {}

		private: 
			StringId m_entityName;
			ECSEntityID m_entityID;
			friend class ECSManager;
		};
	}
}
