#include <BaldLion.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "NavigationPanel.h"
#include "EditorUtils.h"
#include "BaldLion/SceneManagement/SceneManager.h"
#include "BaldLion/Core/StringId.h"
#include "BaldLion/Rendering/Renderer.h"
#include "BaldLion/AI/Navigation/NavigationManager.h"
#include "BaldLion/AI/Navigation/NavMeshBuilder.h"


namespace BaldLion 
{
	using namespace BaldLion::AI::Navigation;

	namespace Editor 
	{
		NavigationPanel::NavigationPanel() : EditorPanel("Navigation"),m_agentCount(0), m_currentTarget(0.0f), m_manageAgents(false)
		{
			m_tileIndexToDraw = DynamicArray<ui32>(Memory::AllocationType::FreeList_Main, 10);
		}

		NavigationPanel::~NavigationPanel()
		{
			m_tileIndexToDraw.Delete();
		}

		void NavigationPanel::SetNavigationPanel(EditorViewportPanel* editorViewPortPanel)
		{
			m_editorViewportPanel = editorViewPortPanel;
		}

		void NavigationPanel::OnImGuiRender(float deltaTime)
		{
			ImGui::Begin(BL_STRINGID_TO_STR_C(m_panelName));

			if (ImGui::Button("Load Geometry"))
			{
				NavMeshBuilder::LoadGeom();
			}

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

				if (geom->getMesh() != nullptr)
				{
					char text2[64];
					snprintf(text2, 64, "Verts: %.1fk  Tris: %.1fk",
						geom->getMesh()->getVertCount() / 1000.0f,
						geom->getMesh()->getTriCount() / 1000.0f);
					ImGui::Text(text2);
				}
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
			if (ImGui::Checkbox("Layers", &partitionIsLayers) && partitionIsLayers)
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

			ImGui::Text("Tiles");

			ImGui::SliderFloat("Tile Size", &(NavMeshBuilder::navMeshConfig.tileSize), 16.0f, 1024.0f, "%16.0f");
			
			ImGui::Separator();

			if (geom != nullptr)
			{
				char text[64];
				int gw = 0, gh = 0;
				const float* bmin = geom->getNavMeshBoundsMin();
				const float* bmax = geom->getNavMeshBoundsMax();
				rcCalcGridSize(bmin, bmax, NavMeshBuilder::navMeshConfig.cellSize, &gw, &gh);
				const int ts = (int)NavMeshBuilder::navMeshConfig.tileSize;
				const int tw = (gw + ts - 1) / ts;
				const int th = (gh + ts - 1) / ts;
				snprintf(text, 64, "Tiles  %d x %d", tw, th);
				ImGui::Text(text);

				// Max tiles and max polys affect how the tile IDs are caculated.
				// There are 22 bits available for identifying a tile and a polygon.
				int tileBits = rcMin((int)ilog2(nextPow2(tw*th)), 14);
				if (tileBits > 14) tileBits = 14;
				int polyBits = 22 - tileBits;
				NavMeshBuilder::navMeshConfig.maxTiles = 1 << tileBits;
				NavMeshBuilder::navMeshConfig.maxPolys = 1 << polyBits;
				snprintf(text, 64, "Max Tiles  %d", NavMeshBuilder::navMeshConfig.maxTiles);
				ImGui::Text(text);
				snprintf(text, 64, "Max Polys  %d", NavMeshBuilder::navMeshConfig.maxPolys);
				ImGui::Text(text);

				ImGui::Separator();
			}
			

			if (NavMeshBuilder::GeomMeshAdded())
			{
				if (ImGui::Button("Bake NavMesh"))
				{
					NavMeshBuilder::BuildNavMesh();
					NavigationManager::InitCrowd();
				}
			}

			if (NavMeshBuilder::NavMeshIsValid() && !NavMeshBuilder::NavMeshIsBaking())
			{
				DrawAgents();	
				
				ImGui::Checkbox("Manage Agents", &m_manageAgents);
			}

			ImGui::End();

			if (m_manageAgents)
			{
				HandleInput();
			}
		}

