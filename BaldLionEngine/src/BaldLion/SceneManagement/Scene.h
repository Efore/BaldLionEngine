#pragma once
#include "BaldLion/Core/StringId.h"
#include "BaldLion/ECS/ECSManager.h"

namespace BaldLion {

	namespace SceneManagement {

		class Scene
		{
		public:
			Scene();
			Scene(const char* sceneName, ECS::ECSManager* ecsManager);
			~Scene();

			void Init();
			void FrameStart();
			void Update();
			void FrameEnd();

			const StringId GetSceneID() const { return m_sceneID; }

			const StringId GetSceneName() const { return m_sceneName; }
			void SetSceneName(const char* sceneName) { m_sceneName = STRING_TO_STRINGID(sceneName); }

			const ECS::ECSManager* GetECSManager() const { return m_ecsManager; }
			ECS::ECSManager* GetECSManager() { return m_ecsManager; }

		private:

			StringId m_sceneName;
			StringId m_sceneID;			
			ECS::ECSManager* m_ecsManager;

		};
	}
}
