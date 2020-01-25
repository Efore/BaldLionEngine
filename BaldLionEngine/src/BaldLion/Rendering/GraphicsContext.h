#pragma once

namespace BaldLion
{
	namespace Rendering
	{
		class GraphicsContext
		{
		public:
			virtual void Init() = 0;
			virtual void SwapBuffers() = 0;
		};
	}
}
