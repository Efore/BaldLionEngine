#include "blpch.h"
#include "Scene.h"
#include "BaldLion/ECS/ECSSystemsInclude.h"

namespace BaldLion {

	namespace SceneManagement {

		Scene::Scene(const char* sceneName, ECS::ECSManager* ecsManager) : m_sceneName(BL_STRING_TO_STRINGID(sceneName)), m_sceneID(BL_STRING_TO_STRINGID(sceneName)), m_ecsManager(ecsManager)
		{
			
		}

		Scene::Scene() : m_ecsManager(nullptr)
		{

		}

		Scene::~Scene()
		{
			if (m_ecsManager != nullptr) {
				MemoryManager::Delete(m_ecsManager);
			}
		}

		void Scene::Init()
		{
			if (m_ecsManager == nullptr)
			{
				ECS::ECSManager::SetComponentIDProvider(0);
				ECS::ECSManager::SetEntityIDProvider(0);

				m_ecsManager = Memory::MemoryManager::New<ECS::ECSManager>("ECS Manager", Memory::AllocationType::FreeList_ECS);

				{//Systems

					BL_GENERATE_SYSTEM("ECS PhysicsSystem", ECS::ECSPhysicsSystem, ECS::ECSSystemType::PhysicsSystem, m_ecsManager, ECS::ECSComponentType::PhysicsBody, ECS::ECSComponentType::Transform);
					BL_GENERATE_SYSTEM("ECS LocomotionSystem", ECS::ECSLocomotionSystem, ECS::ECSSystemType::LocomotionSystem, m_ecsManager, ECS::ECSComponentType::Transform, ECS::ECSComponentType::Locomotion);
					BL_GENERATE_SYSTEM("ECS CameraFollowSystem", ECS::ECSCameraFollowSystem, ECS::ECSSystemType::CameraFollowSystem, m_ecsManager, ECS::ECSComponentType::Transform, ECS::ECSComponentType::CameraFollow);
					BL_GENERATE_SYSTEM("ECS AnimationSystem", ECS::ECSAnimationSystem, ECS::ECSSystemType::AnimationSystem, m_ecsManager, ECS::ECSComponentType::Animation, ECS::ECSComponentType::Skeleton);
					BL_GENERATE_SYSTEM("ECS NavigationSystem", ECS::ECSNavigationSystem, ECS::ECSSystemType::NavigationSystem, m_ecsManager, ECS::ECSComponentType::Locomotion, ECS::ECSComponentType::NavMeshAgent, ECS::ECSComponentType::Transform);
					BL_GENERATE_SYSTEM("ECS PlayerControllerSystem", ECS::ECSPlayerControllerSystem, ECS::ECSSystemType::PlayerControllerSystem, m_ecsManager, ECS::ECSComponentType::PlayerController, ECS::ECSComponentType::Locomotion, ECS::ECSComponentType::Transform);
					BL_GENERATE_SYSTEM("ECS Frustrum Culling System", ECS::ECSFrustrumCullingSystem, ECS::ECSSystemType::FrustrumCullingSystem, m_ecsManager, ECS::ECSComponentType::Mesh, ECS::ECSComponentType::Transform);
					BL_GENERATE_SYSTEM("ECS HTN Planner System", ECS::ECSHTNPlannerSystem, ECS::ECSSystemType::HTNPlannerSystem, m_ecsManager, ECS::ECSComponentType::HTNAgent);
										
					BL_SET_SYSTEM_DEPENDENCIES(ECS::ECSSystemType::LocomotionSystem, m_ecsManager, ECS::ECSSystemType::PhysicsSystem, ECS::ECSSystemType::NavigationSystem);
					BL_SET_SYSTEM_DEPENDENCIES(ECS::ECSSystemType::CameraFollowSystem, m_ecsManager, ECS::ECSSystemType::LocomotionSystem);
					BL_SET_SYSTEM_DEPENDENCIES(ECS::ECSSystemType::AnimationSystem, m_ecsManager, ECS::ECSSystemType::LocomotionSystem);					
					BL_SET_SYSTEM_DEPENDENCIES(ECS::ECSSystemType::FrustrumCullingSystem, m_ecsManager, ECS::ECSSystemType::AnimationSystem);
				}

				m_ecsManager->StartSystems();
			}
		}

		void Scene::FrameStart()
		{
			if (m_ecsManager != nullptr) {
				m_ecsManager->FrameStart();
			}
		}

		void Scene::Update(float deltaTime)
		{
			if (m_ecsManager != nullptr) {
				m_ecsManager->UpdateSystems(deltaTime);
			}
		}

		void Scene::FrameEnd()
		{
			if (m_ecsManager != nullptr) {
				m_ecsManager->FrameEnd();
			}
		}

	}
}
