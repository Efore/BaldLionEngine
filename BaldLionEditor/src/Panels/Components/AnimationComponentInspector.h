#pragma once
#include "ComponentInspector.h"
#include "BaldLion/ECS/Components/ECSAnimationComponent.h"

namespace BaldLion
{
	namespace Editor {

		class AnimationComponentInspector : public ComponentInspector {

		public:

			static void OnImGuiRender(ECS::ECSComponent* component, SceneHierarchyPanel* sceneHierarchyPanel) {

				ECS::ECSAnimationComponent* animationComponent = (ECS::ECSAnimationComponent*)component;

				ComponentInspector::BeginComponentRender("Animation Component", component->GetComponentType(), sceneHierarchyPanel, (GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f) * 7.0f);

				if (ImGui::Button("Play")) {
					animationComponent->timer.Resume();
				}

				ImGui::SameLine();

				if (ImGui::Button("Pause")) {
					animationComponent->timer.Pause();
				}

				ImGui::SameLine();

				static bool steppingForward = false;
				if (steppingForward)
				{
					steppingForward = false;
					animationComponent->timer.Pause();
				}

				if (ImGui::Button("Step Fwd")) {
					animationComponent->timer.Resume();
					steppingForward = true;
				}

				ImGui::SameLine();


				if (ImGui::Button("Stop")) {
					steppingForward = true;
					animationComponent->currentAnimationTime = 0.0f;
					animationComponent->currentTransitionTime = 0.0f;
				}

				Animator* animator = Animation::AnimationManager::GetAnimator(animationComponent->animatorID);
				const AnimationClip* currentAnimation = animator->GetAnimationClip(animationComponent->currentAnimationID);
				
				if (animationComponent->currentTransitionTime > 0.0f) {

					const AnimatorTransition* currentTransition = animator->CheckTransition(animationComponent->currentAnimationID, animationComponent->currentAnimationTime, animationComponent->animatorParameters);


					//ImGui::Text("Transition");
					float progress = animationComponent->currentTransitionTime / currentTransition->GetTransitionTime();

					ImGui::ProgressBar(progress, ImVec2(0.f, 0.f));

				}
				else {
					//ImGui::Text("Current animation: %s", BL_STRINGID_TO_STR_C(currentAnimation->GetResourceName()));

					int frame = (int)(glm::floor((animationComponent->currentAnimationTime / currentAnimation->AnimationTimeLength) * currentAnimation->NumFrames));
					float progress = animationComponent->currentAnimationTime / currentAnimation->AnimationTimeLength;

					char buf[32];
					sprintf_s(buf, "%d/%d", frame, currentAnimation->NumFrames);
					ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);
				}

				ImGui::Separator();

				ImGui::Text("Parameters"); 
				ImGui::Separator();

				ImGui::BeginChild("animator_parameters");
				ImGui::Columns(2);

				BL_HASHTABLE_FOR(animationComponent->animatorParameters, it)
				{
					ImGui::Text(BL_STRINGID_TO_STR_C(it.GetKey())); ImGui::NextColumn();
					ImGui::Separator();
					
					switch (it.GetValue().Type)
					{
					case AnimatorParameter::ValueType::Bool:
						ImGui::Checkbox("##", &it.GetValue().Value.boolean);
						break;

					case AnimatorParameter::ValueType::Int:
						{
						int value = it.GetValue().Value.integer;
						ImGui::InputInt("##", &value);
						it.GetValue().Value.integer = (ui32)value;
						}
						break;
					case AnimatorParameter::ValueType::Float:
						ImGui::InputFloat("##", &it.GetValue().Value.floating);
						break;
					default:
						break;
					}
					ImGui::NextColumn();
				}

				ImGui::EndChild();				
				
				ComponentInspector::EndComponentRender();
			}

		};

	}
}

