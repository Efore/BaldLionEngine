#include "blpch.h"
#include "ResourceManager.h"
#include "BaldLion/Rendering/Texture.h"
#include "BaldLion/Rendering/Model.h"
#include "BaldLion/Animation/AnimationManager.h"
#include "BaldLion/Rendering/Shapes/CubeMesh.h"
#include "BaldLion/Rendering/Shapes/PlaneMesh.h"
#include "BaldLion/Rendering/Shapes/SphereMesh.h"

#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

#define ASSETS_PATH "assets/"

#define EXTERNAL_RESOURCES_METADATA_PATH "assets/gameAssets/externalResourcesPaths.meta"

#define YAML_KEY_EXTERNAL_RESOURCES_LIST	"ExternalResourcesList"
#define YAML_KEY_EXTERNAL_RESOURCE_PATH		"ExternalResourcePath"

namespace BaldLion
{
	namespace ResourceManagement
	{
		HashMap<ui32, Resource*> ResourceManager::s_resourceMap;
		DynamicArray<StringId> ResourceManager::s_externalResourcesPaths;

		std::mutex ResourceManager::s_resourceManagerMutex;

		void ResourceManager::Init()
		{
			s_resourceMap = HashMap<ui32, Resource*>(AllocationType::FreeList_Resources, 96);	
			s_externalResourcesPaths = DynamicArray<StringId>(AllocationType::FreeList_Resources, 64);
			DeserializeExternalResourcesPath();
		}

		void ResourceManager::Stop()
		{
			SerializeExternalResourcesPaths();
			s_resourceMap.Delete();
			s_externalResourcesPaths.Delete();
		}

