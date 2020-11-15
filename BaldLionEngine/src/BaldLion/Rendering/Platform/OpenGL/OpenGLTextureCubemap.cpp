#include "blpch.h"
#include "OpenGLTextureCubemap.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace BaldLion
{
	namespace Rendering
	{
		OpenGLTextureCubemap::OpenGLTextureCubemap(const std::string& path)
		{
			TextureLibrary::GetNameFromPath(path, m_name);

			BL_PROFILE_FUNCTION();

			glGenTextures(1, &m_rendererID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_rendererID);

			for (size_t i = 0; i < 6; ++i)
			{
				int width, height, channels;
				stbi_uc* data = nullptr;

				{
					BL_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D");

					const std::string texturePath = TextureCubeMap::GetSkyboxTexturePath(path, i);
					data = stbi_load(texturePath.c_str(), &width, &height, &channels, 0);
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

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (int)i, 0, internalFormat, m_width, m_height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

				stbi_image_free(data);
			}
			 
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);		
		}

		OpenGLTextureCubemap::~OpenGLTextureCubemap()
		{
			BL_PROFILE_FUNCTION();

			glDeleteTextures(1, &m_rendererID);
		}

		void OpenGLTextureCubemap::Bind(uint32_t slot) const
		{
			BL_PROFILE_FUNCTION();	

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_rendererID);

		}
	}
}