#pragma once
#include "blpch.h"
#include "LightningSystem.h"

namespace BaldLion {

	namespace ECS {

		namespace SingletonSystems {

			ECSDirectionalLightComponent* LightningSystem::s_directionalLight = nullptr;
		}
	}
}
