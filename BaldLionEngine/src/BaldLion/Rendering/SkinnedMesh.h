#pragma once
#include "Vertex.h"
#include "VertexArray.h"
#include "Material.h"
#include "BaldLion/Animation/Joint.h"
#include <assimp/scene.h>
#include <vector>
#include <map>

namespace BaldLion
{
	namespace Rendering
	{
		class SkinnedMesh {

		public:

			SkinnedMesh(const DynamicArray<Animation::Joint>& joints, Material* material);
			~SkinnedMesh();

			void SetUpMesh(const DynamicArray<Vertex>& vertices, const DynamicArray<VertexBoneData>& verticesBoneData, const DynamicArray<uint32_t>& indices);
			void Draw(const glm::mat4& worldTransform) const;

			inline DynamicArray<Animation::Joint>& GetJoints() { return m_joints; }
			inline const DynamicArray<Animation::Joint>& GetJoints() const { return m_joints; }

			static glm::mat4 AiMat4ToGlmMat4(const aiMatrix4x4& aiMat4);

		private:
			
			DynamicArray<Animation::Joint> m_joints;

			VertexArray* m_vertexArray;
			Material* m_material;
		};
	}
}