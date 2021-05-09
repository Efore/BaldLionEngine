#pragma once
#include "BaldLion/Rendering/Mesh.h"
#include "BaldLion/Core/Memory/MemoryManager.h"

namespace BaldLion
{
	namespace Rendering
	{
		class PlaneMesh : public Mesh {

		public:
			PlaneMesh(Material* material, AABB aabb, const glm::mat4& worldTransform, bool isStatic, float size);

			void SetUpPlane();

		private:

			const float m_gridSize = 10.0f;
			float m_size;
		};
	}
}