		void NavigationPanel::DrawAgents()
		{
			const dtNavMesh* mesh = NavMeshBuilder::GetNavMesh();			

			BL_HASHTABLE_FOR(SceneManager::GetECSManager()->GetEntityComponents(), it)
			{
				const ECS::ECSNavMeshAgentComponent* navMeshAgentComponent = it.GetValue().Read<ECS::ECSNavMeshAgentComponent>(ECS::ECSComponentType::NavMeshAgent);
				const ECS::ECSTransformComponent* transformComponent = it.GetValue().Read<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);

				if (navMeshAgentComponent != nullptr && transformComponent != nullptr)
				{
					static const int MAX_NEIS = 32;
					const dtMeshTile* neis[MAX_NEIS];
					ui32 tilesToDraw = 0;
					int tx, ty;
					mesh->calcTileLoc((float*)&transformComponent->position, &tx, &ty);

				

					for (ui32 i = 0; i < 9; ++i)
					{
						int nx = tx, ny = ty;
						switch (i)
						{
						case 0: nx++; break;
						case 1: nx++; ny++; break;
						case 2: ny++; break;
						case 3: nx--; ny++; break;
						case 4: nx--; break;
						case 5: nx--; ny--; break;
						case 6: ny--; break;
						case 7: nx++; ny--; break;
						case 8: break;
						};

						tilesToDraw = mesh->getTilesAt(nx, ny, neis, MAX_NEIS);

						for (ui32 j = 0; j < tilesToDraw; ++j)
						{
							DrawMeshTile(neis[j]);
						}
					}

					const dtCrowdAgent* agent = NavigationManager::GetCrowdAgent(navMeshAgentComponent->crowdAgentIdx);

					const glm::vec3 arrowFrom = transformComponent->position + BaldLion::MathUtils::Vector3UnitY;
					const glm::vec3 arrowToVel = arrowFrom + *(glm::vec3*)agent->nvel;
					Renderer::DrawDebugLine(arrowFrom, arrowToVel, EditorUtils::ColorBlue, 0.1f);

					const glm::vec3 arrowToDVel = arrowFrom + *(glm::vec3*)agent->dvel;
					Renderer::DrawDebugLine(arrowFrom, arrowToDVel, EditorUtils::ColorGreen, 0.1f);

					Renderer::DrawDebugCircle(transformComponent->position + BaldLion::MathUtils::Vector3UnitY * 0.1f, BaldLion::MathUtils::Vector3UnitY, NavMeshBuilder::navMeshConfig.agentRadius, 10, EditorUtils::ColorRed);
				}
			}	

		}

		void NavigationPanel::DrawNavMesh()
		{
			const dtNavMesh* mesh = NavMeshBuilder::GetNavMesh();

			BL_DYNAMICARRAY_FOREACH(m_tileIndexToDraw)
			{
				const dtMeshTile* tile = mesh->getTile(m_tileIndexToDraw[i]);
				if (!tile->header) continue;
				
				DrawMeshTile(tile);
			}
		}

