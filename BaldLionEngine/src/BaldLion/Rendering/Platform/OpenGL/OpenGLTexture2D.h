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

			virtual ui32 GetWidth() const override { return m_width; }
			virtual ui32 GetHeight() const override { return m_height; }
			virtual ui32 GetRendererID() const override { return m_rendererID; }

			virtual void Bind(ui32 slot = 0) const override;

			virtual StringId GetName() const override { return m_name; }

			virtual int GetTextureType() const override { return BL_TEXTURE_TYPE_2D; }

		private:
			StringId m_name;

			ui32 m_width, m_height;
			ui32 m_rendererID;
		};
	}
}