#include "blpch.h"
#include "SceneSerializer.h"

#include "BaldLion/ECS/ECSManager.h"
#include "BaldLion/ECS/ECSComponentsInclude.h"
#include "BaldLion/ECS/SingletonSystems/LightningSystem.h"
#include "BaldLion/SceneManagement/SceneManager.h"
#include "BaldLion/ResourceManagement/ResourceManager.h"
#include "BaldLion/Rendering/Mesh.h"
#include "BaldLion/Animation/Skeleton.h"
#include "BaldLion/Animation/AnimationManager.h"
#include "BaldLion/AI/Navigation/NavigationManager.h"
#include "BaldLion/Utils/MathUtils.h"

using namespace BaldLion::ECS;

#define YAML_KEY_SCENENAME			"SceneName"
#define YAML_KEY_SCENEID			"SceneID"
#define YAML_KEY_ENTITIES			"Entities"
#define YAML_KEY_ENTITYNAME			"EntityName"
#define YAML_KEY_ENTITYID			"EntityID"
#define YAML_KEY_ENTITYISSTATIC		"EntityIsStatic"
#define YAML_KEY_COMPONENTS			"Components"
#define YAML_KEY_COMPONENTTYPE		"ComponentType"
#define YAML_KEY_ENTITYID_PROVIDER	"EntityIDProvider"

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
#define YAML_KEY_ANIMATOR_ID		"AnimatorID"

//Physics
#define YAML_KEY_PHYSICS_SHAPE			"PhyisicsShape"
#define YAML_KEY_PHYSICS_COLLIDER_SIZE	"PhyisicsColliderSize"
#define YAML_KEY_PHYSICS_BODY_TYPE		"PhyisicsBodyType"
#define YAML_KEY_PHYSICS_BODY_POSITION	"PhyisicsBodyPosition"
#define YAML_KEY_PHYSICS_BODY_ROTATION	"PhyisicsBodyRotation"
#define YAML_KEY_PHYSICS_MASS			"PhyisicsMass"

//Locomotion
#define YAML_KEY_LOCOMOTION_ROTSPEED	"LocomotionRotationSpeed"
#define YAML_KEY_LOCOMOTION_MAXMOVSPEED	"LocomotionMaxMovementSpeed"
#define YAML_KEY_LOCOMOTION_MAXACC		"LocomotionMaxAcceleration"

//NavMeshAgent
#define YAML_KEY_NAVMESHAGENT_POSITION		"NavMeshAgentPosition"
#define YAML_KEY_NAVMESHAGENT_MAX_SPEED		"NavMeshAgentMaxSpeed"
#define YAML_KEY_NAVMESHAGENT_MAX_ACC		"NavMeshAgentMaxAcc"

