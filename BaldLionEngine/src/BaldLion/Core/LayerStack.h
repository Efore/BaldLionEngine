#pragma once

#include "BaldLion/Core/Core.h"
#include "BaldLion/Core/Containers/DynamicArray.h"
#include "Layer.h"


namespace BaldLion
{
	class  LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void Init();
		void Delete();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		Layer* operator[](size_t index);
		const Layer* operator[](size_t index) const;

		size_t Size() const { return m_layers.Size(); }

	protected:
		DynamicArray<Layer*> m_layers;
		unsigned int m_layerInsertIndex = 0;
	};
}

