//InputKeyUpEvent.h

#ifndef __INPUTKEYUPEVENT_H__
#define __INPUTKEYUPEVENT_H__

#include "../Event.h"
#include "InputKeyEvent.h"
#include <SDL.h>

class InputKeyUpEvent : public InputKeyEvent
{
public:
	InputKeyUpEvent(SDL_Keycode pressedKey)
		: InputKeyEvent(pressedKey) {}
};

#endif // !__INPUTKEYEVENT_H__
