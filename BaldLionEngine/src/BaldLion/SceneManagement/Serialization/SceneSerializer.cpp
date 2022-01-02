#include "blpch.h"
#include "SceneSerializer.h"
#include <fstream>

#include "BaldLion/ECS/ECSComponentsInclude.h"
#include "BaldLion/SceneManagement/SceneManager.h"
#include "BaldLion/ResourceManagement/ResourceManager.h"
#include "BaldLion/Rendering/Mesh.h"
#include "BaldLion/Animation/Skeleton.h"
#include "BaldLion/Animation/AnimationManager.h"

using namespace BaldLion::ECS;

#define YAML_KEY_SCENE				"Scene"
#define YAML_KEY_ENTITIES			"Entities"
#define YAML_KEY_ENTITYNAME			"EntityName"
#define YAML_KEY_COMPONENTS			"Components"
#define YAML_KEY_COMPONENTTYPE		"ComponentType"

//Transform
#define YAML_KEY_POSITION			"Position"
#define YAML_KEY_ROTATION			"Rotation"
#define YAML_KEY_SCALE				"Scale"

//Projection camera
#define YAML_KEY_CAMERAFOV			"CameraFov"
#define YAML_KEY_CAMERAWIDTH		"CameraWidth"
#define YAML_KEY_CAMERAHEIGHT		"CameraHeight"
#define YAML_KEY_CAMERANEARPLANE	"CameraNearPlane"
#define YAML_KEY_CAMERAFARPLANE		"CameraFarPlane"

//Mesh
#define YAML_KEY_MESHRESOURCEID		"MeshResourceID"
#define YAML_KEY_MESHISSTATIC		"MeshIsStatic"

//Light
#define YAML_KEY_AMBIENTCOLOR		"AmbientColor"
#define YAML_KEY_DIFFUSECOLOR		"DiffuseColor"
#define YAML_KEY_SPECULARCOLOR		"SpecularColor"
#define YAML_KEY_LIGHTDIRECTION		"LightDirection"

//Animation
#define YAML_KEY_ANIMATORID			"AnimatorID"

//Hierarchy
#define YAML_KEY_PARENTID			"ParentEntityID"
#define YAML_KEY_CHILDENTITIES		"ChildEntities"
#define YAML_KEY_CHILDENTITY		"ChildEntity"




namespace BaldLion
{
	namespace SceneManagement 
	{		
		void SceneSerializer::SerializeScene(const Scene* scene, const char* filepath)
		{
			YAML::Emitter out;
			out << YAML::BeginMap;
			out << YAML::Key << YAML_KEY_SCENE << YAML::Value << STRINGID_TO_STR_C(scene->GetSceneID());
			out << YAML::Key << YAML_KEY_ENTITIES << YAML::Value << YAML::BeginSeq;

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

			if (!data[YAML_KEY_SCENE])
				return false;

			std::string sceneName = data[YAML_KEY_SCENE].as<std::string>();

			SceneManager::AddScene(sceneName.c_str(), true);

			auto entities = data[YAML_KEY_ENTITIES];

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
			out << YAML::Key << YAML_KEY_ENTITYNAME << YAML::Value << STRINGID_TO_STR_C(entity->GetEntityName());
			out << YAML::Key << YAML_KEY_COMPONENTS << YAML::Value << YAML::BeginSeq;

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
			std::string entityName = yamlEntity[YAML_KEY_ENTITYNAME].as<std::string>();

			ECS::ECSEntityID entityID = SceneManager::GetECSManager()->AddEntity(entityName.c_str());

			auto components = yamlEntity[YAML_KEY_COMPONENTS];

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
			out << YAML::Key << YAML_KEY_COMPONENTTYPE << YAML::Value << (ui32)component->GetComponentType();

			switch (component->GetComponentType())
			{

			default:
				break;

			case ECS::ECSComponentType::Transform:
			{
				ECSTransformComponent* transformComponent = (ECSTransformComponent*)component;

				SerializeVec3(out, YAML_KEY_POSITION, transformComponent->position);
				SerializeVec3(out, YAML_KEY_ROTATION, transformComponent->rotation);
				SerializeVec3(out, YAML_KEY_SCALE, transformComponent->scale);
			}
				break;

			case ECS::ECSComponentType::ProjectionCamera:
			{
				ECSProjectionCameraComponent* cameraComponent = (ECSProjectionCameraComponent*)component;

				out << YAML::Key << YAML_KEY_CAMERAFOV << YAML::Value << cameraComponent->fov;

				out << YAML::Key << YAML_KEY_CAMERAWIDTH << YAML::Value << cameraComponent->width;
				out << YAML::Key << YAML_KEY_CAMERAHEIGHT << YAML::Value << cameraComponent->height;

				out << YAML::Key << YAML_KEY_CAMERANEARPLANE << YAML::Value << cameraComponent->nearPlane;
				out << YAML::Key << YAML_KEY_CAMERAFARPLANE << YAML::Value << cameraComponent->farPlane;
			}
				break;

			case ECS::ECSComponentType::Mesh:
			{
				ECSMeshComponent* meshComponent = (ECSMeshComponent*)component;
				out << YAML::Key << YAML_KEY_MESHRESOURCEID << YAML::Value << meshComponent->meshResourceID;
				out << YAML::Key << YAML_KEY_MESHISSTATIC << YAML::Value << meshComponent->isStatic;
			}
				break;

			case ECS::ECSComponentType::Skeleton:
			{
				ECSSkeletonComponent* skeletonComponent = (ECSSkeletonComponent*)component;
				out << YAML::Key << YAML_KEY_MESHRESOURCEID << YAML::Value << skeletonComponent->skeletonResourceID;
			}
				break;

			case ECS::ECSComponentType::DirectionalLight:
			{
				ECSDirectionalLightComponent* directionalLightComponent = (ECSDirectionalLightComponent*)component;

				SerializeVec3(out, YAML_KEY_AMBIENTCOLOR, directionalLightComponent->ambientColor);
				SerializeVec3(out, YAML_KEY_DIFFUSECOLOR, directionalLightComponent->diffuseColor);
				SerializeVec3(out, YAML_KEY_SPECULARCOLOR, directionalLightComponent->specularColor);
				SerializeVec3(out, YAML_KEY_LIGHTDIRECTION, directionalLightComponent->direction);
			}
				break;

			case ECS::ECSComponentType::PointLight:
				break;

			case ECS::ECSComponentType::Animation:
			{
				ECSAnimationComponent* animationComponent = (ECSAnimationComponent*)component;
				out << YAML::Key << YAML_KEY_ANIMATORID << YAML::Value << animationComponent->animatorID;
			}
				break;
			case ECS::ECSComponentType::Hierarchy:
			{
				ECSHierarchyComponent* hierarchyComponent = (ECSHierarchyComponent*)component;

				out << YAML::Key << YAML_KEY_PARENTID << YAML::Value << hierarchyComponent->parentEntityID;
				out << YAML::Key << YAML_KEY_CHILDENTITIES << YAML::Value << YAML::BeginSeq;

				for (ui32 i = 0; i < hierarchyComponent->childEntitiesIDs.Size(); ++i)
				{
					out << YAML::Key << YAML_KEY_CHILDENTITY + std::to_string(i) << YAML::Value << hierarchyComponent->childEntitiesIDs[i];
				}

				out << YAML::EndSeq;
			}
				break;
			}

			out << YAML::EndMap;
		}

