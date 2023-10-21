#include "blpch.h"
#include "SceneManager.h"
#include "Serialization/SceneSerializer.h"
#include "BaldLion/ECS/ComponentsSingleton/CameraSystem.h"
#include <filesystem>

namespace BaldLion
{
	namespace SceneManagement {

		#define LAST_SCENE_PATH "assets/scenes/lastScene.meta"

		HashMap<StringId, Scene*> SceneManager::s_activeScenes;
		Scene* SceneManager::s_mainScene;		

		std::string SceneManager::s_mainScenePathFile;

		void SceneManager::Init()
		{
			s_activeScenes = HashMap<StringId, Scene*>(AllocationType::FreeList_ECS, 10);

			std::ifstream lastSceneFile(LAST_SCENE_PATH);			
			if (lastSceneFile.is_open())
			{
				while (getline(lastSceneFile, s_mainScenePathFile));
				lastSceneFile.close();
			}

			ECS::SingletonComponents::CameraSystem::Init();
		}

		void SceneManager::FrameStart()
		{
			if (s_mainScene != nullptr)
			{
				s_mainScene->FrameStart();
			}
		}

		void SceneManager::Update(float deltaTime)
		{
			BL_PROFILE_FUNCTION();

			if (s_mainScene != nullptr)
			{
				s_mainScene->Update(deltaTime);
			}
		}

		void SceneManager::FrameEnd()
		{
			if (s_mainScene != nullptr)
			{
				s_mainScene->FrameEnd();
			}
		}

		void SceneManager::Stop()
		{
			s_activeScenes.Delete();

			ECS::SingletonComponents::CameraSystem::Stop();
		}

		void SceneManager::SetMainScene(StringId sceneID)
		{
			Scene* result = nullptr;

			if (s_activeScenes.TryGet(sceneID, result))
			{
				s_mainScene = result;
			}			
		}

		Scene* SceneManager::GetMainScene()
		{
			return s_mainScene;
		}

		ECS::ECSManager* SceneManager::GetECSManager()
		{
			return s_mainScene->GetECSManager();
		}

		void SceneManager::OpenScene(const char* filepath)
		{
			SceneSerializer::DeserializeScene(filepath);
			s_mainScenePathFile = filepath;
		}

		void SceneManager::SaveScene(const char* filepath)
		{
			SceneSerializer::SerializeScene(s_mainScene, filepath);
			s_mainScenePathFile = filepath;

			std::ofstream lastSceneFile(LAST_SCENE_PATH);
			if (lastSceneFile.is_open())
			{
				lastSceneFile << filepath;
				lastSceneFile.close();
			}
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
				if (!s_activeScenes.Contains(sceneid))
				{
					s_activeScenes.Emplace(sceneid, newScene);
				}
				if (setAsMainScene)
				{
					s_mainScene = newScene;
				}
			}
			else
			{
				if (s_mainScene)
				{
					MemoryManager::Delete(s_mainScene);
				}
				s_mainScene = newScene;
				ECS::ECSManager::SetComponentIDProvider(0);				
			}
		}

		void SceneManager::RemoveActiveScene(StringId sceneID)
		{
			s_activeScenes.Remove(sceneID);
		}
	}
}
