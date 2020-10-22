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

			SkinnedMesh(BLVector<Vertex>& vertices, BLVector<VertexBoneData>& verticesBoneData, BLVector<uint32_t>& indices, const BLVector<Animation::Joint>& joints, Material* material);
			~SkinnedMesh();

			void SetUpMesh(const BLVector<Vertex>& vertices, const BLVector<VertexBoneData>& verticesBoneData, const BLVector<uint32_t>& indices);
			void Draw(const glm::mat4& worldTransform) const;

			inline BLVector<Animation::Joint>& GetJoints() { return m_joints; }
			inline const BLVector<Animation::Joint>& GetJoints() const { return m_joints; }

			static glm::mat4 AiMat4ToGlmMat4(const aiMatrix4x4& aiMat4);

		private:
			
			BLVector<Animation::Joint> m_joints;

			VertexArray* m_vertexArray;
			Material* m_material;
		};
	}
}