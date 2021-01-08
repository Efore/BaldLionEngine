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
			virtual ui32 GetHeight() const override { return m_height; }
			virtual ui32 GetRendererID() const { return m_rendererID; }

			virtual void Bind(ui32 slot = 0) const override;

			virtual StringId GetName() const override { return m_name; }

			virtual int GetTextureType() const override { return BL_TEXTURE_TYPE_CUBEMAP; }


		private:			
			StringId m_name;

			ui32 m_width;
			ui32 m_height;

			ui32 m_rendererID;
		};
	}
}