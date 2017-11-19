//InputKeyUpEvent.h

#ifndef __INPUTKEYEVENT_H__
#define __INPUTKEYEVENT_H__

#include "../Event.h"
#include <SDL.h>

class InputKeyEvent : public Event
{
public:


private:
	SDL_Keycode m_pressedKey;

public:
	InputKeyEvent(SDL_Keycode pressedKey)
		: Event(k_inputKeyEvent)
		, m_pressedKey(pressedKey)
	{

	};
	~InputKeyEvent() {};

	//list of keys that we can choose from.

	//-------------------------------------------------------------------------------------------------------------------
	//Getters/Setters
	//-------------------------------------------------------------------------------------------------------------------
	SDL_Keycode KeyTypePressed() const { return m_pressedKey; }


private:

};

#endif // !__INPUTKEYEVENT_H__
