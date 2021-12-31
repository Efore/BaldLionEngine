#pragma once
#include "BaldLion/Rendering/Mesh.h"
#include "BaldLion/Core/Memory/MemoryManager.h"

namespace BaldLion
{
	namespace Rendering
	{
		class PlaneMesh : public Mesh {

		public:
			PlaneMesh(Material* material, float size, const std::string& meshName);

			void SetUpPlane();

		private:

			const float m_gridSize = 10.0f;
			float m_size;
		};
	}
}