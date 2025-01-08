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
		m_layers = DynamicArray<GameStateLayer*>(Memory::AllocationType::FreeList_Main, 8);
	}

	void GameStateLayerStack::Delete()
	{
		m_layers.Delete();
	}

	void GameStateLayerStack::PushLayer(GameStateLayer * layer)
	{
		m_layers[m_layerInsertIndex]->OnDeactivate();
		m_layers.PushAt(layer, m_layerInsertIndex);
		++m_layerInsertIndex;
		layer->OnActivate();
	}

	void GameStateLayerStack::PushOverlay(GameStateLayer * overlay)
	{
		m_layers.PushBack(overlay);
		overlay->OnActivate();
	}

	void GameStateLayerStack::PopLayer(GameStateLayer * layer)
	{		
		layer->OnDeactivate();
		m_layers.RemoveAt(m_layerInsertIndex);
		--m_layerInsertIndex;
		m_layers[m_layerInsertIndex]->OnActivate();
	}

	void GameStateLayerStack::PopOverlay(GameStateLayer * overlay)
	{
		m_layers.Remove(overlay);
		overlay->OnDeactivate();
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