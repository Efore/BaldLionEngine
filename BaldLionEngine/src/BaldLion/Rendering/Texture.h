#pragma once

#include "BaldLion/Core/Core.h"
#include "BaldLion/Core/Containers/HashTable.h"
#include "BaldLion/ResourceManagement/Resource.h"
#include "BaldLion/Utils/StringUtils.h"

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

		class Texture : public ResourceManagement::Resource
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

			virtual int GetTextureType() const = 0;

			virtual void SetWrapMode(WrapMode xCoord, WrapMode yCoord) const = 0;

		protected:

			Texture(const std::string& path) : ResourceManagement::Resource(BL_STRING_TO_STRINGID(path), StringUtils::GetFileNameFromPath(path), ResourceManagement::ResourceType::Texture) {}
		};

		class Texture2D : public Texture
		{
		public:		

			static Texture2D* Create(const std::string& path, bool emptyTexture = false);
			static Texture2D* Create(const std::string& path, const unsigned char* textureData, int size);

		protected:

			Texture2D(const std::string& path) : Texture(path) {}
		};

		class TextureCubeMap : public Texture
		{
		public:
			static TextureCubeMap* Create(const std::string& path);

		protected:
			TextureCubeMap(const std::string& path) : Texture(path) {}
			static const std::string GetSkyboxTexturePath(const std::string& path, size_t index);
		};		


	}
}
