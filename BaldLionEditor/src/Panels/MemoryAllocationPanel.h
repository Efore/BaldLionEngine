#pragma once

#include "EditorPanel.h"

namespace BaldLion {

	namespace Editor {

		class MemoryAllocationPanel : public EditorPanel{

		public:
			MemoryAllocationPanel() = default;			
			virtual void OnImGuiRender() override;						
		};
	}
}
#pragma once
