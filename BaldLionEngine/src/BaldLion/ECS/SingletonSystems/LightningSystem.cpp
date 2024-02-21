#pragma once
#include "blpch.h"
#include "LightningSystem.h"

namespace BaldLion {

	namespace ECS {

		namespace SingletonSystems {

			ECSTransformComponent* LightningSystem::s_directionalLightTransform = nullptr;
			ECSDirectionalLightComponent* LightningSystem::s_directionalLight = nullptr;
			float LightningSystem::s_shadowDistance;
			float LightningSystem::s_shadowTransitionDistance;

		}
	}
}
