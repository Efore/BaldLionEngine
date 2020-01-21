#include "blpch.h"
#include "LightManager.h"

namespace BaldLion
{
	DirectionalLight LightManager::m_directionalLight;
	std::vector <PointLight> LightManager::m_scenePointLights;

	void LightManager::Init()
	{

	}

	void LightManager::BeginScene(const DirectionalLight& directionalLight, const std::vector<PointLight>& pointLights)
	{
		m_directionalLight = directionalLight;
		m_scenePointLights = pointLights;
	}

	void LightManager::EndScene()
	{

	}

}