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

		public:
			ECSMeshComponent(bool isStatic) : 
				ECSComponent(ECSComponentType::Mesh),
				isStatic(isStatic)
			{

			}

			~ECSMeshComponent() {
				vertices.Delete();
				indices.Delete();
			}

			void UpdateLocalBoundingBox() {

				glm::vec3 minPointInLocalSpace = vertices[0].position;
				glm::vec3 maxPointInLocalSpace = vertices[0].position;

				{
					BL_DYNAMICARRAY_FOREACH(vertices)
					{
						const glm::vec3 vertexPosInWorldSpace = vertices[i].position;

						if (vertexPosInWorldSpace.x > maxPointInLocalSpace.x)	maxPointInLocalSpace.x = vertexPosInWorldSpace.x;
						if (vertexPosInWorldSpace.y > maxPointInLocalSpace.y)	maxPointInLocalSpace.y = vertexPosInWorldSpace.y;
						if (vertexPosInWorldSpace.z > maxPointInLocalSpace.z)	maxPointInLocalSpace.z = vertexPosInWorldSpace.z;

						if (vertexPosInWorldSpace.x < minPointInLocalSpace.x)	minPointInLocalSpace.x = vertexPosInWorldSpace.x;
						if (vertexPosInWorldSpace.y < minPointInLocalSpace.y)	minPointInLocalSpace.y = vertexPosInWorldSpace.y;
						if (vertexPosInWorldSpace.z < minPointInLocalSpace.z)	minPointInLocalSpace.z = vertexPosInWorldSpace.z;
					}
				}

				localBoundingBox = { minPointInLocalSpace , maxPointInLocalSpace, ((maxPointInLocalSpace + minPointInLocalSpace) * 0.5f) };
			}
			
		public:

			DynamicArray<Vertex> vertices;
			DynamicArray<ui32> indices;
			BoundingBox localBoundingBox;
			Material* material;
			ui32 meshResourceID;
			bool isStatic;
		};

	}
}
