#pragma once
#include "LightClasses/PointLight.h"
#include "LightClasses/DirectionalLight.h"
#include "BaldLion/Core/Containers/BLVector.h"

namespace BaldLion
{
	namespace Rendering
	{
		class LightManager
		{
		public:
			static void Init();
			static void BeginScene(const DirectionalLight& directionalLight, const BLVector<PointLight>& PointLights);
			static void EndScene();

			static inline const BLVector<PointLight>& GetScenePointLights() { return s_scenePointLights; }
			static inline const DirectionalLight& GetDirectionalLight() { return s_directionalLight; }

		private:
			static bool s_initialized;
			static BLVector<PointLight> s_scenePointLights;
			static DirectionalLight s_directionalLight;
		};
	}
}
