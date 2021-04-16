#pragma once

#include "BaldLion/Animation/Joint.h"
#include "Mesh.h"
#include <assimp/scene.h>

namespace BaldLion
{
	namespace Rendering
	{
		class SkinnedMesh : public Mesh {

		public:

			SkinnedMesh(Material* material, GeometryUtils::AABB aabb, const glm::mat4& worldTransform, const DynamicArray<Animation::Joint>& joint);
			~SkinnedMesh();

			void SetUpMesh(const DynamicArray<VertexBoneData>& verticesBoneData, const DynamicArray<Vertex>& vertices, const DynamicArray<ui32>& indices);

			virtual void Draw() const override;

			inline DynamicArray<Animation::Joint>& GetJoints() { return m_joints; }
			inline const DynamicArray<Animation::Joint>& GetJoints() const { return m_joints; }			

		private:
			
			DynamicArray<Animation::Joint> m_joints;
		};
	}
}