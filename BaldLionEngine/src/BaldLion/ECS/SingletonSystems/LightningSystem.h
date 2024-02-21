#pragma once
#include "BaldLion/ECS/Components/ECSDirectionalLightComponent.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/Core/Containers/DynamicArray.h"


namespace BaldLion {

	namespace ECS {

		namespace SingletonSystems {

			class LightningSystem {

			public:

				static const glm::vec3& GetDirectionaLightAmbientColor()	{	return s_directionalLight->ambientColor;	}
				static const glm::vec3& GetDirectionaLightDiffuseColor()	{	return s_directionalLight->diffuseColor;	}
				static const glm::vec3& GetDirectionaLightSpecularColor()	{	return s_directionalLight->specularColor;	}			

				static const ECSTransformComponent* GetDirectionaLightTransform() { return s_directionalLightTransform; }
				
				static const float& GetShadowDistance() { return s_shadowDistance; }
				static const float& GetShadowTransitionDistance() { return s_shadowTransitionDistance; }

				static void SetDirectionaLightAmbientColor(const glm::vec3& ambientColor) { s_directionalLight->ambientColor = ambientColor; }
				static void SetDirectionaLightDiffuseColor(const glm::vec3& diffuseColor) { s_directionalLight->diffuseColor = diffuseColor; }
				static void SetDirectionaLightSpecularColor(const glm::vec3& specularColor) { s_directionalLight->specularColor = specularColor; }				

				static void SetDirectionalLight(ECSDirectionalLightComponent* directionalLight) { s_directionalLight = directionalLight; }
				static void SetDirectionalLightTransform(ECSTransformComponent* directionalLightTransform) { s_directionalLightTransform = directionalLightTransform; }
				static void SetShadowDistance(float distance) { s_shadowDistance = distance; }
				static void SetShadowTransitionDistance(float distance) { s_shadowTransitionDistance = distance; }


			private:

				static ECSTransformComponent* s_directionalLightTransform;
				static ECSDirectionalLightComponent* s_directionalLight;
				static float s_shadowDistance;
				static float s_shadowTransitionDistance;				
			};
		}		
	}

}
