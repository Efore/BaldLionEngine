#pragma once
#include "ComponentInspector.h"
#include "BaldLion/ECS/Components/ECSDirectionalLightComponent.h"
#include "BaldLion/ECS/SingletonSystems/LightningSystem.h"
#include "BaldLion/ECS/SingletonSystems/CameraSystem.h"
#include "ImGuizmo.h"
#include "EditorUtils.h"
#include "BaldLion/Utils/MathUtils.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

namespace BaldLion
{
	namespace Editor {

		class DirectionalLightComponentInspector : public ComponentInspector {

		public:

			static void OnImGuiRender(ECS::ECSComponent* component, SceneHierarchyPanel* sceneHierarchyPanel) {

				ECS::ECSDirectionalLightComponent* directionalLight = (ECS::ECSDirectionalLightComponent*)component;

				ComponentInspector::BeginComponentRender("Directional Light Component", component->GetComponentType(), sceneHierarchyPanel, (GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f) * 7.0f);

				static float shadowDistance = ECS::SingletonSystems::LightningSystem::GetShadowDistance();
				BL_IMGUI_LEFT_LABEL(ImGui::DragFloat, "Shadow Distance", &shadowDistance);
				ECS::SingletonSystems::LightningSystem::SetShadowDistance(shadowDistance);

				static float shadowTransitionDistance = ECS::SingletonSystems::LightningSystem::GetShadowTransitionDistance();
				BL_IMGUI_LEFT_LABEL(ImGui::DragFloat, "Shadow Transition Distance", &shadowTransitionDistance);
				ECS::SingletonSystems::LightningSystem::SetShadowTransitionDistance(shadowTransitionDistance);

				BL_IMGUI_LEFT_LABEL(ImGui::ColorEdit3,"Ambient color", (float*)&directionalLight->ambientColor);				
				BL_IMGUI_LEFT_LABEL(ImGui::ColorEdit3,"Diffuse color", (float*)&directionalLight->diffuseColor);
				BL_IMGUI_LEFT_LABEL(ImGui::ColorEdit3, "Specular color", (float*)&directionalLight->specularColor);				

				ComponentInspector::EndComponentRender();				
			}

		};

	}
}

