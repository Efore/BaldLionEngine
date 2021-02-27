#include "blpch.h"
#include "LightManager.h"

namespace BaldLion
{
	namespace Rendering
	{
		DirectionalLight LightManager::s_directionalLight;
		bool LightManager::s_initialized = false;

		void LightManager::Init()
		{
			if (!s_initialized)
			{
				s_initialized = true;
			}
		}

		void LightManager::BeginScene(const DirectionalLight& directionalLight)
		{
			s_directionalLight = directionalLight;
		}

		void LightManager::EndScene()
		{

		}
	}
}