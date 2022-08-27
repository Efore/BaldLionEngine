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
				m_ecsManager = Memory::MemoryManager::New<ECS::ECSManager>("ECS Manager", Memory::AllocationType::FreeList_ECS);

				{//Systems

					BL_GENERATE_SYSTEM("ECS AnimationSystem", ECS::ECSAnimationSystem, m_ecsManager, ECS::ECSComponentType::Animation, ECS::ECSComponentType::Skeleton);
					BL_GENERATE_SYSTEM("ECS PhysicsSystem", ECS::ECSPhysicsSystem, m_ecsManager, ECS::ECSComponentType::PhysicsBody, ECS::ECSComponentType::Transform);
					BL_GENERATE_SYSTEM("ECS RenderSystem", ECS::ECSRenderSystem, m_ecsManager, ECS::ECSComponentType::Mesh, ECS::ECSComponentType::Transform);
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

		void Scene::Update()
		{
			if (m_ecsManager != nullptr) {
				m_ecsManager->UpdateSystems();
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
