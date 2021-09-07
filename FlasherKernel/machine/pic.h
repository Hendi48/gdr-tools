#pragma once

#include "machine/io_port.h"

/*
 * Programmable Interrupt Controller
 * Enables you to allow or mask interrupts individually.
 */
class PIC
{
private:
	IOPort m_pic1_command, m_pic1_data, m_pic2_command, m_pic2_data;
    PIC(const PIC &copy);
public:
    PIC() : m_pic1_command(0x20), m_pic1_data(0x21), m_pic2_command(0xa0), m_pic2_data(0xa1) {}

	enum {
		timer    = 0,
		keyboard = 1,
		ata_primary   = 14,
		ata_secondary = 15,
	};

	void allow (int interrupt_device);
	void forbid (int interrupt_device);
	bool is_masked (int interrupt_device);
	// When not using AEOI mode
	void ack(int irq);
};

extern PIC pic;
