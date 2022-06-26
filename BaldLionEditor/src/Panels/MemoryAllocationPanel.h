#pragma once

#include "EditorPanel.h"

namespace BaldLion {

	namespace Editor {

		class MemoryAllocationPanel : public EditorPanel{

		public:	
			MemoryAllocationPanel() : EditorPanel("Memory") {}
			virtual void OnImGuiRender() override;						
		};
	}
}
#pragma once
