#include "blpch.h"
#include "GameStateLayerStack.h"

namespace BaldLion
{
	GameStateLayerStack::GameStateLayerStack()
	{

	}

	GameStateLayerStack::~GameStateLayerStack()
	{
		
	}

	void GameStateLayerStack::Init()
	{
		m_layers = DynamicArray<GameStateLayer*>(Memory::AllocationType::FreeList_Main, 10);
	}

	void GameStateLayerStack::Delete()
	{
		m_layers.Delete();
	}

	void GameStateLayerStack::PushLayer(GameStateLayer * layer)
	{
		m_layers.PushAt(layer, m_layerInsertIndex);
		++m_layerInsertIndex;
		layer->OnAttach();
	}

	void GameStateLayerStack::PushOverlay(GameStateLayer * overlay)
	{
		m_layers.PushBack(overlay);
		overlay->OnAttach();
	}

	void GameStateLayerStack::PopLayer(GameStateLayer * layer)
	{		
		m_layers.RemoveAtFast(m_layerInsertIndex);
		--m_layerInsertIndex;
		layer->OnDetach();
	}

	void GameStateLayerStack::PopOverlay(GameStateLayer * overlay)
	{
		m_layers.RemoveFast(overlay);
		overlay->OnDetach();
	}

	BaldLion::GameStateLayer* GameStateLayerStack::operator[](size_t index)
	{
		return m_layers[index];
	}

	const BaldLion::GameStateLayer* GameStateLayerStack::operator[](size_t index) const
	{
		return m_layers[index];
	}

}