		void NavigationPanel::DrawMeshTile(const dtMeshTile* tile)
		{
			const glm::vec3 polyColor(0.0f, 0.7f, 1.0f);
			for (int i = 0; i < tile->header->polyCount; ++i)
			{
				const dtPoly* p = &tile->polys[i];
				const dtPolyDetail* pd = &tile->detailMeshes[i];

				for (int j = 0; j < pd->triCount; ++j)
				{
					const unsigned char* t = &tile->detailTris[(pd->triBase + j) * 4];

					bool passingVertCount = t[0] < p->vertCount;
					const glm::vec3 vert0 = passingVertCount ? *(glm::vec3*)&tile->verts[p->verts[t[0]] * 3] : *(glm::vec3*)&tile->detailVerts[(pd->vertBase + t[0] - p->vertCount) * 3];

					passingVertCount = t[1] < p->vertCount;
					const glm::vec3 vert1 = passingVertCount ? *(glm::vec3*)&tile->verts[p->verts[t[1]] * 3] : *(glm::vec3*)&tile->detailVerts[(pd->vertBase + t[1] - p->vertCount) * 3];

					passingVertCount = t[2] < p->vertCount;
					const glm::vec3 vert2 = passingVertCount ? *(glm::vec3*)&tile->verts[p->verts[t[2]] * 3] : *(glm::vec3*)&tile->detailVerts[(pd->vertBase + t[2] - p->vertCount) * 3];

					Renderer::DrawDebugTriangle(vert0, vert1, vert2, polyColor, 0.5f);
				}
			}

			DrawPolygonBoundaries(tile, true);

			DrawPolygonBoundaries(tile, false);

			DrawVertices(tile);

			//Drawing tile separations
			{
				int gw = 0, gh = 0;
				const float* bmin = NavMeshBuilder::GetInputGeom()->getNavMeshBoundsMin();
				const float* bmax = NavMeshBuilder::GetInputGeom()->getNavMeshBoundsMax();

				rcCalcGridSize(bmin, bmax, NavMeshBuilder::navMeshConfig.cellSize, &gw, &gh);
				const int tw = (gw + (int)NavMeshBuilder::navMeshConfig.tileSize - 1) / (int)NavMeshBuilder::navMeshConfig.tileSize;
				const int th = (gh + (int)NavMeshBuilder::navMeshConfig.tileSize - 1) / (int)NavMeshBuilder::navMeshConfig.tileSize;
				const float s = NavMeshBuilder::navMeshConfig.tileSize * NavMeshBuilder::navMeshConfig.cellSize;

				for (int i = 0; i <= th; ++i)
				{
					const glm::vec3 init(bmin[0], bmin[1], bmin[2] + i * s);
					const glm::vec3 end(bmin[0] + tw * s, bmin[1], bmin[2] + i * s);
					Renderer::DrawDebugLine(init, end, glm::vec3(1.0f));
				}
				for (int i = 0; i <= tw; ++i)
				{
					const glm::vec3 init(bmin[0] + i * s, bmin[1], bmin[2]);
					const glm::vec3 end(bmin[0] + i * s, bmin[1], bmin[2] + th * s);
					Renderer::DrawDebugLine(init, end, glm::vec3(1.0f));
				}
			}
		}

