#pragma once
#include "BaldLion/Rendering/Texture.h"

namespace BaldLion
{
	namespace Rendering
	{
		class OpenGLTexture2D : public Texture2D
		{
		public:
			OpenGLTexture2D(const std::string& path, bool emptyTexture);
			OpenGLTexture2D(const std::string& path, const unsigned char* textureData, int size);
			
			virtual ~OpenGLTexture2D();

			virtual ui32 GetWidth() const override { return m_width; }
			virtual void SetWidth(ui32 width) override { m_width = width; }

			virtual ui32 GetHeight() const override { return m_height; }
			virtual void SetHeight(ui32 height) override { m_height = height; }

			virtual ui32 GetRendererID() const override { return m_rendererID; }
			virtual void SetRendererID(ui32 rendererID) override { m_rendererID = rendererID; }

			virtual void Bind(ui32 slot) const override;
			virtual void Bind() const override;

			virtual int GetTextureType() const override { return TextureType::Texture2d; }

			virtual void SetWrapMode(WrapMode xCoord, WrapMode yCoord) const;

		private:
			ui32 m_width, m_height;
			ui32 m_rendererID;
		};
	}
}