#pragma once
#include "BaldLion/Rendering/Mesh.h"
#include "BaldLion/Core/Memory/MemoryManager.h"

namespace BaldLion
{
	namespace Rendering
	{
		class SphereMesh : public Mesh {

		public:
			SphereMesh(Material* material, float radius, const std::string& meshName);

			void SetUpSphere();

		private:

			float m_radius;
			ui32 m_rings;
			ui32 m_sectors;
		};
	}
}