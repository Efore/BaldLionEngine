#pragma once

#include "ECSUtils.h"

namespace BaldLion
{
	namespace ECS {
		
		class ECSComponent {

		public:
			ECSComponent(ECSComponentType componentType) : m_componentType(componentType){}

			const ECSComponentType GetComponentType() const { return m_componentType; }

		protected:
			ECSComponentType m_componentType;
		};

	}
}
