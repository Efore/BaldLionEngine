#include "blpch.h"
#include "LayerStack.h"

namespace BaldLion
{
	LayerStack::LayerStack()
	{

	}

	LayerStack::~LayerStack()
	{
		
	}

	void LayerStack::Init()
	{
		m_layers = DynamicArray<Layer*>(Memory::AllocationType::FreeList_Main, 10);
	}

	void LayerStack::Delete()
	{
		m_layers.Delete();
	}

	void LayerStack::PushLayer(Layer * layer)
	{
		m_layers.PushAt(layer, m_layerInsertIndex);
		++m_layerInsertIndex;
		layer->OnAttach();
	}

	void LayerStack::PushOverlay(Layer * overlay)
	{
		m_layers.PushBack(overlay);
		overlay->OnAttach();
	}

	void LayerStack::PopLayer(Layer * layer)
	{		
		m_layers.RemoveAtFast(m_layerInsertIndex);
		--m_layerInsertIndex;
		layer->OnDetach();
	}

	void LayerStack::PopOverlay(Layer * overlay)
	{
		m_layers.RemoveFast(overlay);
		overlay->OnDetach();
	}

	BaldLion::Layer* LayerStack::operator[](size_t index)
	{
		return m_layers[index];
	}

	const BaldLion::Layer* LayerStack::operator[](size_t index) const
	{
		return m_layers[index];
	}

}