#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include "BaldLion/Core/Containers/DynamicArray.h"


namespace BaldLion {

	namespace ECS
	{
		class ECSHierarchyComponent : public ECSComponent
		{
		public:

			ECSHierarchyComponent(ECSEntityID parentEntityID) :
				ECSComponent(ECSComponentType::Hierarchy), parentEntityID(parentEntityID), childEntitiesSize(0)
			{
				
			}

			ECSHierarchyComponent(ECSEntityID parentEntityID, const ECSEntityID* childEntitiesIDs, ui32 childEntitiesSize) :
				ECSComponent(ECSComponentType::Hierarchy),parentEntityID(parentEntityID), childEntitiesSize(childEntitiesSize)
			{
				for (ui32 i = 0; i < childEntitiesSize; ++i)
				{
					this->childEntitiesIDs[i] = childEntitiesIDs[i];
				}
			}

		public:
			ECSEntityID parentEntityID;
			ECSEntityID childEntitiesIDs[100];
			ui32 childEntitiesSize;
		};
	}
}
