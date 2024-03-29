#pragma once

#include "BaldLion/Core/Containers/HashMap.h"
#include "Resource.h"
#include "BaldLion/Rendering/Material.h"
#include "BaldLion/Rendering/Model.h"
#include <filesystem>

namespace BaldLion
{
	namespace ResourceManagement
	{
		class ResourceManager
		{
		public:
			static void Init();
			static void Stop();

			template <typename T>
			static T* LoadResource(const std::string &path);

			template <typename T>
			static T* AddResource(const std::string &path, ResourceType resourceType);

			static HashMap<ui32, Resource*>& GetResourceMap() { return s_resourceMap; }

			static bool Exists(const std::string &path);
			static void AddResource(Resource* resource);
			static void LoadAssets();

			static bool HasMetafile(const Resource* resource);			

			static const char* GetResourceSuffixFromType(ResourceType type);

		private:

			static ResourceType GetResourceTypeFromPath(const std::string &path);
			static void LoadMetaFile(const std::string &path);
			static void LoadPrimitiveShapeMeshes();

		private:
			static HashMap<ui32, Resource*> s_resourceMap;
			static std::mutex s_resourceManagerMutex;
		};

		template <typename T>
		T* ResourceManager::LoadResource(const std::string &path)
		{
			static_assert(std::is_base_of<Resource, T>::value, "T must inherit from Resource");

			auto resourcePath = std::filesystem::path(path);

			std::lock_guard<std::mutex> lockGuard(s_resourceManagerMutex);

			Resource* result = nullptr;

			s_resourceMap.TryGet(BL_STRING_TO_STRINGID(resourcePath.make_preferred().string()), result);

			return (T*)result;
		}		

		template <typename T>
		T* ResourceManager::AddResource(const std::string &path, ResourceType resourceType)
		{			
			static_assert(std::is_base_of<Resource, T>::value, "T must inherit from Resource");	

			Resource* result = LoadResource<T>(path);

			if (result)
			{
				return (T*)result;
			}

			auto resourcePath = std::filesystem::path(path);
			const std::string formatedPath = resourcePath.make_preferred().string();

			switch (resourceType)
			{
			case ResourceType::Texture:
				
				result = Rendering::Texture2D::Create(formatedPath);
				break;

			case ResourceType::Model:

				result = MemoryManager::New<Rendering::Model>(formatedPath.c_str(), AllocationType::FreeList_Renderer, formatedPath);

				((Rendering::Model*)result)->SetUpModel();

				break;

			case ResourceType::Skeleton:
				break;

			case ResourceType::Material:
			{
				Rendering::Material::MaterialProperties materialProperties
				{
					BL_STRING_TO_STRINGID(path),
					glm::vec3(1.0f),
					glm::vec3(1.0f),
					glm::vec3(1.0f),
					glm::vec3(1.0f),
					32.0f,
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					Material::BlendMode::None,
					Material::DepthBufferMode::TestAndWrite,
					Material::CullingMode::Back,
					(ui8)Material::ShadowsSettingsBitMask::CastShadows | (ui8)Material::ShadowsSettingsBitMask::ReceiveShadows
				};

				result = Material::Create(formatedPath, materialProperties);
				((Material*)result)->AssignShader();
			}
			break;

			case ResourceType::Shader:

				result = Shader::Create(formatedPath);
				break;

			default:
				break;
			}

			std::lock_guard<std::mutex> lockGuard(s_resourceManagerMutex);

			if (result)
			{
				s_resourceMap.Emplace(result->GetResourceID(), std::move(result));
			}

			return (T*)result;
		}

	}
}