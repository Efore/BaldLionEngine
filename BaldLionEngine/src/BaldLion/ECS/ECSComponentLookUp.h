#pragma once
#include "ECSUtils.h"
#include "ECSComponent.h"

namespace BaldLion
{
	namespace ECS {

		class ECSComponentLookUp {

		public:
			ECSComponentLookUp()
			{
				for (ECSComponent* ptr : m_components)
				{
					ptr = nullptr;
				}
			}

			void Set(ECSComponentID index, ECSComponent* ptrToComponent)
			{
				m_components[(ui32)index] = ptrToComponent;
			}

			template<typename ECSComponentType>
			const ECSComponentType* Read(ECSComponentID componentID) const;

			template<typename ECSComponentType>
			ECSComponentType* Write(ECSComponentID componentID);

		private:
			ECSComponent* m_components[(ui32)ECSComponentID::Count];
		};

		template<typename ECSComponentType>
		ECSComponentType* BaldLion::ECS::ECSComponentLookUp::Write(ECSComponentID componentID)
		{
			static_assert(std::is_base_of<ECSComponent, ECSComponentType>::value, "T must inherit from ECSComponent");
			return (ECSComponentType*)m_components[(ui32)componentID];
		}

		template<typename ECSComponentType>
		const ECSComponentType* BaldLion::ECS::ECSComponentLookUp::Read(ECSComponentID componentID) const
		{
			static_assert(std::is_base_of<ECSComponent, ECSComponentType>::value, "T must inherit from ECSComponent");
			return (ECSComponentType*)m_components[(ui32)componentID];
		}
	}
}
