#pragma once

#include "BaldLion/Core.h"
#include "Layer.h"


namespace BaldLion
{
	class BL_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_layers.end(); }

	protected:
		std::vector<Layer*> m_layers;
		unsigned int m_layerInsertIndex = 0;
	};
}

