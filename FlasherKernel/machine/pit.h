#pragma once

#include "machine/io_port.h"

/*
 * Programmable Interrupt Timer
 */
class PIT
{
private:
    PIT(const PIT &copy);
	  
    int m_interval;
	  
    const IOPort m_pit1_counter0;
	  const IOPort m_pit1_ctrl;
public:
    PIT(int us) : m_pit1_counter0(0x40), m_pit1_ctrl(0x43) {
        interval(us);
    }

    int interval() { return m_interval; }
    void interval(int us);
};
