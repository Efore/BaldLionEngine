#pragma once
#include "EditorViewportPanel.h"
#include "EditorPanel.h"

struct dtMeshTile;

namespace BaldLion {	


	namespace Editor 
	{
		class NavigationPanel : public EditorPanel {

		public:
			NavigationPanel();
			~NavigationPanel();

			void SetEditorViewportPanel(EditorViewportPanel* editorViewportPanel) 
			{
				m_editorViewportPanel = editorViewportPanel;
			}

			virtual void OnImGuiRender() override;

		private:

			void DrawNavMesh();
			void DrawMeshTile(const dtMeshTile* tile);
			void DrawPolygonBoundaries(const dtMeshTile* tile, bool innerBoundaries);
			void DrawVertices(const dtMeshTile* tile);

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

			EditorViewportPanel* m_editorViewportPanel;

		};		
	}
}
