#pragma once

#include "ECSUtils.h"

namespace BaldLion
{
	namespace ECS {

	
		
		class ECSComponent {

		public:
			ECSComponent(ECSComponentID componentID) : m_componentID(componentID){}

			const ECSComponentID GetComponentID() const { return m_componentID; }

		protected:
			ECSComponentID m_componentID;
		};

	}
}
