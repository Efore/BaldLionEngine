#include <BaldLion.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "NavigationPanel.h"
#include "EditorUtils.h"
#include "BaldLion/AI/Navigation/NavigationMeshGenerator.h"
#include "BaldLion/SceneManagement/SceneManager.h"
#include "BaldLion/Core/StringId.h"
#include "BaldLion/Rendering/Renderer.h"
#include "BaldLion/ECS/ComponentsSingleton/ECSProjectionCameraSingleton.h"

using namespace BaldLion::AI::Navigation;

namespace BaldLion {

	namespace Editor 
	{
		NavigationPanel::NavigationPanel() : EditorPanel("Navigation")
		{
			NavigationMeshGenerator::Init();
		}

		NavigationPanel::~NavigationPanel()
		{
			NavigationMeshGenerator::Stop();
		}

		void NavigationPanel::OnImGuiRender()
		{
			ImGui::Begin(BL_STRINGID_TO_STR_C(m_panelName));

			if (ImGui::Button("Add all static meshes"))
			{
				NavigationMeshGenerator::AddAllStaticEntities();
			}

			ImGui::Separator();

			if (NavigationMeshGenerator::GetEntitiesToBake().Size() > 0)
			{
				ImGui::Columns(2, "Entities To Bake");
				BL_DYNAMICARRAY_INVERSE_FOREACH(NavigationMeshGenerator::GetEntitiesToBake())
				{
					const ECS::ECSEntityID entityID = NavigationMeshGenerator::GetEntitiesToBake()[i];

					std::string entityName = BL_STRINGID_TO_STRING(SceneManagement::SceneManager::GetECSManager()->GetEntityMap().Get(entityID)->GetEntityName());
					ImGui::Text(entityName.c_str());

					ImGui::NextColumn();

					entityName = "Remove" + entityName;
					if (ImGui::Button(entityName.c_str()))
					{
						NavigationMeshGenerator::RemoveEntityToBake(entityID);
					}

					ImGui::NextColumn();
				}		
				ImGui::EndColumns();

				ImGui::Separator();

				if (ImGui::Button("Bake navigation mesh"))
				{
					NavigationMeshGenerator::BakeNavMesh();
				}
			}
			
			if (NavigationMeshGenerator::IsNavMeshDataGenerated())
			{
				static bool showNavMesh = false;				
				static bool adjacences[10000];
				
				bool showingAdjacence = false;
				BL_IMGUI_LEFT_LABEL(ImGui::Checkbox, "Show NavMesh", &showNavMesh);

				for (ui32 i = 0; i < NavigationMeshGenerator::GetNavMeshData().polygons.Size(); ++i)
				{
					ImGui::Checkbox(std::to_string(i).c_str(), &adjacences[i]);					
				}

				i32 firstIndex = -1;
				i32 secondIndex = -1;
				
				for (ui32 i = 0; i < NavigationMeshGenerator::GetNavMeshData().polygons.Size(); ++i)
				{
					if (adjacences[i])
					{
						if (firstIndex < 0)
						{
							firstIndex = i;
						}
						else {
							secondIndex = i;
						}
					}
				}

				if (firstIndex >= 0 && secondIndex >= 0)
				{
					DynamicArray<glm::vec3> navMeshPath(Memory::AllocationType::Linear_Frame, NavigationMeshGenerator::GetNavMeshData().polygons.Size());
					if (NavigationMeshGenerator::FindPath(firstIndex, secondIndex, navMeshPath))
					{
						for (ui32 i = 0; i < navMeshPath.Size() - 1; ++i)
						{
							Renderer::DrawDebugLine(navMeshPath[i], navMeshPath[i + 1], glm::vec3(1.0f, 1.0f, 0.2f), false);
						}
					}
				}

				if (showNavMesh)
				{
					BL_DYNAMICARRAY_FOREACH(NavigationMeshGenerator::GetNavMeshData().polygons)
					{
						BL_DYNAMICARRAY_FOR(j, NavigationMeshGenerator::GetNavMeshData().polygons[i].vertexIndices, 0)
						{
							const ui32 nextIndex = (j + 1) % NavigationMeshGenerator::GetNavMeshData().polygons[i].vertexIndices.Size();

							const ui32 vertexIndex = NavigationMeshGenerator::GetNavMeshData().polygons[i].vertexIndices[j];
							const ui32 nextVertexIndex = NavigationMeshGenerator::GetNavMeshData().polygons[i].vertexIndices[nextIndex];

							const glm::vec3 startPosition = NavigationMeshGenerator::GetNavMeshData().polygonVertices[vertexIndex] + MathUtils::Vector3UnitY * 0.1f;
							const glm::vec3 endPosition = NavigationMeshGenerator::GetNavMeshData().polygonVertices[nextVertexIndex] + MathUtils::Vector3UnitY * 0.1f;

							Renderer::DrawDebugLine(startPosition, endPosition, glm::vec3(1.0f, 0.2f, 0.2f), false);
						}
					}
				}
				
			}

			ImGui::End();
		}
	}
}