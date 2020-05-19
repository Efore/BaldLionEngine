#pragma once
#include "BaldLion/Rendering/Texture.h"

namespace BaldLion
{
	namespace Rendering
	{
		class OpenGLTextureCubemap : public TextureCubeMap
		{
		public:
			OpenGLTextureCubemap(const std::string& path);
			virtual ~OpenGLTextureCubemap();

			virtual uint32_t GetWidth() const override { return m_width; }
			virtual uint32_t GetHeight() const override { return m_height; }

			virtual void Bind(uint32_t slot = 0) const override;

			virtual const std::string& GetName() const override { return m_name; }

			virtual const int GetTextureType() const override { return BL_TEXTURE_TYPE_CUBEMAP; }

			const int GetRendererID() const { return m_rendererID; }

		private:
			std::string m_path;
			std::string m_name;

			uint32_t m_width;
			uint32_t m_height;

			uint32_t m_rendererID;
		};
	}
}