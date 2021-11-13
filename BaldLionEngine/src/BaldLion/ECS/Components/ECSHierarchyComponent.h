#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include "BaldLion/Core/Containers/DynamicArray.h"


namespace BaldLion {

	namespace ECS
	{
		class ECSHierarchyComponent : public ECSComponent
		{
		public:

			ECSHierarchyComponent() :
				ECSComponent(ECSComponentID::Hierarchy), hasParent(false)
			{
				childEntitiesIDs = DynamicArray<ECSEntityID>(AllocationType::FreeList_ECS, 10);
			}

		public:
			bool hasParent;
			ECSEntityID parentEntityID;
			DynamicArray<ECSEntityID> childEntitiesIDs;
		};
	}
}
