//InputKeyDownEvent.h

#ifndef __INPUTKEYDOWNEVENT_H__
#define __INPUTKEYDOWNEVENT_H__

#include "../Event.h"
#include "InputKeyEvent.h"
#include <SDL.h>

class InputKeyDownEvent : public InputKeyEvent
{
public:
	InputKeyDownEvent(SDL_Keycode pressedKey)
		: InputKeyEvent(pressedKey){}
};

#endif // !__INPUTKEYEVENT_H__
