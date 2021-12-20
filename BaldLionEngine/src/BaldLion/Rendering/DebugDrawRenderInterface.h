#pragma once

#include <glm/glm.hpp>

#define DEBUG_DRAW_IMPLEMENTATION
#include "debug_draw.hpp"     


namespace BaldLion
{
	namespace Rendering
	{
		class DebugDrawRenderInterface : public dd::RenderInterface
		{
		public:
			virtual ~DebugDrawRenderInterface() {}

			virtual void Init() = 0;
			virtual void Stop() = 0;

			void SetMvpMatrix(const glm::mat4& mvpMatrix)
			{
				m_mvpMatrix = mvpMatrix;
			}

			void SetScreenDimensions(float width, float height)
			{
				m_windowWidth = width;
				m_windowHeight = height;
			}

		protected:

			glm::mat4 m_mvpMatrix;
			float m_windowHeight;
			float m_windowWidth;
		};
	}
}
