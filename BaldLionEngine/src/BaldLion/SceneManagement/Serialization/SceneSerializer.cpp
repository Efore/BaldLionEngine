#include "blpch.h"
#include "SceneSerializer.h"
#include <fstream>

#include "BaldLion/ECS/ECSComponentsInclude.h"
#include "BaldLion/SceneManagement/SceneManager.h"

using namespace BaldLion::ECS;

namespace BaldLion
{
	namespace SceneManagement 
	{		
		void SceneSerializer::SerializeScene(const Scene* scene, const char* filepath)
		{
			YAML::Emitter out;
			out << YAML::BeginMap;
			out << YAML::Key << "Scene" << YAML::Value << STRINGID_TO_STR_C(scene->GetSceneID());
			out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

			for (ui32 i = 0; i < scene->GetECSManager()->GetEntities().Size(); ++i) 
			{
				SerializeEntity(scene, out, &scene->GetECSManager()->GetEntities()[i]);
			}

			out << YAML::EndSeq;
			out << YAML::EndMap;

			std::ofstream fout(filepath);
			fout << out.c_str();
		}

		bool SceneSerializer::DeserializeScene(Scene* scene, const char* filepath)
		{
			std::ifstream stream(filepath);
			std::stringstream strStream;

			strStream << stream.rdbuf();

			YAML::Node data = YAML::Load(strStream.str());

			if (!data["Scene"])
				return false;

			std::string sceneName = data["Scene"].as<std::string>();

			SceneManager::AddScene(sceneName.c_str(), true);

			auto entities = data["Entities"];

			if (entities)
			{
				for (auto entity : entities)
				{
					DeserializeEntity(entity);
				}
			}

			return true;
		}

		void SceneSerializer::SerializeEntity(const Scene* scene, YAML::Emitter &out, const ECS::ECSEntity* entity)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Entity" << YAML::Value << STRINGID_TO_STR_C(entity->GetEntityName());
			out << YAML::Key << "Components" << YAML::Value << YAML::BeginSeq;

			const ECS::ECSComponentLookUp* entityComponentLookUp = &scene->GetECSManager()->GetEntityComponents().Get(entity->GetEntityID());
			for (ui32 i = 0; i < (ui32)ECS::ECSComponentType::Count; ++i) 
			{
				SerializeComponent(out, (*entityComponentLookUp)[i]);
			}
			out << YAML::EndSeq;
			out << YAML::EndMap;			
		}

		void SceneSerializer::DeserializeEntity(const YAML::detail::iterator_value& yamlEntity)
		{
			std::string entityName = yamlEntity["Entity"].as<std::string>();

			ECS::ECSEntityID entityID = SceneManager::GetECSManager()->AddEntity(entityName.c_str());

			auto components = yamlEntity["components"];

			if (components)
			{
				for (auto component : components)
				{
					DeserializeComponent(entityID, component);
				}
			}
		}

		void SceneSerializer::SerializeComponent(YAML::Emitter &out, const ECS::ECSComponent* component)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Component" << YAML::Value << (ui32)component->GetComponentType();

			switch (component->GetComponentType())
			{

			default:
				break;

			case ECS::ECSComponentType::Transform:
			{
				ECSTransformComponent* transformComponent = (ECSTransformComponent*)component;

				SerializeVec3(out, "Position", transformComponent->position);
				SerializeVec3(out, "Rotation", transformComponent->rotation);
				SerializeVec3(out, "Scale", transformComponent->scale);
			}
				break;

			case ECS::ECSComponentType::ProjectionCamera:
			{
				ECSProjectionCameraComponent* cameraComponent = (ECSProjectionCameraComponent*)component;

				out << YAML::Key << "Fov" << YAML::Value << cameraComponent->fov;

				out << YAML::Key << "Width" << YAML::Value << cameraComponent->width;
				out << YAML::Key << "Height" << YAML::Value << cameraComponent->height;

				out << YAML::Key << "NearPlane" << YAML::Value << cameraComponent->nearPlane;
				out << YAML::Key << "FarPlane" << YAML::Value << cameraComponent->farPlane;
			}
				break;

			case ECS::ECSComponentType::Mesh:
				break;

			case ECS::ECSComponentType::Skeleton:
				break;

			case ECS::ECSComponentType::DirectionalLight:
			{
				ECSDirectionalLightComponent* directionalLightComponent = (ECSDirectionalLightComponent*)component;

				SerializeVec3(out, "AmbientColor", directionalLightComponent->ambientColor);
				SerializeVec3(out, "DiffuseColor", directionalLightComponent->diffuseColor);
				SerializeVec3(out, "SpecularColor", directionalLightComponent->specularColor);
				SerializeVec3(out, "Direction", directionalLightComponent->direction);
			}
				break;

			case ECS::ECSComponentType::PointLight:
				break;
			case ECS::ECSComponentType::Animation:
				break;
			case ECS::ECSComponentType::Hierarchy:
			{
				ECSHierarchyComponent* hierarchyComponent = (ECSHierarchyComponent*)component;

				out << YAML::Key << "ParentEntityID" << YAML::Value << hierarchyComponent->parentEntityID;
				out << YAML::Key << "ChildEntitiesIDs" << YAML::Value << YAML::BeginSeq;

				for (ui32 i = 0; i < hierarchyComponent->childEntitiesIDs.Size(); ++i)
				{
					out << YAML::Key << "ChildEntity" + std::to_string(i) << YAML::Value << hierarchyComponent->childEntitiesIDs[i];
				}

				out << YAML::EndSeq;
			}
				break;
			}

