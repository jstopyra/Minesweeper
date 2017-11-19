#pragma once

#include "../Event.h"
#include "InputKeyEvent.h"
#include <SDL.h>

class MouseClickEvent : public Event
{
	int m_x, m_y;


public:
	MouseClickEvent(int x, int y)
		: Event(k_mouseClickEvent)
		, m_x(x)
		, m_y(y)
	{
	};

	~MouseClickEvent() {};

	int GetXCord() { return m_x; }
	int GetYCord() { return m_y; }
};
