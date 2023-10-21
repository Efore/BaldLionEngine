#include "blpch.h"
#include "GameStateLayer.h"

namespace BaldLion
{
	GameStateLayer::GameStateLayer()
	{
		Time::RequestNewTimer(m_gameStateTimer);
	}

	GameStateLayer::~GameStateLayer()
	{
		Time::RemoveTimer(m_gameStateTimer.GetTimerID());
	}

	void GameStateLayer::OnPause()
	{		
		m_gameStateTimer.Pause();
	}

	void GameStateLayer::OnResume()
	{
		m_gameStateTimer.Resume();
	}

}