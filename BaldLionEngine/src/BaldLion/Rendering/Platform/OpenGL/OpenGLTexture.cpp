#include "blpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace BaldLion
{
	namespace Rendering
	{
		OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
			: m_path(path)
		{
			int width, height, channels;

			stbi_set_flip_vertically_on_load(1);
			stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

			BL_CORE_ASSERT(data, "Failed to load image!");

			m_width = width;
			m_height = height;

			GLenum internalFormat = 0, dataFormat = 0;

			if (channels == 4)
			{
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
			}
			else if (channels == 3)
			{
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
			}

			BL_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
			glTextureStorage2D(m_rendererID, 1, internalFormat, m_width, m_height);

			glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, dataFormat, GL_UNSIGNED_BYTE, data);
			glGenerateTextureMipmap(m_rendererID);

			stbi_image_free(data);

			// Extracting name from lastpath
			auto lastSlash = path.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			auto lastDot = path.rfind('.');
			auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;

			m_name = path.substr(lastSlash, count);
		}

		OpenGLTexture2D::~OpenGLTexture2D()
		{
			glDeleteTextures(1, &m_rendererID);
		}

		void OpenGLTexture2D::Bind(uint32_t slot) const
		{
			glBindTextureUnit(slot, m_rendererID);
		}
	}
}