//Camera Follow
#define YAML_KEY_CAMERAFOLLOW_ENTITY_ID		"CameraFollowEntityID"
#define YAML_KEY_CAMERAFOLLOW_XYOFFSET		"CameraFollowXYOffset"
#define YAML_KEY_CAMERAFOLLOW_ZOFFSET		"CameraFollowZOffset"
#define YAML_KEY_CAMERAFOLLOW_ROTSPEED		"CameraFollowRotationSpeed"

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

			out << YAML::Key << YAML_KEY_ENTITYID_PROVIDER << YAML::Value << ECS::ECSManager::GetLatestEntityID();
			out << YAML::Key << YAML_KEY_SCENEID << YAML::Value << BL_STRINGID_TO_STR_C(scene->GetSceneID());
			out << YAML::Key << YAML_KEY_SCENENAME << YAML::Value << BL_STRINGID_TO_STR_C(scene->GetSceneName());
			out << YAML::Key << YAML_KEY_ENTITIES << YAML::Value << YAML::BeginSeq;

			BL_DYNAMICARRAY_FOR(i, scene->GetECSManager()->GetEntities(), 0)			
			{
				SerializeEntity(scene, out, &scene->GetECSManager()->GetEntities()[i]);
			}

			out << YAML::EndSeq;
			out << YAML::EndMap;

			std::ofstream fout(filepath);			
			fout << out.c_str();
			fout.close();
		}

		bool SceneSerializer::DeserializeScene(const char* filepath)
		{
			std::ifstream stream(filepath);
			std::stringstream strStream;

			strStream << stream.rdbuf();

			YAML::Node data = YAML::Load(strStream.str());

			if (!data[YAML_KEY_SCENEID])
				return false;

			std::string sceneID = data[YAML_KEY_SCENEID].as<std::string>();
			std::string sceneName = data[YAML_KEY_SCENENAME].as<std::string>();

			SceneManager::AddScene(sceneName.c_str());
			SceneManager::GetMainScene()->SetSceneName(sceneName.c_str());

			ECS::ECSManager::SetEntityIDProvider(data[YAML_KEY_ENTITYID_PROVIDER].as<ui32>());

			auto entities = data[YAML_KEY_ENTITIES];

			if (entities)
			{
				for (auto entity : entities)
				{
					DeserializeEntity(entity);
				}
			}

			SceneManager::GetECSManager()->GenerateCachedHierarchyRoot();

			return true;
		}

		void SceneSerializer::SerializeEntity(const Scene* scene, YAML::Emitter &out, const ECS::ECSEntity* entity)
		{
			out << YAML::BeginMap;
			out << YAML::Key << YAML_KEY_ENTITYNAME << YAML::Value << BL_STRINGID_TO_STR_C(entity->GetEntityName());
			out << YAML::Key << YAML_KEY_ENTITYID << YAML::Value << (entity->GetEntityID());
			out << YAML::Key << YAML_KEY_ENTITYISSTATIC << YAML::Value << entity->GetIsStatic();

			out << YAML::Key << YAML_KEY_COMPONENTS << YAML::Value << YAML::BeginSeq;

			ECS::ECSComponentLookUp entityComponentLookUp;
			if (scene->GetECSManager()->GetEntityComponents().TryGet(entity->GetEntityID(), entityComponentLookUp) )
			{
				for (ui32 i = 0; i < (ui32)ECS::ECSComponentType::Count; ++i)
				{
					if (entityComponentLookUp[i])
					{
						SerializeComponent(out, entityComponentLookUp[i]);
					}
				}
			}
			out << YAML::EndSeq;

			out << YAML::Key << YAML_KEY_PARENTID << YAML::Value << entity->GetParentID();
			out << YAML::Key << YAML_KEY_CHILDENTITIES << YAML::Value << YAML::BeginSeq;

			BL_DYNAMICARRAY_FOREACH(entity->GetChildrenIDs())
			{
				out << YAML::BeginMap;
				out << YAML::Key << YAML_KEY_CHILDENTITY + std::to_string(i) << YAML::Value << entity->GetChildrenIDs()[i];
				out << YAML::EndMap;
			}

			out << YAML::EndSeq;			

			out << YAML::EndMap;			
		}

		void SceneSerializer::DeserializeEntity(const YAML::detail::iterator_value& yamlEntity)
		{
			std::string entityName = yamlEntity[YAML_KEY_ENTITYNAME].as<std::string>();
			bool isStatic = yamlEntity[YAML_KEY_ENTITYISSTATIC].as<bool>();
			ECS::ECSEntityID entityID = yamlEntity[YAML_KEY_ENTITYID].as<ECS::ECSEntityID>();

			SceneManager::GetECSManager()->AddEntity(entityName.c_str(), entityID);

			auto components = yamlEntity[YAML_KEY_COMPONENTS];

			if (components)
			{
				for (auto component : components)
				{
					DeserializeComponent(entityID, component);
				}
			}

			ECS::ECSEntity* entity = SceneManager::GetECSManager()->GetEntityMap().Get(entityID);
			entity->SetIsStatic(isStatic);

			ECSEntityID parentEntityID = yamlEntity[YAML_KEY_PARENTID].as<ECSEntityID>();
			entity->SetParentID(parentEntityID);

			auto children = yamlEntity[YAML_KEY_CHILDENTITIES];

			ui32 i = 0;

			for (auto child : children)
			{
				entity->GetChildrenIDs().PushBack(child[YAML_KEY_CHILDENTITY + std::to_string(i)].as<ECSEntityID>());
				++i;
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

			case ECS::ECSComponentType::Animation:
			{
				ECSAnimationComponent* animationComponent = (ECSAnimationComponent*)component;
				out << YAML::Key << YAML_KEY_ANIMATOR_ID << YAML::Value << animationComponent->animatorID;
			}			
			break;

			case ECS::ECSComponentType::PhysicsBody:
			{
				ECSPhysicsBodyComponent* physicsBodyComponent = (ECSPhysicsBodyComponent*)component;

				out << YAML::Key << YAML_KEY_PHYSICS_SHAPE << YAML::Value << (ui32)physicsBodyComponent->shape;
				SerializeVec3(out, YAML_KEY_PHYSICS_COLLIDER_SIZE, physicsBodyComponent->unscaledColliderSize);				

				out << YAML::Key << YAML_KEY_PHYSICS_BODY_TYPE << YAML::Value << (ui32)physicsBodyComponent->bodyType;
				SerializeVec3(out, YAML_KEY_PHYSICS_BODY_POSITION, Physics::ToGlmVec3(physicsBodyComponent->rigidBody->getTransform().getPosition()));
				SerializeVec3(out, YAML_KEY_PHYSICS_BODY_ROTATION, glm::eulerAngles(Physics::ToGlmQuat(physicsBodyComponent->rigidBody->getTransform().getOrientation())));

				out << YAML::Key << YAML_KEY_PHYSICS_MASS << YAML::Value << (float)physicsBodyComponent->rigidBody->getMass();
			}
			break;

			case ECS::ECSComponentType::Locomotion:
			{
				ECSLocomotionComponent* locomotionComponent = (ECSLocomotionComponent*)component;
				out << YAML::Key << YAML_KEY_LOCOMOTION_ROTSPEED << YAML::Value << locomotionComponent->rotationSpeed;
				out << YAML::Key << YAML_KEY_LOCOMOTION_MAXMOVSPEED << YAML::Value << locomotionComponent->maxMovementSpeed;
				out << YAML::Key << YAML_KEY_LOCOMOTION_MAXACC << YAML::Value << locomotionComponent->maxAcceleration;
			}
			break;

			case ECS::ECSComponentType::NavMeshAgent:
			{
				ECSNavMeshAgentComponent* navMeshAgentComponent = (ECSNavMeshAgentComponent*)component;
				
				SerializeVec3(out, YAML_KEY_NAVMESHAGENT_POSITION, AI::Navigation::NavigationManager::GetCrowdAgentPosition(navMeshAgentComponent->crowdAgentIdx));				
				out << YAML::Key << YAML_KEY_NAVMESHAGENT_MAX_SPEED	<< YAML::Value << navMeshAgentComponent->agentMaxSpeed;
				out << YAML::Key << YAML_KEY_NAVMESHAGENT_MAX_ACC << YAML::Value << navMeshAgentComponent->agentMaxAcceleration;
			}
			break;

			case ECS::ECSComponentType::CameraFollow:
			{
				ECSCameraFollowComponent* cameraFollowComponent = (ECSCameraFollowComponent*)component;

				out << YAML::Key << YAML_KEY_CAMERAFOLLOW_ENTITY_ID << YAML::Value << cameraFollowComponent->followedEntityID;
				SerializeVec2(out, YAML_KEY_CAMERAFOLLOW_XYOFFSET, cameraFollowComponent->offsetXY);
				out << YAML::Key << YAML_KEY_CAMERAFOLLOW_ZOFFSET << YAML::Value << cameraFollowComponent->offsetZ;
				out << YAML::Key << YAML_KEY_CAMERAFOLLOW_ROTSPEED << YAML::Value << cameraFollowComponent->rotationSpeed;
			}
			break;
			case ECS::ECSComponentType::PlayerController:
			{
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

				component = SceneManager::GetECSManager()->CreateComponent<ECSTransformComponent>(
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

				component = SceneManager::GetECSManager()->CreateComponent<ECSProjectionCameraComponent>(
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

				Mesh* mesh = ResourceManagement::ResourceManager::LoadResource<Mesh>(BL_STRINGID_TO_STRING(meshResourceID));

				ECSMeshComponent* meshComponent = nullptr;
				ECSSkeletonComponent* skeletonComponent = nullptr;

				const ECSTransformComponent* transformComponent = SceneManager::GetECSManager()->GetEntityComponents().Get(entityID).Read<ECSTransformComponent>(ECSComponentType::Transform);
				mesh->GenerateMeshComponent(SceneManager::GetECSManager(), isStatic, transformComponent, meshComponent, skeletonComponent);
				component = meshComponent;
			}
				break;

			case BaldLion::ECS::ECSComponentType::Skeleton:
			{
				ui32 meshResourceID = yamlComponent[YAML_KEY_MESHRESOURCEID].as<ui32>();
				Mesh* mesh = ResourceManagement::ResourceManager::LoadResource<Mesh>(BL_STRINGID_TO_STRING(meshResourceID));
				component = mesh->GenerateSkeletonComponent(SceneManager::GetECSManager());
			}
				break;

			case BaldLion::ECS::ECSComponentType::DirectionalLight:
			{
				glm::vec3 ambientColor = DeserializeVec3(yamlComponent, YAML_KEY_AMBIENTCOLOR);
				glm::vec3 diffuseColor = DeserializeVec3(yamlComponent, YAML_KEY_DIFFUSECOLOR);
				glm::vec3 specularColor = DeserializeVec3(yamlComponent, YAML_KEY_SPECULARCOLOR);
				glm::vec3 direction = DeserializeVec3(yamlComponent, YAML_KEY_LIGHTDIRECTION);

				component = SceneManager::GetECSManager()->CreateComponent<ECS::ECSDirectionalLightComponent>(
					ECS::ECSComponentType::DirectionalLight,
					ambientColor,
					diffuseColor,
					specularColor,
					direction);

				ECS::SingletonSystems::LightningSystem::SetDirectionalLight((ECS::ECSDirectionalLightComponent*)component);
			}
				break;

			case BaldLion::ECS::ECSComponentType::Animation:
			{
				StringId animatorID = yamlComponent[YAML_KEY_ANIMATOR_ID].as<StringId>();
				StringId initAnimationID = Animation::AnimationManager::GetAnimator(animatorID)->GetInitialAnimationID();

				component = SceneManager::GetECSManager()->CreateComponent<ECS::ECSAnimationComponent>(ECS::ECSComponentType::Animation, animatorID, initAnimationID);
			}
				break;

			case ECS::ECSComponentType::PhysicsBody:
			{
				glm::vec3 position = DeserializeVec3(yamlComponent, YAML_KEY_PHYSICS_BODY_POSITION);
				glm::vec3 rotation = DeserializeVec3(yamlComponent, YAML_KEY_PHYSICS_BODY_ROTATION);

				Physics::PhysicsShape shape = (Physics::PhysicsShape)yamlComponent[YAML_KEY_PHYSICS_SHAPE].as<ui32>();
				Physics::PhysicsBodyType bodyType = (Physics::PhysicsBodyType)yamlComponent[YAML_KEY_PHYSICS_BODY_TYPE].as<ui32>();
				glm::vec3 colliderSize = DeserializeVec3(yamlComponent, YAML_KEY_PHYSICS_COLLIDER_SIZE);

				float mass = yamlComponent[YAML_KEY_PHYSICS_MASS].as<float>();

				component = SceneManager::GetECSManager()->CreateComponent<ECSPhysicsBodyComponent>(ECS::ECSComponentType::PhysicsBody, shape, bodyType, colliderSize, position, rotation, mass);
				
			}
			break;


			case BaldLion::ECS::ECSComponentType::Locomotion:
			{
				float rotSpeed = yamlComponent[YAML_KEY_LOCOMOTION_ROTSPEED].as<float>();
				float maxMovementSpeed = yamlComponent[YAML_KEY_LOCOMOTION_MAXMOVSPEED ].as<float>();
				float maxAcceleration = yamlComponent[YAML_KEY_LOCOMOTION_MAXACC].as<float>();

				component = SceneManager::GetECSManager()->CreateComponent<ECS::ECSLocomotionComponent>(
					ECS::ECSComponentType::Locomotion,
					rotSpeed, 
					maxMovementSpeed,
					maxAcceleration);
			}
			break;

			case BaldLion::ECS::ECSComponentType::NavMeshAgent:
			{
				float maxSpeed = yamlComponent[YAML_KEY_NAVMESHAGENT_MAX_SPEED].as<float>();
				float maxAcc = yamlComponent[YAML_KEY_NAVMESHAGENT_MAX_ACC].as<float>();
				glm::vec3 agentPos = DeserializeVec3(yamlComponent, YAML_KEY_NAVMESHAGENT_POSITION);

				component = SceneManager::GetECSManager()->CreateComponent<ECS::ECSNavMeshAgentComponent>(
					ECS::ECSComponentType::NavMeshAgent,
					agentPos,
					maxSpeed,
					maxAcc);
				break;
			}
			case BaldLion::ECS::ECSComponentType::CameraFollow:
			{
				ECS::ECSEntityID followedEntityID = yamlComponent[YAML_KEY_CAMERAFOLLOW_ENTITY_ID].as<ui32>();
				float rotSpeed = yamlComponent[YAML_KEY_CAMERAFOLLOW_ROTSPEED].as<float>();
				glm::vec2 offsetXY = DeserializeVec2(yamlComponent, YAML_KEY_CAMERAFOLLOW_XYOFFSET);
				float offsetZ = yamlComponent[YAML_KEY_CAMERAFOLLOW_ZOFFSET].as<float>();
				
				ECSTransformComponent* transform = SceneManagement::SceneManager::GetECSManager()->GetEntityComponents().Get(entityID).Write<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);
				
				component = SceneManager::GetECSManager()->CreateComponent<ECS::ECSCameraFollowComponent>(
					ECS::ECSComponentType::CameraFollow,
					followedEntityID,
					offsetXY,
					offsetZ,				
					rotSpeed);
				break;
			}
			case BaldLion::ECS::ECSComponentType::PlayerController:
			{
				component = SceneManager::GetECSManager()->CreateComponent<ECS::ECSPlayerControllerComponent>(
					ECS::ECSComponentType::PlayerController);
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

		void SceneSerializer::SerializeVec2(YAML::Emitter& out, const std::string& key, const glm::vec2& vec)
		{
			out << YAML::Key << (key + "X") << YAML::Value << vec.x;
			out << YAML::Key << (key + "Y") << YAML::Value << vec.y;			
		}

		glm::vec2 SceneSerializer::DeserializeVec2(const YAML::Node& node, const std::string& key)
		{
			float x = node[key + "X"].as<float>();
			float y = node[key + "Y"].as<float>();

			return glm::vec2(x, y);
		}

		void SceneSerializer::SerializeQuat(YAML::Emitter &out, const std::string& key, const glm::quat& quat)
		{
			out << YAML::Key << (key + "X") << YAML::Value << quat.x;
			out << YAML::Key << (key + "Y") << YAML::Value << quat.y;
			out << YAML::Key << (key + "Z") << YAML::Value << quat.z;
			out << YAML::Key << (key + "W") << YAML::Value << quat.w;
		}

		glm::quat SceneSerializer::DeserializeQuat(const YAML::Node& node, const std::string& key)
		{
			float x = node[key + "X"].as<float>();
			float y = node[key + "Y"].as<float>();
			float z = node[key + "Z"].as<float>();
			float w = node[key + "W"].as<float>();

			return glm::quat(w, x, y, z);
		}
	}
}