#pragma once
#include "ComponentInspector.h"
#include "BaldLion/ECS/Components/ECSDirectionalLightComponent.h"

namespace BaldLion
{
	namespace Editor {

		class DirectionalLightComponentInspector : public ComponentInspector {

		public:

			static void OnImGuiRender(ECS::ECSComponent* component, SceneHierarchyPanel* sceneHierarchyPanel) {

				ECS::ECSDirectionalLightComponent* directionalLight = (ECS::ECSDirectionalLightComponent*)component;

				ComponentInspector::BeginComponentRender("Directional Light Component", component->GetComponentType(), sceneHierarchyPanel, (GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f) * 7.0f);

				IMGUI_LEFT_LABEL(ImGui::ColorEdit3,"Ambient color", (float*)&directionalLight->ambientColor);				
				IMGUI_LEFT_LABEL(ImGui::ColorEdit3,"Diffuse color", (float*)&directionalLight->diffuseColor);
				IMGUI_LEFT_LABEL(ImGui::ColorEdit3,"Specular color", (float*)&directionalLight->specularColor);

				EditorUtils::DrawVec3Handler("Direction", directionalLight->direction, 1.0f);

				ComponentInspector::EndComponentRender();
			}

		};

	}
}

