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

			static void AddScene(const char* sceneID, bool setAsMainScene);
			static void AddScene(const char* sceneID, ECS::ECSManager* ecsManager, bool setAsMainScene);

			static void RemoveScene(StringId sceneID);

		private:
			static HashMap<StringId,Scene> m_scenes;
			static Scene* m_mainScene;
		};

	}
}
