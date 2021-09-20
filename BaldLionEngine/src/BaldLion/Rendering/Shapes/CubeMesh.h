#pragma once
#include "BaldLion/Rendering/Mesh.h"
#include "BaldLion/Core/Memory/MemoryManager.h"

namespace BaldLion
{
	namespace Rendering
	{
		class CubeMesh : public Mesh {

		public:
			CubeMesh(Material* material, float size);

			void SetUpCube();

		private:

			float m_size;
		};
	}
}