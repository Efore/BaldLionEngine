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

			void FrameStart();
			void Update();
			void FrameEnd();

			const StringId GetSceneID() const { return m_sceneID; }

			const ECS::ECSManager* GetECSManager() const { return m_ecsManager; }
			ECS::ECSManager* GetECSManager() { return m_ecsManager; }

		private:

			StringId m_sceneID;			
			ECS::ECSManager* m_ecsManager;

		};
	}
}