		void NavigationPanel::DrawPolygonBoundaries(const dtMeshTile* tile, bool innerBoundaries)
		{
			static const float thr = 0.01f*0.01f;

			glm::vec3 lineColor = innerBoundaries ? glm::vec3(0.0f, 0.4f, 0.7f) : glm::vec3(0.0f, 0.5f, 1.0f);
			for (int i = 0; i < tile->header->polyCount; ++i)
			{
				const dtPoly* p = &tile->polys[i];

				if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION) continue;

				const dtPolyDetail* pd = &tile->detailMeshes[i];

				for (int j = 0, nj = (int)p->vertCount; j < nj; ++j)
				{	
					if (innerBoundaries)
					{
						if (p->neis[j] == 0) continue;
						if (p->neis[j] & DT_EXT_LINK)
						{
							bool con = false;
							for (unsigned int k = p->firstLink; k != DT_NULL_LINK; k = tile->links[k].next)
							{
								if (tile->links[k].edge == j)
								{
									con = true;
									break;
								}
							}
							if (con)
								lineColor = glm::vec3(1.0f);
							else
								lineColor = glm::vec3(0.0f);
						}
						else
							lineColor = glm::vec3(0.0f, 0.4f, 0.7f);
					}
					else
					{
						if (p->neis[j] != 0) continue;
					}

					const float* v0 = &tile->verts[p->verts[j] * 3];
					const float* v1 = &tile->verts[p->verts[(j + 1) % nj] * 3];

					// Draw detail mesh edges which align with the actual poly edge.
					// This is really slow.
					for (int k = 0; k < pd->triCount; ++k)
					{
						const unsigned char* t = &tile->detailTris[(pd->triBase + k) * 4];
						const float* tv[3];
						for (int m = 0; m < 3; ++m)
						{
							if (t[m] < p->vertCount)
								tv[m] = &tile->verts[p->verts[t[m]] * 3];
							else
								tv[m] = &tile->detailVerts[(pd->vertBase + (t[m] - p->vertCount)) * 3];
						}
						for (int m = 0, n = 2; m < 3; n = m++)
						{
							if (((t[3] >> (n * 2)) & 0x3) == 0) continue;	// Skip inner detail edges.
							if (DistancePtLine2d(tv[n], v0, v1) < thr &&
								DistancePtLine2d(tv[m], v0, v1) < thr)
							{
								Renderer::DrawDebugLine(*(glm::vec3*)v0, *(glm::vec3*)v1, lineColor);
							}
						}
					}
				}
			}
		}

		void NavigationPanel::DrawVertices(const dtMeshTile* tile)
		{
			for (int i = 0; i < tile->header->vertCount; ++i)
			{
				const float* v = &tile->verts[i * 3];
				Renderer::DrawDebugPoint(*(glm::vec3*)&tile->verts[i * 3], glm::vec3(0.0f));				
			}
		}

		void NavigationPanel::HandleInput()
		{
			if (!NavMeshBuilder::NavMeshIsValid() || !m_editorViewportPanel->GetViewportIsFocused())
			{
				return;
			}

			//if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			//{
			//	glm::vec2 mouseInWindow;
			//	if (EditorUtils::GetMouseRelativePosInWindow(m_editorViewportPanel->GetPanelID(), mouseInWindow))
			//	{
			//		const glm::vec3 rayOrigin = m_editorViewportPanel->GetViewportCameraTransform()->position;

			//		const glm::vec4 rayClip = glm::vec4(mouseInWindow.x, mouseInWindow.y, 1.0f, 1.0f);

			//		glm::vec3 rayDirection = glm::inverse(m_editorViewportPanel->GetViewportCamera()->viewProjectionMatrix) * rayClip;
			//		rayDirection = glm::normalize(rayDirection);

			//		const glm::vec3 rayEnd = rayOrigin + rayDirection * 5000.0f;

			//		float hitTime;
			//		bool hit = NavMeshBuilder::GetInputGeom()->raycastMesh((float*)&rayOrigin, (float*)&rayEnd, hitTime);

			//		if (hit)
			//		{
			//			const glm::vec3 hitPos = rayOrigin + rayDirection * 5000.0f * hitTime;
			//			CreateAgent(hitPos);
			//		}
			//	}
			//}
			
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				glm::vec2 mouseInWindow;
				if (EditorUtils::GetMouseRelativePosInWindow(m_editorViewportPanel->GetPanelID(), mouseInWindow))
				{
					const glm::vec3 rayOrigin = ECS::SingletonSystems::CameraSystem::GetMainCameraTransform()->position;

					const glm::vec4 rayClip = glm::vec4(mouseInWindow.x, mouseInWindow.y, 1.0f, 1.0f);

					glm::vec3 rayDirection = glm::inverse(ECS::SingletonSystems::CameraSystem::GetMainCamera()->viewProjectionMatrix) * rayClip;
					rayDirection = glm::normalize(rayDirection);

					const glm::vec3 rayEnd = rayOrigin + rayDirection * 5000.0f;

					float hitTime;
					bool hit = NavMeshBuilder::GetInputGeom()->raycastMesh((float*)&rayOrigin, (float*)&rayEnd, hitTime);

					if (hit)
					{
						const glm::vec3 hitPos = rayOrigin + rayDirection * 5000.0f * hitTime;
						SetAgentsTarget(hitPos);
					}
				}
			}
		}		

		void NavigationPanel::SetAgentsTarget(const glm::vec3& pos)
		{
			m_currentTarget = pos;
			SetCrowdTargetPosition(m_currentTarget);
		}

		void NavigationPanel::SetCrowdTargetPosition(const glm::vec3& pos)
		{
			BL_HASHTABLE_FOR(SceneManager::GetECSManager()->GetEntityComponents(), it)
			{
				const ECS::ECSNavMeshAgentComponent* navMeshAgentComponent = it.GetValue().Read<ECS::ECSNavMeshAgentComponent>(ECS::ECSComponentType::NavMeshAgent);

				if (navMeshAgentComponent != nullptr)
				{
					NavigationManager::RequestMoveTarget(navMeshAgentComponent->crowdAgentIdx, pos);
				}
			}
		}

		float NavigationPanel::DistancePtLine2d(const float* pt, const float* p, const float* q)
		{			
			float pqx = q[0] - p[0];
			float pqz = q[2] - p[2];
			float dx = pt[0] - p[0];
			float dz = pt[2] - p[2];
			float d = pqx * pqx + pqz * pqz;
			float t = pqx * dx + pqz * dz;
			if (d != 0) t /= d;
			dx = p[0] + t * pqx - pt[0];
			dz = p[2] + t * pqz - pt[2];
			return dx * dx + dz * dz;			
		}

	}
}