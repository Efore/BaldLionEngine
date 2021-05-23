#pragma once
#include "Texture.h"
#include "VertexArray.h"
#include <glm/glm.hpp>


namespace BaldLion
{
	namespace Rendering
	{
		class SkyboxPlatformInterface
		{
		public:

			virtual void Init(const std::string& path);
			virtual void Draw() = 0;	
			virtual void Stop();

			static SkyboxPlatformInterface* Create();

		protected:			
			VertexArray* m_vertexArray;
			Shader* m_skyboxShader;
		};
	}
}
