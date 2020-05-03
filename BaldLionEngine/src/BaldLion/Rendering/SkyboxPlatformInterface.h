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

			enum class SkyboxPlatform
			{
				None = 0, OpenGL = 1
			};

			virtual void Init(const std::string& path);
			virtual void Draw() = 0;			

			inline static SkyboxPlatform GetAPI() { return s_skyboxPlatform; }		

		private:
			static SkyboxPlatform s_skyboxPlatform;

		protected:			
			Ref<VertexArray> m_vertexArray;
			Ref<Shader> m_skyboxShader;
		};
	}
}
