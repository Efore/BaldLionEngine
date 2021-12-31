#include "blpch.h"
#include "ResourceManager.h"
#include "BaldLion/Rendering/Texture.h"
#include "BaldLion/Rendering/Model.h"

namespace BaldLion
{
	namespace ResourceManagement
	{
		HashMap<ui32, Resource*> ResourceManager::s_resourceMap;
		std::mutex ResourceManager::s_resourceManagerMutex;

		void ResourceManager::Init()
		{
			s_resourceMap = HashMap<ui32, Resource*>(AllocationType::FreeList_Resources, 100);
		}

		void ResourceManager::Stop()
		{
			s_resourceMap.Delete();
		}

		bool ResourceManager::Exists(const std::string &path)
		{
			Resource* result = nullptr;

			if (s_resourceMap.TryGet(STRING_TO_STRINGID(path), result))
			{
				return true;
			}

			return false;
		}

		void ResourceManager::AddResource(Resource* resource)
		{
			std::lock_guard<std::mutex> lockGuard(s_resourceManagerMutex);

			if (resource && !s_resourceMap.Contains(resource->GetResourceID()))
			{
				s_resourceMap.Emplace(resource->GetResourceID(), std::move(resource));
			}
		}

		BaldLion::ResourceManagement::ResourceType ResourceManager::GetResourceTypeFromPath(const std::string &path)
		{			
			// Extracting name from lastpath			
			auto lastDot = path.rfind('.');

			const std::string extension = path.substr(lastDot, path.size() - 1);
			
			if (extension == ".obj"		|| 
				extension == ".fbx"		||
				extension == ".blend"	||
				extension == ".3ds"		||
				extension == ".dae")
			{
				return ResourceType::Model;
			}

			if (extension == ".png"	|| 
				extension == ".jpg"	|| 
				extension == ".jpeg"|| 
				extension == ".bmp"	||
				extension == ".tga"	||
				extension == ".psd")
			{
				return ResourceType::Texture;
			}

			return ResourceType::Count;
		}
	}
}