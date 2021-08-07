#include "blpch.h"
#include "Material.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLMaterial.h"

namespace BaldLion
{
	namespace Rendering
	{
		Material* Material::Create(const std::string& matName, const MaterialProperties& materialProperties)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
				case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
				case RendererPlatformInterface::RendererPlatform::OpenGL:	
					return MemoryManager::New<OpenGLMaterial>(std::string("Material " + matName).c_str(),AllocationType::FreeList_Renderer, matName, materialProperties);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}		

		//
		//MATERIAL LIBRARY
		//

		std::mutex MaterialLibrary::s_materialLibraryMutex;
		BaldLion::HashTable<StringId, Material*> MaterialLibrary::s_materials;

		void MaterialLibrary::Init()
		{
			s_materials = HashTable<StringId, Material*>(BaldLion::Memory::AllocationType::FreeList_Renderer, 10);
		}

		void MaterialLibrary::Add(Material* material)
		{
			auto name = material->GetMaterialName();
			BL_CORE_ASSERT(!s_materials.Contains(name), "Shader already exists!");
			s_materials.Emplace(name, std::move(material));
		}

		Material* MaterialLibrary::Load(const std::string& matName, Material::MaterialProperties* materialProperties)
		{
			std::lock_guard<std::mutex> lockGuard(s_materialLibraryMutex);

			StringId name = STRING_TO_STRINGID(matName);

			if (s_materials.Contains(name))
				return s_materials.Get(name);

			Material* material = Material::Create(matName, *materialProperties);
			Add(material);
			return material;
		}

		void MaterialLibrary::Clear()
		{
			s_materials.Clear();
		}
	}
}
