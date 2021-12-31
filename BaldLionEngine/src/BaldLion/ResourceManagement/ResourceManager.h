#pragma once

#include "BaldLion/Core/Containers/HashMap.h"
#include "Resource.h"

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

			static bool Exists(const std::string &path);

			template <typename T>
			static T* AddResource(const std::string &path, ResourceType resourceType);

			static void AddResource(Resource* resource);

		private:

			static ResourceType GetResourceTypeFromPath(const std::string &path);

			static HashMap<ui32, Resource*> s_resourceMap;
			static std::mutex s_resourceManagerMutex;
		};

		template <typename T>
		T* ResourceManager::LoadResource(const std::string &path)
		{
			static_assert(std::is_base_of<Resource, T>::value, "T must inherit from Resource");

			std::lock_guard<std::mutex> lockGuard(s_resourceManagerMutex);

			Resource* result = nullptr;

			s_resourceMap.TryGet(STRING_TO_STRINGID(path), result);

			return (T*)result;
		}		

		template <typename T>
		T* ResourceManager::AddResource(const std::string &path, ResourceType resourceType)
		{
			static_assert(std::is_base_of<Resource, T>::value, "T must inherit from Resource");

			std::lock_guard<std::mutex> lockGuard(s_resourceManagerMutex);

			Resource* result = nullptr;

			switch (resourceType)
			{
			case ResourceType::Texture:

				result = Rendering::Texture2D::Create(path);
				break;

			case ResourceType::Model:

				result = MemoryManager::New<Rendering::Model>(path.c_str(), AllocationType::FreeList_Renderer, path);

				((Rendering::Model*)result)->SetUpModel();

				break;

			case ResourceType::Skeleton:
				break;

			case ResourceType::Material:
			{
				Rendering::Material::MaterialProperties materialProperties
				{
					STRING_TO_STRINGID(path),
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
					(ui8)Material::ShadowsSettingsBitMask::CastShadows
				};

				result = Material::Create(path, materialProperties);
				((Material*)result)->AssignShader();
			}
			break;

			case ResourceType::Shader:

				result = Shader::Create(path);
				break;

			default:
				break;
			}

			if (result)
			{
				s_resourceMap.Emplace(result->GetResourceID(), std::move(result));
			}

			return (T*)result;
		}

	}
}