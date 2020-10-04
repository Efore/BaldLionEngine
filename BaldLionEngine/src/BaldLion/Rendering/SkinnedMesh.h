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

			SkinnedMesh(const std::vector<Vertex>& vertices, const std::vector<VertexBoneData>& verticesBoneData, const std::vector<uint32_t>& indices, std::vector<Animation::Joint> joints, const Ref<Material>& material);
			~SkinnedMesh();

			void SetUpMesh(const std::vector<Vertex>& vertices, const std::vector<VertexBoneData>& verticesBoneData, const std::vector<uint32_t>& indices);
			void Draw(const glm::mat4& worldTransform) const;

			inline std::vector<Animation::Joint>& GetJoints() { return m_joints; }
			inline const std::vector<Animation::Joint>& GetJoints() const { return m_joints; }

			static glm::mat4 AiMat4ToGlmMat4(const aiMatrix4x4& aiMat4);

		private:
			
			std::vector<Animation::Joint> m_joints;

			Ref<VertexArray> m_vertexArray;
			Ref<Material> m_material;
		};
	}
}