#pragma once

#include "BaldLion/Core/Core.h"
#include "BaldLion/Core/Containers/HashTable.h"

namespace BaldLion 
{
	namespace Rendering
	{
		enum TextureType
		{
			Texture2d = 0,
			CubeMap = 1
		};

		enum WrapMode {
			Repeat,
			Mirrored,
			ClampToEdge,
			ClampToBorder
		};

		class Texture
		{

		public:			

			virtual ~Texture() = default;

			virtual ui32 GetWidth() const = 0;
			virtual void SetWidth(ui32 width) = 0;
 
			virtual ui32 GetHeight() const = 0;
			virtual void SetHeight(ui32 width) = 0;

			virtual ui32 GetRendererID() const = 0;
			virtual void SetRendererID(ui32 rendererID) = 0;

			virtual void Bind(ui32 slot) const = 0;
			virtual void Bind() const = 0;

			virtual StringId GetName() const = 0;

			virtual int GetTextureType() const = 0;

			virtual void SetWrapMode(WrapMode xCoord, WrapMode yCoord) const = 0;
			
		};

		class Texture2D : public Texture
		{
		public:
			static Texture2D* Create(const std::string& path, bool emptyTexture = false);
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

			virtual ~TextureLibrary();

			void Init();

			void Add(Texture* texture);
			void Add(StringId name, Texture* texture);

			Texture* Load(const std::string& filepath, TextureType textureType);
			Texture* Load(const std::string& filepath,const unsigned char* textureData, int size, TextureType textureType);
			Texture* Load(StringId name, const std::string& filepath, TextureType textureType);

			void Clear();
			bool Exists(StringId name) const;

			static void GetNameFromPath(const std::string &path, StringId& name);

		private:
			HashTable<StringId, Texture*> m_textures;
			std::mutex m_textureLibraryMutex;
		};
	}
}
