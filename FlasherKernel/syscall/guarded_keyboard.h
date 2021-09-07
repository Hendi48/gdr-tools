#pragma once

#include "device/keyboard.h"
#include "machine/key.h"

class Guarded_Keyboard : public Keyboard
{
private:
    Guarded_Keyboard (const Guarded_Keyboard &copy);
public:
    Guarded_Keyboard () {}
	Key getkey();
};

extern Guarded_Keyboard keyboard;
