#pragma once
#include "LightClasses/Light.h"
#include "BaldLion/Core/Containers/DynamicArray.h"

namespace BaldLion
{
	namespace Rendering
	{
		class LightManager
		{
		public:
			static void Init();
			static void BeginScene(const DirectionalLight& directionalLight);
			static void EndScene();

			static inline const DirectionalLight& GetDirectionalLight() { return s_directionalLight; }

		private:
			static bool s_initialized;			
			static DirectionalLight s_directionalLight;
		};
	}
}
