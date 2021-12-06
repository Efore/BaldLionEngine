#include "blpch.h"
#include "Scene.h"

namespace BaldLion {

	namespace SceneManagement {

		Scene::Scene(const char* sceneName, ECS::ECSManager* ecsManager) : m_sceneID(STRING_TO_STRINGID(sceneName)), m_ecsManager(ecsManager)
		{
			if (ecsManager == nullptr)
			{
				m_ecsManager = Memory::MemoryManager::New<ECS::ECSManager>("ECS Manager", Memory::AllocationType::FreeList_ECS);
			}
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

		void Scene::FrameStart()
		{
			if (m_ecsManager != nullptr) {
				m_ecsManager->FrameStart();
			}
		}

		void Scene::Update(TimeStep timeStep)
		{
			if (m_ecsManager != nullptr) {
				m_ecsManager->UpdateSystems(timeStep);
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