			out << YAML::EndMap;
		}

		void SceneSerializer::DeserializeComponent(ECS::ECSEntityID entityID, const YAML::detail::iterator_value& yamlComponent)
		{
			ECS::ECSComponentType componentType = (ECS::ECSComponentType)yamlComponent["Component"].as<ui32>();

			ECSComponent* component = nullptr;

			switch (componentType)
			{

			case BaldLion::ECS::ECSComponentType::Transform:
			{
				glm::vec3 position = DeserializeVec3(yamlComponent, "Position");
				glm::vec3 rotation = DeserializeVec3(yamlComponent, "Rotation");
				glm::vec3 scale = DeserializeVec3(yamlComponent, "Scale");

				component = SceneManager::GetECSManager()->AddComponent<ECSTransformComponent>(
					ECSComponentType::Transform,
					position,
					rotation,
					scale);
			}
				break;

			case BaldLion::ECS::ECSComponentType::ProjectionCamera:			
			{
				float fov = yamlComponent["Fov"].as<float>();

				float width = yamlComponent["Width"].as<float>();
				float height = yamlComponent["Height"].as<float>();

				float nearPlane = yamlComponent["NearPlane"].as<float>();
				float farPlane = yamlComponent["FarPlane"].as<float>();

				component = SceneManager::GetECSManager()->AddComponent<ECSProjectionCameraComponent>(
					ECSComponentType::ProjectionCamera,
					fov,
					width,
					height,
					nearPlane,
					farPlane);
			}
				break;

			case BaldLion::ECS::ECSComponentType::Mesh:
				break;

			case BaldLion::ECS::ECSComponentType::Skeleton:
				break;

			case BaldLion::ECS::ECSComponentType::DirectionalLight:
			{
				glm::vec3 ambientColor = DeserializeVec3(yamlComponent, "AmbientColor");
				glm::vec3 diffuseColor = DeserializeVec3(yamlComponent, "DiffuseColor");
				glm::vec3 specularColor = DeserializeVec3(yamlComponent, "SpecularColor");
				glm::vec3 direction = DeserializeVec3(yamlComponent, "Direction");

				component = SceneManager::GetECSManager()->AddComponent<ECS::ECSDirectionalLightComponent>(
					ECS::ECSComponentType::DirectionalLight,
					ambientColor,
					diffuseColor,
					specularColor,
					direction);
			}
				break;

			case BaldLion::ECS::ECSComponentType::PointLight:
				break;

			case BaldLion::ECS::ECSComponentType::Animation:
				break;

			case BaldLion::ECS::ECSComponentType::Hierarchy:
			{
				ECSEntityID parentEntityID = yamlComponent["ParentEntityID"].as<ECSEntityID>();

				auto children = yamlComponent["ChildEntitiesIDs"];

				DynamicArray<ECSEntityID> childEntitiesIDs(AllocationType::Linear_Frame, 10);
				ui32 i = 0;

				for (auto child : children)
				{
					childEntitiesIDs.EmplaceBack(child["ChildEntity" + std::to_string(i)].as<ECSEntityID>());
					++i;
				}

				component = SceneManager::GetECSManager()->AddComponent<ECSHierarchyComponent>(
					ECS::ECSComponentType::Hierarchy,
					parentEntityID,
					childEntitiesIDs);
			}
				break;

			default:
				break;
			}

			SceneManager::GetECSManager()->AddComponentToEntity(entityID, component);
		}

		void SceneSerializer::SerializeVec3(YAML::Emitter &out, const std::string& key, const glm::vec3& vec)
		{
			out << YAML::Key << (key + "X") << YAML::Value << vec.x;
			out << YAML::Key << (key + "Y") << YAML::Value << vec.y;
			out << YAML::Key << (key + "Z") << YAML::Value << vec.z;
		}

		glm::vec3 SceneSerializer::DeserializeVec3(const YAML::Node& node, const std::string& key)
		{
			float x = node[key + "X"].as<float>();
			float y = node[key + "Y"].as<float>();
			float z = node[key + "Z"].as<float>();

			return glm::vec3(x, y, z);
		}

	}
}