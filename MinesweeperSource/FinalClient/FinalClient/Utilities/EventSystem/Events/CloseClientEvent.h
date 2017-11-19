#pragma once

#include "../Event.h"
#include "InputKeyEvent.h"
#include <SDL.h>

class CloseClientEvent : public Event
{

public:
	CloseClientEvent()
		: Event(k_closeClientEvent)
	{
	};

	~CloseClientEvent() {};

};
