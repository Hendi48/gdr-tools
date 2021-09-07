#pragma once

#include "interrupt/gate.h"

/**
 * Default interrupt handler
 */
class Panic : public Gate
{
private:
    Panic (const Panic &copy);
public:
    Panic () {}
	bool prologue();
};
 
extern Panic panicDefaultGate;
extern int cur_interrupt_number;
extern void* cur_interrupt_context;
