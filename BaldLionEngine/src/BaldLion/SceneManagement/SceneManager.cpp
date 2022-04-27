#include "blpch.h"
#include "SceneManager.h"
#include "Serialization/SceneSerializer.h"

namespace BaldLion
{
	namespace SceneManagement {
		
		HashMap<StringId, Scene*> SceneManager::m_activeScenes;
		Scene* SceneManager::m_mainScene;

		void SceneManager::Init()
		{
			m_activeScenes = HashMap<StringId, Scene*>(AllocationType::FreeList_ECS, 10);
		}

		void SceneManager::FrameStart()
		{
			if (m_mainScene != nullptr)
			{
				m_mainScene->FrameStart();
			}
		}

		void SceneManager::Update()
		{
			BL_PROFILE_FUNCTION();

			if (m_mainScene != nullptr)
			{
				m_mainScene->Update();
			}
		}

		void SceneManager::FrameEnd()
		{
			if (m_mainScene != nullptr)
			{
				m_mainScene->FrameEnd();
			}
		}

		void SceneManager::Stop()
		{
			m_activeScenes.Delete();
		}

		void SceneManager::SetMainScene(StringId sceneID)
		{
			Scene* result = nullptr;

			if (m_activeScenes.TryGet(sceneID, result))
			{
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

		void SceneManager::OpenScene(const char* filepath)
		{
			SceneSerializer::DeserializeScene(filepath);
		}

		void SceneManager::SaveScene(const char* filepath)
		{
			SceneSerializer::SerializeScene(m_mainScene, filepath);
		}

		void SceneManager::AddScene(const char* sceneID, bool openAdditive, bool setAsMainScene)
		{
			AddScene(sceneID, nullptr, openAdditive, setAsMainScene);
		}

		void SceneManager::AddScene(const char* sceneID, ECS::ECSManager* ecsManager, bool openAdditive, bool setAsMainScene)
		{
			StringId sceneid = BL_STRING_TO_STRINGID(sceneID);

			Scene* newScene = MemoryManager::New<Scene>(sceneID, AllocationType::FreeList_Main, sceneID, ecsManager);
			newScene->Init();

			if (openAdditive)
			{
				if (!m_activeScenes.Contains(sceneid))
				{
					m_activeScenes.Emplace(sceneid, newScene);
				}
				if (setAsMainScene)
				{
					m_mainScene = newScene;
				}
			}
			else
			{
				if (m_mainScene)
				{
					MemoryManager::Delete(m_mainScene);
				}
				m_mainScene = newScene;
			}
		}

		void SceneManager::RemoveActiveScene(StringId sceneID)
		{
			m_activeScenes.Remove(sceneID);
		}
	}
}
