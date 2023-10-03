#pragma once

#include "BaldLion/Core/Core.h"
#include "BaldLion/Core/Containers/DynamicArray.h"
#include "GameStateLayer.h"


namespace BaldLion
{
	class  GameStateLayerStack
	{
	public:
		GameStateLayerStack();
		~GameStateLayerStack();

		void Init();
		void Delete();

		void PushLayer(GameStateLayer* layer);
		void PushOverlay(GameStateLayer* overlay);
		void PopLayer(GameStateLayer* layer);
		void PopOverlay(GameStateLayer* overlay);

		GameStateLayer* operator[](size_t index);
		const GameStateLayer* operator[](size_t index) const;

		size_t Size() const { return m_layers.Size(); }

	protected:
		DynamicArray<GameStateLayer*> m_layers;
		unsigned int m_layerInsertIndex = 0;
	};
}

