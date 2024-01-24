#pragma once
#include "BaldLion/ECS/Components/ECSDirectionalLightComponent.h"
#include "BaldLion/Core/Containers/DynamicArray.h"


namespace BaldLion {

	namespace ECS {

		namespace SingletonSystems {

			class LightningSystem {

			public:

				static const glm::vec3& GetDirectionaLightAmbientColor()	{	return s_directionalLight->ambientColor;	}
				static const glm::vec3& GetDirectionaLightDiffuseColor()	{	return s_directionalLight->diffuseColor;	}
				static const glm::vec3& GetDirectionaLightSpecularColor()	{	return s_directionalLight->specularColor;	}
				static const glm::vec3& GetDirectionaLightDirection()		{	return s_directionalLight->direction;		}

				static void SetDirectionaLightAmbientColor(const glm::vec3& ambientColor) { s_directionalLight->ambientColor = ambientColor; }
				static void SetDirectionaLightDiffuseColor(const glm::vec3& diffuseColor) { s_directionalLight->diffuseColor = diffuseColor; }
				static void SetDirectionaLightSpecularColor(const glm::vec3& specularColor) { s_directionalLight->specularColor = specularColor; }
				static void SetDirectionaLightDirection(const glm::vec3& direction) { s_directionalLight->direction = direction; }

				static void SetDirectionalLight(ECSDirectionalLightComponent* directionalLight) { s_directionalLight = directionalLight; }

			private:

				static ECSDirectionalLightComponent* s_directionalLight;
				
			};
		}		
	}

}
