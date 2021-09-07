#include "syscall/guarded_keyboard.h"
#include "interrupt/secure.h"

Key Guarded_Keyboard::getkey()
{
    Secure sec;
    return Keyboard::getkey();
}

Guarded_Keyboard keyboard;
