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
				for (ui32 i = 0; i < (ui32)ECSComponentID::Count; ++i)
				{
					m_components[i] = nullptr;
				}
			}

			void Set(ECSComponentID index, ECSComponent* ptrToComponent)
			{
				m_components[(ui32)index] = ptrToComponent;
			}

			template<typename T>
			const T* Read(ECSComponentID componentID) const;

			template<typename T>
			T* Write(ECSComponentID componentID);

			ECSComponent* operator[](ui32 index) {
				return m_components[index];
			}

		private:
			ECSComponent* m_components[(ui32)ECSComponentID::Count];
		};

		template<typename T>
		T* BaldLion::ECS::ECSComponentLookUp::Write(ECSComponentID componentID)
		{
			static_assert(std::is_base_of<ECSComponent, T>::value, "T must inherit from ECSComponent");
			return (T*)m_components[(ui32)componentID];
		}

		template<typename T>
		const T* BaldLion::ECS::ECSComponentLookUp::Read(ECSComponentID componentID) const
		{
			static_assert(std::is_base_of<ECSComponent, T>::value, "T must inherit from ECSComponent");
			
			return (T*)m_components[(ui32)componentID];
		}
	}
}
