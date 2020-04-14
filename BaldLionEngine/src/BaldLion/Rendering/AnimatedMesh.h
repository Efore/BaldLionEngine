#pragma once

#include "Mesh.h"
#include "BaldLion/Animation/Joint.h"

namespace BaldLion
{
	namespace Rendering
	{
		class AnimatedMesh : public Mesh {

		public:

			AnimatedMesh(std::vector<Vertex> vertices, std::vector<VertexBoneData> verticesBoneData, std::vector<Animation::Joint> joints, std::vector<uint32_t> indices, const Ref<Material>& material);
			~AnimatedMesh();

			virtual void SetUpMesh() override;
			virtual void Draw() const override;

		private:

			std::vector<VertexBoneData> m_verticesBoneData;
			std::vector<Animation::Joint> m_joints;
		};
	}
}