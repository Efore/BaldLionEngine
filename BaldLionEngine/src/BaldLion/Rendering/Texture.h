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

			virtual const char* GetName() const = 0;

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
			static const std::string GetSkyboxTexturePath(const std::string& path, size_t index);
		};

		

		class TextureLibrary
		{
		public:

			void Clear();
			virtual ~TextureLibrary();

			void Add(Texture* texture);
			void Add(const char*, Texture* texture);

			Texture* Load(const std::string& filepath, int textureType);
			Texture* Load(const std::string& filepath,const unsigned char* textureData, int size, int textureType);
			Texture* Load(const char* name, const std::string& filepath, int textureType);

			bool Exists(const char* name) const;

			static void GetNameFromPath(const std::string &path, char* name);

		private:
			std::unordered_map<const char*, Texture*> m_textures;
		};
	}
}
