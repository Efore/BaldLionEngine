#pragma once
#include "BaldLion/Rendering/Texture.h"

namespace BaldLion
{
	namespace Rendering
	{
		class OpenGLTexture2D : public Texture2D
		{
		public:
			OpenGLTexture2D(const std::string& path);
			OpenGLTexture2D(const std::string& path, const unsigned char* textureData, int size);
			virtual ~OpenGLTexture2D();

			virtual uint32_t GetWidth() const override { return m_width; }
			virtual uint32_t GetHeight() const override { return m_height; }
			virtual uint32_t GetRendererID() const override { return m_rendererID; }

			virtual void Bind(uint32_t slot = 0) const override;

			virtual const std::string& GetName() const override { return m_name; }

			virtual int GetTextureType() const override { return BL_TEXTURE_TYPE_2D; }

		private:
			std::string m_path;
			std::string m_name;

			uint32_t m_width, m_height;
			uint32_t m_rendererID;
		};
	}
}