#pragma once
#include "LightClasses/PointLight.h"
#include "LightClasses/DirectionalLight.h"
#include <vector>

namespace BaldLion
{
	class LightManager
	{
	public:
		static void Init();
		static void BeginScene(const DirectionalLight& directionalLight, const std::vector<PointLight>& PointLights);
		static void EndScene();

		static inline const std::vector<PointLight>& GetScenePointLights() { return m_scenePointLights; }
		static inline const DirectionalLight& GetDirectionalLight() { return m_directionalLight; }

	private:
		
		static std::vector<PointLight> m_scenePointLights;
		static DirectionalLight m_directionalLight;
	};
}
