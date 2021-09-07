#pragma once

#include "interrupt/gate.h"
#include "machine/pit.h"

class Watch : public Gate, public PIT
{
private:
    Watch (const Watch &copy);
public:
    Watch (int us) : PIT (us) {}

    // "Winds up" the watch, after which it will start generating interrupts
    void plugin ();

    bool prologue ();
    void epilogue ();
};