		bool ResourceManager::Exists(const std::string &path)
		{
			auto resourcePath = std::filesystem::path(path); 			

			Resource* result = nullptr;

			if (s_resourceMap.TryGet(BL_STRING_TO_STRINGID(resourcePath.make_preferred().string()), result))
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

		ResourceType ResourceManager::GetResourceTypeFromPath(const std::string &path)
		{			
			// Extracting name from lastpath			
			auto lastDot = path.rfind('.');

			if (lastDot == std::string::npos)
			{
				return ResourceType::None;
			}

			const std::string extension = path.substr(lastDot);
			
			if (extension == ".obj"		|| 
				extension == ".fbx"		||
				extension == ".blend"	||
				extension == ".3ds"		||
				extension == ".glb"		||
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

			if (extension == ".skeleton")
			{
				return ResourceType::Skeleton;
			}

			if (extension == ".animator")
			{
				return ResourceType::Animator;
			}

			if (extension == ".animation")
			{
				return ResourceType::Animation;
			}

			if (extension == ".meta")
			{
				return ResourceType::Meta;
			}

			return ResourceType::None;
		}

		void ResourceManager::LoadMetaFile(const std::string &path)
		{
			// Extracting name from lastpath			
			auto lastDot = path.rfind('.');
			if (lastDot == std::string::npos)
			{
				return;
			}

			const std::string fileWithoutMeta = path.substr(0, lastDot);
			ResourceType resourceType = GetResourceTypeFromPath(fileWithoutMeta);

			switch (resourceType)
			{
			case BaldLion::ResourceManagement::ResourceType::None:
				break;
			case BaldLion::ResourceManagement::ResourceType::Texture:
				break;
			case BaldLion::ResourceManagement::ResourceType::Model:
				break;
			case BaldLion::ResourceManagement::ResourceType::Mesh:
				break;
			case BaldLion::ResourceManagement::ResourceType::Skeleton:
				break;
			case BaldLion::ResourceManagement::ResourceType::Material:
				break;
			case BaldLion::ResourceManagement::ResourceType::Shader:
				break;
			case BaldLion::ResourceManagement::ResourceType::Animator:
				AnimationManager::DeserializeAnimator(path.c_str());
				break;
			case BaldLion::ResourceManagement::ResourceType::Animation:
				break;
			case BaldLion::ResourceManagement::ResourceType::Meta:
				break;
			default:
				break;
			}
		}

		const char* ResourceManager::GetResourceSuffixFromType(ResourceType type)
		{
			switch (type)
			{
			case BaldLion::ResourceManagement::ResourceType::None:
				break;

			case BaldLion::ResourceManagement::ResourceType::Texture:
				break;

			case BaldLion::ResourceManagement::ResourceType::Model:
				break;

			case BaldLion::ResourceManagement::ResourceType::Mesh:
				return ".mesh";
				break;

			case BaldLion::ResourceManagement::ResourceType::Skeleton:
				return ".skeleton";
				break;

			case BaldLion::ResourceManagement::ResourceType::Material:
				return ".mat";
				break;

			case BaldLion::ResourceManagement::ResourceType::Shader:
				break;

			case BaldLion::ResourceManagement::ResourceType::Animator:
				return ".animator";
				break;

			case BaldLion::ResourceManagement::ResourceType::Animation:
				return ".animation";
				break;

			case BaldLion::ResourceManagement::ResourceType::Meta:
				return ".meta";
				break;

			default:
				break;
			}

			return "";
		}

		void ResourceManager::LoadAssets()
		{
			DynamicArray<std::string> metaPaths(AllocationType::Linear_Frame, 96);
			for (const auto & entry : fs::recursive_directory_iterator(ASSETS_PATH))
			{		
				ResourceType entryType = GetResourceTypeFromPath(entry.path().string());
				if (entryType != ResourceType::None)
				{
					if (entryType == ResourceType::Meta)
					{
						metaPaths.EmplaceBack(entry.path().string());
					}
					else 
					{
						AddResource<Resource>(entry.path().string(), entryType);
					}
				}
			}

			LoadPrimitiveShapeMeshes();

			BL_DYNAMICARRAY_FOREACH(metaPaths)
			{
				LoadMetaFile(metaPaths[i]);
			}

			BL_DYNAMICARRAY_FOREACH(s_externalResourcesPaths)
			{
				const std::string externalResourcePath = BL_STRINGID_TO_STRING(s_externalResourcesPaths[i]);
				ResourceType entryType = GetResourceTypeFromPath(externalResourcePath);
				if (entryType != ResourceType::None)
				{					
					AddResource<Resource>(externalResourcePath, entryType);					
				}
			}
		
		}

		bool ResourceManager::HasMetafile(const Resource* resource)
		{
			std::string metafilePath = BL_STRINGID_TO_STRING(resource->GetResourcePath()) + GetResourceSuffixFromType(ResourceType::Meta);

			if (FILE *file = fopen(metafilePath.c_str(), "r")) {
				fclose(file);
				return true;
			}
			return false;
		}

		void ResourceManager::LoadPrimitiveShapeMeshes()
		{
			Material::MaterialProperties materialProperties
			{
				BL_STRING_TO_STRINGID("assets/editorAssets/shaders/BaseLit.glsl"),
				glm::vec3(0.5f),
				glm::vec3(0.5f),
				glm::vec3(0.5f),
				glm::vec3(0.5f),
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



			const std::string matPath = "assets/editorAssets/materials/primitive.mat";
			Material* meshMaterial = Material::Create(matPath, materialProperties);
			ResourceManagement::ResourceManager::AddResource(meshMaterial);
			meshMaterial->AssignShader();

			const std::string cubeMeshPath = "assets/editorAssets/meshes/cubeMesh.mesh";
			auto resourcePath = std::filesystem::path(cubeMeshPath);

			CubeMesh* cubeMesh = MemoryManager::New<CubeMesh>("CubeMesh", AllocationType::FreeList_Renderer, meshMaterial, 1.0f, resourcePath.make_preferred().string());
			ResourceManagement::ResourceManager::AddResource(cubeMesh);
			cubeMesh->SetUpCube();

			const std::string planeMeshPath = "assets/editorAssets/meshes/planeMesh.mesh";
			resourcePath = std::filesystem::path(planeMeshPath);

			PlaneMesh* planeMesh = MemoryManager::New<PlaneMesh>("CubeMesh", AllocationType::FreeList_Renderer, meshMaterial, 1.0f, resourcePath.make_preferred().string());
			ResourceManagement::ResourceManager::AddResource(planeMesh);
			planeMesh->SetUpPlane();

			const std::string sphereMeshPath = "assets/editorAssets/meshes/sphereMesh.mesh";
			resourcePath = std::filesystem::path(sphereMeshPath);
			SphereMesh* sphereMesh = MemoryManager::New<SphereMesh>("CubeMesh", AllocationType::FreeList_Renderer, meshMaterial, 1.0f, resourcePath.make_preferred().string());
			ResourceManagement::ResourceManager::AddResource(sphereMesh);
			sphereMesh->SetUpSphere();

		}

		void ResourceManager::SerializeExternalResourcesPaths()
		{
			YAML::Emitter out;
			out << YAML::BeginMap;

			out << YAML::Key << YAML_KEY_EXTERNAL_RESOURCES_LIST << YAML::Value << YAML::BeginSeq;

			BL_DYNAMICARRAY_FOREACH(s_externalResourcesPaths)
			{
				out << YAML::BeginMap;
				out << YAML::Key << YAML_KEY_EXTERNAL_RESOURCE_PATH << YAML::Value << (ui32)s_externalResourcesPaths[i];
				out << YAML::EndMap;
			}

			out << YAML::EndSeq;

			out << YAML::EndMap;

			std::ofstream fout(EXTERNAL_RESOURCES_METADATA_PATH);
			fout << out.c_str();
			fout.close();
		}

		void ResourceManager::DeserializeExternalResourcesPath()
		{
			std::ifstream stream(EXTERNAL_RESOURCES_METADATA_PATH);
			std::stringstream strStream;

			strStream << stream.rdbuf();

			YAML::Node data = YAML::Load(strStream.str());

			if (!data[YAML_KEY_EXTERNAL_RESOURCES_LIST])
				return;

			auto externalResourcesList = data[YAML_KEY_EXTERNAL_RESOURCES_LIST];
			for (auto entry : externalResourcesList)
			{
				s_externalResourcesPaths.EmplaceBack(entry[YAML_KEY_EXTERNAL_RESOURCE_PATH].as<ui32>());
			}

		}

	}
}