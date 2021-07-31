#pragma once
#include "Vertex.h"
#include "VertexArray.h"
#include "Material.h"
#include "BaldLion/Utils/GeometryUtils.h"
#include "GeometryData.h"
#include "BaldLion/Animation/Skeleton.h"
#include <vector>

namespace BaldLion
{
	using namespace GeometryUtils;

	namespace Rendering
	{
		class Mesh {

		public:
			Mesh(Material* material, AABB aabb, const glm::mat4& worldTransform, bool isStatic);
			~Mesh();

			void SetUpMesh(const DynamicArray<Vertex>& vertices, const DynamicArray<ui32>& indices);

			virtual void Draw() const;

			inline const Material* GetMaterial() const { return m_material; }
			inline Material* GetMaterial() { return m_material; }
			inline const AABB& GetAABB() const { return m_aabb; }

			inline const DynamicArray<Vertex>& GetVertices() const { return m_geometryData->vertices; }
			inline DynamicArray<Vertex>& GetVertices() { return m_geometryData->vertices; }

			inline const DynamicArray<ui32>& GetIndices() const { return m_geometryData->indices; }
			inline DynamicArray<ui32>& GetIndices() { return m_geometryData->indices; }

			inline const VertexArray* GetVertexArray() const { return m_vertexArray; }
			inline VertexArray* GetVertexArray() { return m_vertexArray; }

			inline const bool GetIsStatic() const { return m_isStatic; }
			inline const glm::mat4& GetWorldTransform() const { return m_worldTransform; }

			const Animation::Skeleton* GetSkelton() const { return m_skeleton; }
			void SetSkeleton(Animation::Skeleton* skeleton);

		protected:

			AABB RecalculateAABB() const;

		protected:

			glm::mat4 m_worldTransform;
			AABB m_aabb;

			GeometryData* m_geometryData;			
			VertexArray* m_vertexArray;
			Material* m_material;

			Animation::Skeleton* m_skeleton;

			bool m_isStatic;
		};
	}
}