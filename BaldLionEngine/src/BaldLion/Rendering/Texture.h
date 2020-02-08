#pragma once

#include <string>
#include "BaldLion/Core.h"

namespace BaldLion {
	namespace Rendering
	{
		class Texture
		{
		public:

			virtual ~Texture() = default;

			virtual uint32_t GetWidth() const = 0;
			virtual uint32_t GetHeight() const = 0;

			virtual void Bind(uint32_t slot = 0) const = 0;

			virtual const std::string& GetName() const = 0;

			static std::string GetNameFromPath(const std::string &path);
		};

		class Texture2D : public Texture
		{
		public:
			static Ref<Texture2D> Create(const std::string& path);

		};

		class TextureLibrary
		{
		public:
			void Add(const Ref<Texture2D>& texture);
			void Add(const std::string& name, const Ref<Texture2D>& texture);

			Ref<Texture2D> Load(const std::string& filepath);
			Ref<Texture2D> Load(const std::string& name, const std::string& filepath);

			Ref<Texture2D> Get(const std::string& name);

			bool Exists(const std::string& name) const;

		private:
			std::unordered_map<std::string, Ref<Texture2D>> m_textures;
		};
	}
}
