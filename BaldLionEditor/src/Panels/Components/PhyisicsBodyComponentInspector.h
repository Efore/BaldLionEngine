#pragma once

#include "ComponentInspector.h"
#include "BaldLion/ECS/Components/ECSPhysicsBodyComponent.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/Utils/MathUtils.h"
#include "BaldLion/Rendering/Renderer.h"


namespace BaldLion
{
	namespace Editor {

		class PhyisicsBodyComponentInspector : public ComponentInspector {

		public:

			static void OnImGuiRender(ECS::ECSComponent* component, SceneHierarchyPanel* sceneHierarchyPanel) {

				ComponentInspector::BeginComponentRender("Physics Body Component", component->GetComponentType(), sceneHierarchyPanel, (GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f) * 7.0f);

				ECS::ECSPhysicsBodyComponent* componentPhysicsBody = (ECS::ECSPhysicsBodyComponent*)component;
				
				const ECS::ECSTransformComponent* transformComponent = SceneManager::GetECSManager()->GetEntityComponents().Get(sceneHierarchyPanel->GetSelectedEntityID()).Read< ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);

				if (!Physics::PhysicsManager::GetIsPhysicsActive())
				{
					reactphysics3d::Transform physicsTransform(Physics::FromGlmVec3(transformComponent->position), Physics::FromGlmQuat(glm::quat(transformComponent->rotation)));
					componentPhysicsBody->rigidBody->setTransform(physicsTransform);

					float rigidBodyMass = componentPhysicsBody->rigidBody->getMass();
					BL_IMGUI_LEFT_LABEL(ImGui::InputFloat, "Rigidbody Mass", &rigidBodyMass);
					componentPhysicsBody->rigidBody->setMass(rigidBodyMass);

					int selectIndex = (int)componentPhysicsBody->bodyType;
					BL_IMGUI_LEFT_LABEL(ImGui::Combo, "Body Type", &selectIndex, "Dynamic\0Static\0Kinematic");
					componentPhysicsBody->bodyType = (Physics::PhysicsBodyType)selectIndex;

					switch (componentPhysicsBody->bodyType)
					{
					default:
						break;
					case Physics::PhysicsBodyType::Dynamic:
						componentPhysicsBody->rigidBody->setType(reactphysics3d::BodyType::DYNAMIC);
						break;
					case Physics::PhysicsBodyType::Static:
						componentPhysicsBody->rigidBody->setType(reactphysics3d::BodyType::STATIC);
						break;
					case Physics::PhysicsBodyType::Kinematic:
						componentPhysicsBody->rigidBody->setType(reactphysics3d::BodyType::KINEMATIC);
						break;
					}
				}
				else {
					ImGui::Text("Physics System is active. Please, stop it to show the component inspector");
				}


				float radius = 0.0f;
				switch (componentPhysicsBody->shape)
				{

				case Physics::PhysicsShape::Box:

					glm::vec3 boxSize = Physics::ToGlmVec3(((reactphysics3d::BoxShape*)componentPhysicsBody->collider->getCollisionShape())->getHalfExtents()) * 2.0f;

					if (!Physics::PhysicsManager::GetIsPhysicsActive())
					{
						EditorUtils::DrawVec3Handler("Box Size", componentPhysicsBody->unscaledColliderSize, 1.0f, 110.0f);
						boxSize = componentPhysicsBody->unscaledColliderSize;
						boxSize.x *= transformComponent->scale.x;
						boxSize.y *= transformComponent->scale.y;
						boxSize.z *= transformComponent->scale.z;

						((reactphysics3d::BoxShape*)componentPhysicsBody->collider->getCollisionShape())->setHalfExtents(Physics::FromGlmVec3(boxSize * 0.5f));
					}

					Rendering::Renderer::DrawDebugBox(transformComponent->position, boxSize, transformComponent->GetTransformMatrix(), MathUtils::Vector3UnitY);

					break;

				case Physics::PhysicsShape::Sphere:

					radius = ((reactphysics3d::SphereShape*)componentPhysicsBody->collider->getCollisionShape())->getRadius();

					if (!Physics::PhysicsManager::GetIsPhysicsActive())
					{
						BL_IMGUI_LEFT_LABEL(ImGui::InputFloat, "Sphere Radius", &componentPhysicsBody->unscaledColliderSize.x);
						radius = componentPhysicsBody->unscaledColliderSize.x * transformComponent->scale.x;
						((reactphysics3d::SphereShape*)componentPhysicsBody->collider->getCollisionShape())->setRadius(radius);
					}

					Rendering::Renderer::DrawDebugSphere(transformComponent->position, radius, MathUtils::Vector3UnitY);

					break;

				case Physics::PhysicsShape::Capsule:

					radius = ((reactphysics3d::CapsuleShape*)componentPhysicsBody->collider->getCollisionShape())->getRadius();
					float height = ((reactphysics3d::CapsuleShape*)componentPhysicsBody->collider->getCollisionShape())->getHeight();

					if (!Physics::PhysicsManager::GetIsPhysicsActive())
					{
						BL_IMGUI_LEFT_LABEL(ImGui::InputFloat, "Capsule Radius", &componentPhysicsBody->unscaledColliderSize.x);
						BL_IMGUI_LEFT_LABEL(ImGui::InputFloat, "Capsule Height", &componentPhysicsBody->unscaledColliderSize.y);

						radius = componentPhysicsBody->unscaledColliderSize.x * transformComponent->scale.x;
						height = componentPhysicsBody->unscaledColliderSize.y * transformComponent->scale.y;

						((reactphysics3d::CapsuleShape*)componentPhysicsBody->collider->getCollisionShape())->setRadius(radius);
						((reactphysics3d::CapsuleShape*)componentPhysicsBody->collider->getCollisionShape())->setHeight(height);						
					}

					Rendering::Renderer::DrawDebugCapsule(transformComponent->position, transformComponent->GetTransformMatrix(), radius, height, MathUtils::Vector3UnitY);

					break;
				}


				ComponentInspector::EndComponentRender();

			}

		};

	}
}


