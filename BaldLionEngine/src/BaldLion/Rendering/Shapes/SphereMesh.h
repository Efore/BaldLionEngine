#pragma once
#include "BaldLion/Rendering/Mesh.h"
#include "BaldLion/Core/Memory/MemoryManager.h"

namespace BaldLion
{
	namespace Rendering
	{
		class SphereMesh : public Mesh {

		public:
			SphereMesh(Material* material, AABB aabb, const glm::mat4& worldTransform, bool isStatic, float radius, ui32 rings, ui32 sectors);

			void SetUpSphere();

		private:

			float m_radius;
			ui32 m_rings;
			ui32 m_sectors;
		};
	}
}