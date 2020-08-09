#pragma once
#include "LightClasses/PointLight.h"
#include "LightClasses/DirectionalLight.h"
#include <vector>

namespace BaldLion
{
	namespace Rendering
	{
		class LightManager
		{
		public:
			static void Init();
			static void BeginScene(const DirectionalLight& directionalLight, const std::vector<PointLight>& PointLights);
			static void EndScene();

			static inline const std::vector<PointLight>& GetScenePointLights() { return s_scenePointLights; }
			static inline const DirectionalLight& GetDirectionalLight() { return s_directionalLight; }

		private:
			static bool s_initialized;
			static std::vector<PointLight> s_scenePointLights;
			static DirectionalLight s_directionalLight;
		};
	}
}
