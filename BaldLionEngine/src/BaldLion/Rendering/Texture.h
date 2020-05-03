#pragma once

#include <string>
#include "BaldLion/Core/Core.h"

namespace BaldLion 
{
	namespace Rendering
	{

#define TEXTURE_TYPE_2D 0
#define TEXTURE_TYPE_CUBEMAP 1

		class Texture
		{
		public:

			virtual ~Texture() = default;

			virtual uint32_t GetWidth() const = 0;
			virtual uint32_t GetHeight() const = 0;

			virtual void Bind(uint32_t slot = 0) const = 0;

			virtual const std::string& GetName() const = 0;

			virtual const int GetTextureType() const = 0;
			
		};

		class Texture2D : public Texture
		{
		public:
			static Ref<Texture2D> Create(const std::string& path);
		};

		class TextureCubeMap : public Texture
		{
		public:
			static Ref<TextureCubeMap> Create(const std::string& path);

		protected:
			static const std::string GetSkyboxTexturePath(const std::string& path, int index);
		};

		

		class TextureLibrary
		{
		public:
			void Add(const Ref<Texture>& texture);
			void Add(const std::string& name, const Ref<Texture>& texture);

			Ref<Texture> Load(const std::string& filepath, int textureType);
			Ref<Texture> Load(const std::string& name, const std::string& filepath, int textureType);

			Ref<Texture> Get(const std::string& name);

			bool Exists(const std::string& name) const;

			static std::string GetNameFromPath(const std::string &path);

		private:
			std::unordered_map<std::string, Ref<Texture>> m_textures;
		};
	}
}
