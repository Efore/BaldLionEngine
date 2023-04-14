#pragma once

#include "ECSUtils.h"

namespace BaldLion
{
	namespace ECS {
		
		class ECSComponent {

		public:
			ECSComponent(ECSComponentType componentType) : m_componentType(componentType), m_componentID(0){}

			const ECSComponentType GetComponentType() const { return m_componentType; }	
			void SetComponentID(ui32 componentID) { m_componentID = componentID; }
			
			friend bool operator == (const ECSComponent& a, const ECSComponent& b) { 
				return a.m_componentID == b.m_componentID;
			}
			friend bool operator != (const ECSComponent& a, const ECSComponent& b) {
				return a.m_componentID != b.m_componentID;
			}

		protected:
			ECSComponentType m_componentType;
			ui32 m_componentID;
		};

	}
}
