#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include "BaldLion/Rendering/VertexArray.h"
#include "BaldLion/Rendering/Material.h"
#include "BaldLion/Utils/GeometryUtils.h"
#include "BaldLion/Rendering/GeometryData.h"

using namespace BaldLion::Rendering;
using namespace BaldLion::GeometryUtils;

namespace BaldLion {

	namespace ECS
	{
		class ECSMeshRendererComponent : public ECSComponent
		{
			ECSMeshRendererComponent(ECSComponentID componentID, bool isStatic) : 
				ECSComponent(componentID), 
				isStatic(isStatic)
			{

			}
			
		public:

			AABB aabb;
			DynamicArray<Vertex> vertices;
			DynamicArray<ui32> indices;

			bool isStatic;
			DynamicArray<Material*> materials;
		};

	}
}
