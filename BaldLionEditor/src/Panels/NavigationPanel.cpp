#include <BaldLion.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "NavigationPanel.h"
#include "EditorUtils.h"
#include "BaldLion/SceneManagement/SceneManager.h"
#include "BaldLion/Core/StringId.h"
#include "BaldLion/Rendering/Renderer.h"
#include "BaldLion/ECS/ComponentsSingleton/ECSProjectionCameraSingleton.h"
#include "BaldLion/AI/Navigation/NavMeshBuilder.h"

namespace BaldLion {

	using namespace BaldLion::AI::Navigation;

	namespace Editor 
	{
		NavigationPanel::NavigationPanel() : EditorPanel("Navigation")
		{
		}

		NavigationPanel::~NavigationPanel()
		{
		}

		void NavigationPanel::OnImGuiRender()
		{
			
			ImGui::Begin(BL_STRINGID_TO_STR_C(m_panelName));
			ImGui::SliderFloat("Cell Size", &(NavMeshBuilder::navMeshConfig.cellSize), 0.1f, 1.0f, "%.2f");
			ImGui::SliderFloat("Cell Height", &(NavMeshBuilder::navMeshConfig.cellHeight), 0.1f, 1.0f, "%.2f");

			const InputGeom* geom = NavMeshBuilder::GetInputGeom();
			if (geom !=  nullptr)
			{
				const float* bmin = geom->getNavMeshBoundsMin();
				const float* bmax = geom->getNavMeshBoundsMax();

				int gw = 0, gh = 0;
				gw = (int)((bmax[0] - bmin[0]) / NavMeshBuilder::navMeshConfig.cellSize + 0.5f);
				gh = (int)((bmax[2] - bmin[2]) / NavMeshBuilder::navMeshConfig.cellSize + 0.5f);

				char text[64];
				snprintf(text, 64, "Voxels  %d x %d", gw, gh);
				ImGui::Text(text);
			}

			ImGui::Separator();
			ImGui::Text("Agent");
			ImGui::SliderFloat("Height", &(NavMeshBuilder::navMeshConfig.agentHeight), 0.1f, 5.0f, "%.1f");
			ImGui::SliderFloat("Radius", &(NavMeshBuilder::navMeshConfig.agentRadius), 0.0f, 5.0f, "%.1f");
			ImGui::SliderFloat("Max Climb", &(NavMeshBuilder::navMeshConfig.agentMaxClimb), 0.1f, 5.0f, "%.1f");
			ImGui::SliderFloat("Max Slope", &(NavMeshBuilder::navMeshConfig.agentMaxSlope), 0.0f, 90.0f, "%.0f");

			ImGui::Separator();
			ImGui::Text("Region");
			ImGui::SliderFloat("Min Region Size", &(NavMeshBuilder::navMeshConfig.regionMinSize), 0.0f, 150.0f, "%.0f");
			ImGui::SliderFloat("Merged Region Size", &(NavMeshBuilder::navMeshConfig.regionMergeSize), 0.0f, 150.0f, "%.0f");

			ImGui::Separator();
			ImGui::Text("Partitioning");
			
			bool partitionWatershed = NavMeshBuilder::navMeshConfig.partitionType == SAMPLE_PARTITION_WATERSHED;			
			if(ImGui::Checkbox("Watershed", &partitionWatershed) && partitionWatershed)
			{
				NavMeshBuilder::navMeshConfig.partitionType = SAMPLE_PARTITION_WATERSHED;
			}			

			bool partitionIsMonotone = NavMeshBuilder::navMeshConfig.partitionType == SAMPLE_PARTITION_MONOTONE;
			if (ImGui::Checkbox("Monotone", &partitionIsMonotone) && partitionIsMonotone)
			{
				NavMeshBuilder::navMeshConfig.partitionType = SAMPLE_PARTITION_MONOTONE;
			}

			bool partitionIsLayers = NavMeshBuilder::navMeshConfig.partitionType == SAMPLE_PARTITION_LAYERS;
			if (ImGui::Checkbox("Monotone", &partitionIsLayers) && partitionIsLayers)
			{
				NavMeshBuilder::navMeshConfig.partitionType = SAMPLE_PARTITION_LAYERS;
			}

			ImGui::Separator();
			ImGui::Text("Polygonization");
			ImGui::SliderFloat("Max Edge Length", &(NavMeshBuilder::navMeshConfig.edgeMaxLen), 0.0f, 50.0f, "%.0f");
			ImGui::SliderFloat("Max Edge Error", &(NavMeshBuilder::navMeshConfig.edgeMaxError), 0.1f, 3.0f, "%.1f");
			ImGui::SliderFloat("Verts Per Poly", &(NavMeshBuilder::navMeshConfig.vertsPerPoly), 3.0f, 12.0f, "%.0f");

			ImGui::Separator();
			ImGui::Text("Detail Mesh");
			
			ImGui::SliderFloat("Sample Distance", &(NavMeshBuilder::navMeshConfig.detailSampleDist), 0.0f, 16.0f, "%.0f");
			ImGui::SliderFloat("Max Sample Error", &(NavMeshBuilder::navMeshConfig.detailSampleMaxError), 0.0f, 16.0f, "%.0f");

			ImGui::Separator();

			if (ImGui::Button("Bake Navigation Mesh")) 
			{
				ui32 meshResourceID = 0;
				BL_HASHTABLE_FOR(SceneManagement::SceneManager::GetECSManager()->GetEntityComponents(), it)
				{
					const ECS::ECSMeshComponent* meshComponent = it.GetValue().Read<ECS::ECSMeshComponent>(ECS::ECSComponentType::Mesh);

					if (meshComponent != nullptr && meshComponent->isStatic)
					{
						meshResourceID = meshComponent->meshResourceID;
						break;
					}
				}
				ResourceManagement::Resource* meshResource = nullptr;
				if (ResourceManagement::ResourceManager::GetResourceMap().TryGet(meshResourceID, meshResource))
				{
					NavMeshBuilder::BuildNavMesh(BL_STRINGID_TO_STR_C(meshResource->GetResourcePath()));
				}
				
			}

			ImGui::End();
		}
	}
}