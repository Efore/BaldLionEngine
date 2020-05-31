#pragma once
#include "Vertex.h"
#include "VertexArray.h"
#include "Material.h"
#include "BaldLion/Animation/Joint.h"
#include <vector>
#include <map>

namespace BaldLion
{
	namespace Rendering
	{
		class AnimatedMesh {

		public:

			AnimatedMesh(std::vector<AnimatedVertex> vertices, std::vector<uint32_t> indices, std::map<std::string, uint32_t> jointMapping, std::vector<Animation::Joint> joints, const Ref<Material>& material);
			~AnimatedMesh();

			void SetUpMesh();
			void Draw() const;

			inline std::vector<Animation::Joint>& GetJoints() { return m_joints; }
			inline const std::vector<Animation::Joint>& GetJoints() const { return m_joints; }

			inline std::map<std::string, uint32_t>& GetJointMapping() { return m_jointMapping; }
			inline const std::map<std::string, uint32_t>& GetJointMapping() const { return m_jointMapping; }

		private:

			std::vector<AnimatedVertex> m_vertices;
			std::vector<uint32_t> m_indices;
			
			std::map < std::string, uint32_t> m_jointMapping;
			std::vector<Animation::Joint> m_joints;

			Ref<VertexArray> m_vertexArray;
			Ref<Material> m_material;
		};
	}
}