#include "blpch.h"
#include "SceneManager.h"

namespace BaldLion
{
	namespace SceneManagement {
		
		HashMap<StringId, Scene> SceneManager::m_scenes;
		Scene* SceneManager::m_mainScene;

		void SceneManager::Init()
		{
			m_scenes = HashMap<StringId, Scene>(AllocationType::FreeList_ECS, 10);
		}

		void SceneManager::Update(TimeStep timeStep)
		{
			if (m_mainScene != nullptr)
			{
				m_mainScene->Update(timeStep);
			}
		}

		void SceneManager::EndOfFrame()
		{
			if (m_mainScene != nullptr)
			{
				m_mainScene->EndOfFrame();
			}
		}

		void SceneManager::Stop()
		{
			m_scenes.Delete();
		}

		void SceneManager::SetMainScene(StringId sceneID)
		{
			Scene* result = nullptr;

			if (m_scenes.TryGet(sceneID, *result)) {
				m_mainScene = result;
			}			
		}

		Scene* SceneManager::GetMainScene()
		{
			return m_mainScene;
		}

		ECS::ECSManager* SceneManager::GetECSManager()
		{
			return m_mainScene->GetECSManager();
		}

		void SceneManager::AddScene(const char* sceneID, bool setAsMainScene)
		{
			AddScene(sceneID, nullptr, setAsMainScene);
		}

		void SceneManager::AddScene(const char* sceneID, ECS::ECSManager* ecsManager, bool setAsMainScene)
		{
			StringId sceneid = STRING_TO_STRINGID(sceneID);

			if (!m_scenes.Contains(sceneid))
			{
				m_scenes.Emplace(sceneid, sceneID, ecsManager);

				if (setAsMainScene) {
					m_mainScene = &m_scenes.Get(sceneid);
				}
			}
		}

		void SceneManager::RemoveScene(StringId sceneID)
		{
			m_scenes.Remove(sceneID);
		}
	}
}
