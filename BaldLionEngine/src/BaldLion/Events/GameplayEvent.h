#pragma once

#include "Event.h"
#include "BaldLion/Core/Variant.h"

namespace BaldLion
{
	enum class GameplayEventType
	{
		PathfindingOnDestinationReached,
		HTNOnOperatorFinished
	};

	class GameplayEvent : public Event
	{
	public:
		GameplayEvent(GameplayEventType gameplayEventType, const Variant& data0, const Variant& data1, const Variant& data2) : m_gameplayEventType(gameplayEventType), m_data0(data0), m_data1(data1), m_data2(data2) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "GameplayEvent type: " << (ui8)m_gameplayEventType;
			return ss.str();
		}

		GameplayEventType GetGameplayEventType() const { return m_gameplayEventType; }
		const Variant& GetData0() const { return m_data0; }
		const Variant& GetData1() const { return m_data1; }
		const Variant& GetData2() const { return m_data2; }

		EVENT_CLASS_TYPE(Gameplay)
			EVENT_CLASS_CATEGORY(EventCatergoryGameplay)

	private:
		Variant m_data0;
		Variant m_data1;
		Variant m_data2;
		GameplayEventType m_gameplayEventType;
	};
}

