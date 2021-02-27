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

			SkinnedMesh(const DynamicArray<Animation::Joint>& joints, Material* material);
			~SkinnedMesh();

			using Mesh::SetUpMesh;
			void SetUpMesh(const DynamicArray<Vertex>& vertices, const DynamicArray<VertexBoneData>& verticesBoneData, const DynamicArray<ui32>& indices);			

			virtual void Draw(const glm::mat4& worldTransform) const override;

			inline DynamicArray<Animation::Joint>& GetJoints() { return m_joints; }
			inline const DynamicArray<Animation::Joint>& GetJoints() const { return m_joints; }

			static glm::mat4 AiMat4ToGlmMat4(const aiMatrix4x4& aiMat4);

		private:
			
			DynamicArray<Animation::Joint> m_joints;
		};
	}
}