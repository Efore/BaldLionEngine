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

			virtual ui32 GetWidth() const override { return m_width; }
			virtual void SetWidth(ui32 width) override { m_width = width; }

			virtual ui32 GetHeight() const override { return m_height; }
			virtual void SetHeight(ui32 height) override { m_height = height; }

			virtual ui32 GetRendererID() const { return m_rendererID; }
			virtual void SetRendererID(ui32 rendererID) override { m_rendererID = rendererID; }

			virtual void Bind(ui32 slot) const override;
			virtual void Bind() const override;

			virtual int GetTextureType() const override { return TextureType::CubeMap; }	
			virtual void SetWrapMode(WrapMode xCoord, WrapMode yCoord) const;


		private:			
			ui32 m_width;
			ui32 m_height;

			ui32 m_rendererID;
		};
	}
}