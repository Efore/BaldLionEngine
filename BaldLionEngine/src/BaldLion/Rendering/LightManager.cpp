#include "blpch.h"
#include "LightManager.h"

namespace BaldLion
{
	namespace Rendering
	{
		DirectionalLight LightManager::s_directionalLight;
		std::vector <PointLight> LightManager::s_scenePointLights;
		bool LightManager::s_initialized = false;

		void LightManager::Init()
		{
			if (!s_initialized)
			{
				s_initialized = true;
			}
		}

		void LightManager::BeginScene(const DirectionalLight& directionalLight, const std::vector<PointLight>& pointLights)
		{
			s_directionalLight = directionalLight;
			s_scenePointLights = pointLights;
		}

		void LightManager::EndScene()
		{

		}
	}
}