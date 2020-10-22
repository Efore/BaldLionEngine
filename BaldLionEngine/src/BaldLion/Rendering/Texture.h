#pragma once

#include <string>
#include "BaldLion/Core/Core.h"

namespace BaldLion 
{
	namespace Rendering
	{

#define BL_TEXTURE_TYPE_2D 0
#define BL_TEXTURE_TYPE_CUBEMAP 1

		class Texture
		{
		public:

			virtual ~Texture() = default;

			virtual uint32_t GetWidth() const = 0;
			virtual uint32_t GetHeight() const = 0;
			virtual uint32_t GetRendererID() const = 0;

			virtual void Bind(uint32_t slot = 0) const = 0;

			virtual const std::string& GetName() const = 0;

			virtual int GetTextureType() const = 0;
			
		};

		class Texture2D : public Texture
		{
		public:
			static Texture2D* Create(const std::string& path);
			static Texture2D* Create(const std::string& path, const unsigned char* textureData, int size);
		};

		class TextureCubeMap : public Texture
		{
		public:
			static TextureCubeMap* Create(const std::string& path);

		protected:
			static const std::string GetSkyboxTexturePath(const std::string& path, int index);
		};

		

		class TextureLibrary
		{
		public:

			~TextureLibrary();

			void Add(Texture* texture);
			void Add(const std::string& name, Texture* texture);

			Texture* Load(const std::string& filepath, int textureType);
			Texture* Load(const std::string& filepath,const unsigned char* textureData, int size, int textureType);
			Texture* Load(const std::string& name, const std::string& filepath, int textureType);			

			bool Exists(const std::string& name) const;

			static const std::string GetNameFromPath(const std::string &path);

		private:
			std::unordered_map<std::string, Texture*> m_textures;
		};
	}
}
