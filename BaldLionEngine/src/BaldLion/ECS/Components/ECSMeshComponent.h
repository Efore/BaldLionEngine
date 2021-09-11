#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include "BaldLion/Rendering/VertexArray.h"
#include "BaldLion/Rendering/Material.h"
#include "BaldLion/Utils/GeometryUtils.h"

using namespace BaldLion::Rendering;
using namespace BaldLion::GeometryUtils;

namespace BaldLion {

	namespace ECS
	{
		class ECSMeshComponent : public ECSComponent
		{
			ECSMeshComponent(bool isStatic) : 
				ECSComponent(ECSComponentID::Mesh),
				isStatic(isStatic)
			{

			}
			
		public:

			AABB aabb;
			DynamicArray<Vertex> vertices;
			DynamicArray<ui32> indices;

			bool isStatic;
			Material* material;
		};

	}
}
