#pragma once
#include "EditorViewportPanel.h"
#include "EditorPanel.h"

struct dtMeshTile;

namespace BaldLion 
{
	namespace Editor 
	{
		class NavigationPanel : public EditorPanel {

		public:
			NavigationPanel();
			~NavigationPanel();

			void SetNavigationPanel(EditorViewportPanel* editorViewPortPanel);
			virtual void OnImGuiRender() override;

		private:

			void DrawAgents();
			void DrawNavMesh();
			void DrawMeshTile(const dtMeshTile* tile);
			void DrawPolygonBoundaries(const dtMeshTile* tile, bool innerBoundaries);
			void DrawVertices(const dtMeshTile* tile);

			void HandleInput();
			void CreateAgent(const glm::vec3& pos);
			void SetAgentsTarget(const glm::vec3& pos);
			void SetCrowdTargetPosition(const glm::vec3& pos);

			float DistancePtLine2d(const float* pt, const float* p, const float* q);

			inline unsigned int nextPow2(unsigned int v)
			{
				v--;
				v |= v >> 1;
				v |= v >> 2;
				v |= v >> 4;
				v |= v >> 8;
				v |= v >> 16;
				v++;
				return v;
			}

			inline unsigned int ilog2(unsigned int v)
			{
				unsigned int r;
				unsigned int shift;
				r = (v > 0xffff) << 4; v >>= r;
				shift = (v > 0xff) << 3; v >>= shift; r |= shift;
				shift = (v > 0xf) << 2; v >>= shift; r |= shift;
				shift = (v > 0x3) << 1; v >>= shift; r |= shift;
				r |= (v >> 1);
				return r;
			}

		private:


			ui32 m_agentCount;
			glm::vec3 m_currentTarget;

			EditorViewportPanel* m_editorViewportPanel;
		};		
	}
}
