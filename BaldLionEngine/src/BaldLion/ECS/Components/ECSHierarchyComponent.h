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
				ECSComponent(ECSComponentType::Hierarchy), parentEntityID(parentEntityID)
			{
				childEntitiesIDs = DynamicArray<ECSEntityID>(AllocationType::FreeList_ECS, 10);
			}

			ECSHierarchyComponent(ECSEntityID parentEntityID, const DynamicArray<ECSEntityID>& childEntitiesIDs) :
				ECSComponent(ECSComponentType::Hierarchy),parentEntityID(parentEntityID), childEntitiesIDs(childEntitiesIDs)
			{
			}

		public:
			ECSEntityID parentEntityID;
			DynamicArray<ECSEntityID> childEntitiesIDs;
		};
	}
}
