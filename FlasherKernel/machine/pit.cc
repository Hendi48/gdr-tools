#include "machine/pit.h"

void PIT::interval(int us)
{
	m_interval = us;
	char b = (2 << 1) /* Periodic interrupts */ | (3 << 4) /* 16-bit counter */;
	m_pit1_ctrl.outb(b);
	us = us * 1000 / 838;
	m_pit1_counter0.outb(us);
	m_pit1_counter0.outb(us >> 8);
}
