#pragma once
#include "blpch.h"
#include "ECSLightSingleton.h"

namespace BaldLion {

	namespace ECS {

		namespace SingletonComponents {

			ECSDirectionalLightComponent* ECSLightSingleton::s_directionalLight = nullptr;
		}
	}
}
