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
				for (ui32 i = 0; i < (ui32)ECSComponentType::Count; ++i)
				{
					m_components[i] = nullptr;
				}
			}

			void Set(ECSComponentType index, ECSComponent* ptrToComponent)
			{
				m_components[(ui32)index] = ptrToComponent;
			}

			template<typename T>
			const T* Read(ECSComponentType componentType) const;

			template<typename T>
			T* Write(ECSComponentType componentType);

			ECSComponent* operator[](ui32 index) {
				return m_components[index];
			}

			const ECSComponent* operator[] (ui32 index) const {
				return m_components[index];
			}

		private:
			ECSComponent* m_components[(ui32)ECSComponentType::Count];
		};

		template<typename T>
		T* BaldLion::ECS::ECSComponentLookUp::Write(ECSComponentType componentType)
		{
			static_assert(std::is_base_of<ECSComponent, T>::value, "T must inherit from ECSComponent");
			return (T*)m_components[(ui32)componentType];
		}

		template<typename T>
		const T* BaldLion::ECS::ECSComponentLookUp::Read(ECSComponentType componentType) const
		{
			static_assert(std::is_base_of<ECSComponent, T>::value, "T must inherit from ECSComponent");
			
			return (T*)m_components[(ui32)componentType];
		}
	}
}
