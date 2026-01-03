#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include "BaldLion/Rendering/VertexArray.h"
#include "BaldLion/Rendering/Material.h"
#include "BaldLion/Rendering/Buffer.h"
#include "BaldLion/Utils/GeometryUtils.h"

using namespace BaldLion::Rendering;
using namespace BaldLion::GeometryUtils;

namespace BaldLion {

	namespace ECS
	{
		class ECSMeshComponent : public ECSComponent
		{

		public:
			ECSMeshComponent(bool isStatic) : 
				ECSComponent(ECSComponentType::Mesh),
				isStatic(isStatic)
			{

			}

			~ECSMeshComponent() {
				vertices.Delete();
				indices.Delete();

				if (vertexArray != nullptr)
				{
					VertexArray::Destroy(vertexArray);
				}
			}
			
		public:

			DynamicArray<Vertex> vertices;
			DynamicArray<ui32> indices;
			BoundingBox localBoundingBox;
			Material* material = nullptr;
			VertexArray* vertexArray = nullptr;
			ui32 meshResourceID = 0;
			bool isStatic = false;
			bool isVisible = false;
			bool isShadowVisible = false;
		};

	}
}
