#pragma once
#include "Vertex.h"
#include "VertexArray.h"
#include "Material.h"
#include "BaldLion/Utils/GeometryUtils.h"
#include "GeometryData.h"
#include "BaldLion/ECS/Components/ECSMeshComponent.h"
#include "BaldLion/ECS/ECSManager.h"
#include "BaldLion/ResourceManagement/ResourceManager.h"
#include "BaldLion/Animation/Skeleton.h"

namespace BaldLion
{
	using namespace GeometryUtils;
	using namespace Animation;

	namespace Rendering
	{
		class Mesh : public ResourceManagement::Resource{

		public:
			Mesh(Material* material, const std::string& meshPath, bool isPrimitive = false);
			~Mesh();

			void SetUpMesh(const DynamicArray<Vertex>& vertices, const DynamicArray<ui32>& indices);
			void Draw() const;

			inline const Material* GetMaterial() const { return m_material; }
			inline Material* GetMaterial() { return m_material; }

			inline const DynamicArray<Vertex>& GetVertices() const { return m_geometryData->vertices; }
			inline DynamicArray<Vertex>& GetVertices() { return m_geometryData->vertices; }

			inline const DynamicArray<ui32>& GetIndices() const { return m_geometryData->indices; }
			inline DynamicArray<ui32>& GetIndices() { return m_geometryData->indices; }

			inline const VertexArray* GetVertexArray() const { return m_vertexArray; }
			inline VertexArray* GetVertexArray() { return m_vertexArray; }

			inline Skeleton* GetSkeleton() { return m_skeleton; }
			inline const Skeleton* GetSkeleton() const { return m_skeleton; }

			inline void SetSkeleton(Skeleton* skeleton) { m_skeleton = skeleton; }

			inline DynamicArray<VertexBone>& GetVertexBones() { return m_vertexBones; }
			inline const DynamicArray<VertexBone>& GetVertexBones() const { return m_vertexBones; }

			void SetVertexBones(const DynamicArray<VertexBone>& vertexBones);

			void GenerateMeshComponent(ECS::ECSManager* ecsManager, bool isStatic, const ECS::ECSTransformComponent* transformComponent, ECS::ECSMeshComponent*& meshComponent, ECS::ECSSkeletonComponent*& skeletonComponent) const ;
			ECS::ECSSkeletonComponent* GenerateSkeletonComponent(ECS::ECSManager* ecsManager) const;

			ECS::ECSEntityID GenerateEntity(ECS::ECSManager* ecsManager, bool isStatic) const;

		protected:

			GeometryData* m_geometryData;
			VertexArray* m_vertexArray;
			Material* m_material;
			Skeleton* m_skeleton;
			DynamicArray<VertexBone> m_vertexBones;
			bool m_isPrimitive = false;
		};
	}
}