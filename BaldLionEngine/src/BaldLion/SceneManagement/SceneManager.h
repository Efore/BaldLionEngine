#pragma once
#include "BaldLion/Core/Containers/HashMap.h"
#include "Scene.h"

namespace BaldLion
{
	namespace SceneManagement {

		class SceneManager {

		public:

			static void Init();
			static void FrameStart();
			static void Update();
			static void FrameEnd();
			static void Stop();

			static void SetMainScene(StringId sceneID);
			static Scene* GetMainScene();
			static ECS::ECSManager* GetECSManager();

			static void OpenScene(const char* filepath);
			static void SaveScene(const char* filepath);

			static void AddScene(const char* sceneID, bool openAdditive = false, bool setAsMainScene = false);
			static void AddScene(const char* sceneID, ECS::ECSManager* ecsManager, bool openAdditive = false, bool setAsMainScene = false);

			static void RemoveActiveScene(StringId sceneID);

		private:
			static HashMap<StringId,Scene*> m_activeScenes;
			static Scene* m_mainScene;

		};

	}
}
