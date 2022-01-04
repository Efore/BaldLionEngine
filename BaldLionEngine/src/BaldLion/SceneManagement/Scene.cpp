#include "blpch.h"
#include "Scene.h"
#include "BaldLion/ECS/ECSSystemsInclude.h"

namespace BaldLion {

	namespace SceneManagement {

		Scene::Scene(const char* sceneName, ECS::ECSManager* ecsManager) : m_sceneName(STRING_TO_STRINGID(sceneName)), m_sceneID(STRING_TO_STRINGID(sceneName)), m_ecsManager(ecsManager)
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
					const ECS::ECSSignature renderSystemSignature = ECS::GenerateSignature(2, ECS::ECSComponentType::Mesh, ECS::ECSComponentType::Transform);

					ECS::ECSRenderSystem* renderSystem = MemoryManager::New<ECS::ECSRenderSystem>("ECS RenderSystem", AllocationType::FreeList_ECS,
						"ECS RenderSystem", renderSystemSignature, m_ecsManager);

					const ECS::ECSSignature animationSystemSignature = ECS::GenerateSignature(2, ECS::ECSComponentType::Animation, ECS::ECSComponentType::Skeleton);

					ECS::ECSAnimationSystem* animationSystem = MemoryManager::New<ECS::ECSAnimationSystem>("ECS AnimationSystem", AllocationType::FreeList_ECS, "ECS AnimationSystem", animationSystemSignature, m_ecsManager);

					m_ecsManager->AddSystem(animationSystem);
					m_ecsManager->AddSystem(renderSystem);
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