		void SceneSerializer::DeserializeComponent(ECS::ECSEntityID entityID, const YAML::detail::iterator_value& yamlComponent)
		{
			ECS::ECSComponentType componentType = (ECS::ECSComponentType)yamlComponent[YAML_KEY_COMPONENTTYPE].as<ui32>();

			ECSComponent* component = nullptr;

			switch (componentType)
			{

			case BaldLion::ECS::ECSComponentType::Transform:
			{
				glm::vec3 position = DeserializeVec3(yamlComponent, YAML_KEY_POSITION);
				glm::vec3 rotation = DeserializeVec3(yamlComponent, YAML_KEY_ROTATION);
				glm::vec3 scale = DeserializeVec3(yamlComponent, YAML_KEY_SCALE);

				component = SceneManager::GetECSManager()->AddComponent<ECSTransformComponent>(
					ECSComponentType::Transform,
					position,
					rotation,
					scale);
			}
				break;

			case BaldLion::ECS::ECSComponentType::ProjectionCamera:			
			{
				float fov = yamlComponent[YAML_KEY_CAMERAFOV].as<float>();

				float width = yamlComponent[YAML_KEY_CAMERAWIDTH].as<float>();
				float height = yamlComponent[YAML_KEY_CAMERAHEIGHT].as<float>();

				float nearPlane = yamlComponent[YAML_KEY_CAMERANEARPLANE].as<float>();
				float farPlane = yamlComponent[YAML_KEY_CAMERAFARPLANE].as<float>();

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
			{
				ui32 meshResourceID = yamlComponent[YAML_KEY_MESHRESOURCEID].as<ui32>();
				bool isStatic = yamlComponent[YAML_KEY_MESHISSTATIC].as<bool>();

				Mesh* mesh = ResourceManagement::ResourceManager::LoadResource<Mesh>(STRINGID_TO_STRING(meshResourceID));
				component = mesh->GenerateMeshComponent(SceneManager::GetECSManager(), isStatic);
			}
				break;

			case BaldLion::ECS::ECSComponentType::Skeleton:
			{
				ui32 meshResourceID = yamlComponent[YAML_KEY_MESHRESOURCEID].as<ui32>();
				Mesh* mesh = ResourceManagement::ResourceManager::LoadResource<Mesh>(STRINGID_TO_STRING(meshResourceID));
				component = mesh->GenerateSkeletonComponent(SceneManager::GetECSManager());
			}
				break;

			case BaldLion::ECS::ECSComponentType::DirectionalLight:
			{
				glm::vec3 ambientColor = DeserializeVec3(yamlComponent, YAML_KEY_AMBIENTCOLOR);
				glm::vec3 diffuseColor = DeserializeVec3(yamlComponent, YAML_KEY_DIFFUSECOLOR);
				glm::vec3 specularColor = DeserializeVec3(yamlComponent, YAML_KEY_SPECULARCOLOR);
				glm::vec3 direction = DeserializeVec3(yamlComponent, YAML_KEY_LIGHTDIRECTION);

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
			{
				StringId animatorID = yamlComponent[YAML_KEY_ANIMATORID].as<StringId>();

				StringId initAnimationID = Animation::AnimationManager::GetAnimator(animatorID)->GetInitialAnimationID();

				component = SceneManager::GetECSManager()->AddComponent<ECS::ECSAnimationComponent>(
					ECS::ECSComponentType::Animation, animatorID, initAnimationID);					
			}
				break;

			case BaldLion::ECS::ECSComponentType::Hierarchy:
			{
				ECSEntityID parentEntityID = yamlComponent[YAML_KEY_PARENTID].as<ECSEntityID>();

				auto children = yamlComponent[YAML_KEY_CHILDENTITIES];

				DynamicArray<ECSEntityID> childEntitiesIDs(AllocationType::Linear_Frame, 10);
				ui32 i = 0;

				for (auto child : children)
				{
					childEntitiesIDs.EmplaceBack(child[YAML_KEY_CHILDENTITY + std::to_string(i)].as<ECSEntityID>());
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