#include "blpch.h"
#include "OpenGLTexture2D.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace BaldLion
{
	namespace Rendering
	{
		OpenGLTexture2D::OpenGLTexture2D(const std::string& path, bool emptyTexture) : Texture2D(path)
		{
			if (emptyTexture)
				return;
			
			BL_PROFILE_FUNCTION();

			int width, height, channels;
			stbi_uc* data = nullptr;
			stbi_set_flip_vertically_on_load(1);

			{
				BL_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D", Optick::Category::Rendering);
				data = stbi_load(path.c_str(), &width, &height, &channels, 0);
			}

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
			else if (channels == 2)
			{
				internalFormat = GL_RG8;
				dataFormat = GL_RG;
			}
			else if (channels == 1)
			{
				internalFormat = GL_R8;
				dataFormat = GL_RED;
			}

			BL_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
			glTextureStorage2D(m_rendererID, 1, internalFormat, m_width, m_height);
			glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, dataFormat, GL_UNSIGNED_BYTE, data);

			glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glGenerateTextureMipmap(m_rendererID);

			stbi_image_free(data);
		}

		OpenGLTexture2D::OpenGLTexture2D(const std::string& path, const unsigned char* textureData, int size) : Texture2D(path)
		{		

			BL_PROFILE_FUNCTION();

			int width, height, channels;
			stbi_uc* data = nullptr;
			stbi_set_flip_vertically_on_load(1);

			{
				BL_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D", Optick::Category::Rendering);				
				data = stbi_load_from_memory(textureData, size, &width, &height, &channels, 0);
			}

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
			glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, dataFormat, GL_UNSIGNED_BYTE, data);

			glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glGenerateTextureMipmap(m_rendererID);

			stbi_image_free(data);
		}

		OpenGLTexture2D::~OpenGLTexture2D()
		{
			BL_PROFILE_FUNCTION();

			glDeleteTextures(1, &m_rendererID);
		}

		void OpenGLTexture2D::Bind(ui32 slot) const
		{
			BL_PROFILE_FUNCTION();
			
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTextureUnit(slot, m_rendererID);
			
		}

		void OpenGLTexture2D::Bind() const
		{
			BL_PROFILE_FUNCTION();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_rendererID);			
		}

		void OpenGLTexture2D::SetWrapMode(WrapMode xCoord, WrapMode yCoord) const
		{
			glBindTexture(GL_TEXTURE_2D, m_rendererID);

			switch (xCoord)
			{
			case BaldLion::Rendering::Repeat:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				break;
			case BaldLion::Rendering::Mirrored:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				break;
			case BaldLion::Rendering::ClampToEdge:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				break;
			case BaldLion::Rendering::ClampToBorder:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				break;
			default:
				break;
			}

			switch (yCoord)
			{
			case BaldLion::Rendering::Repeat:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				break;
			case BaldLion::Rendering::Mirrored:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				break;
			case BaldLion::Rendering::ClampToEdge:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
			case BaldLion::Rendering::ClampToBorder:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				break;
			default:
				break;
			}
			
			

		}

	}
}