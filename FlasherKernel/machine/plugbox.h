#pragma once

#include "interrupt/gate.h"

#define IDT_COUNT 256

/*! \brief Object-oriented abstraction of an device interrupt table
 *
 * This allows you to specify the device handler for each hardware and software
 * interrupt and processor exception. Each device source is represented by a
 * \ref Gate object. These are located in an array with 256 elements, using
 * the index as the vector number.
 */
class Plugbox
{
private:
    Plugbox(const Plugbox &copy);
	Gate *m_gates[IDT_COUNT];
public:
	Plugbox();

	void assign(unsigned int slot, Gate& gate);
	Gate& report(unsigned int slot);

	enum {
		timer    = 32,
		keyboard = 33,
		ata_primary   = 46,
		ata_secondary = 47,
	};
};

extern Plugbox plugbox;
