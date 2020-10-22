#pragma once

#include "BaldLion/Rendering/SkyboxPlatformInterface.h"
#include "OpenGLTextureCubemap.h"

namespace BaldLion
{
	namespace Rendering
	{
		class OpenGLSkybox : public SkyboxPlatformInterface
		{

		public:
			virtual void Init(const std::string& path) override;
			virtual void Draw() override;

		private:
			OpenGLTextureCubemap* m_cubemapTexture;
		};
	}
}