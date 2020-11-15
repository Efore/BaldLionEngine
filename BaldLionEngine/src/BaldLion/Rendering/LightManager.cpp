#include "blpch.h"
#include "LightManager.h"

namespace BaldLion
{
	namespace Rendering
	{
		DirectionalLight LightManager::s_directionalLight;
		DynamicArray<PointLight> LightManager::s_scenePointLights;
		bool LightManager::s_initialized = false;

		void LightManager::Init()
		{
			if (!s_initialized)
			{
				s_initialized = true;
			}
		}

		void LightManager::BeginScene(const DirectionalLight& directionalLight, const DynamicArray<PointLight>& pointLights)
		{
			s_directionalLight = directionalLight;
			s_scenePointLights = std::move(pointLights);
		}

		void LightManager::EndScene()
		{

		}
	}
}