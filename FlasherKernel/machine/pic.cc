#include "machine/pic.h"

PIC pic;

int pow2(int x) {
	int res = 1;
	while (x) {
		res *= 2;
		x--;
	}
	return res;
}

void PIC::allow (int interrupt_device) {
	if (interrupt_device >= 0 && interrupt_device < 8) {
		int current = m_pic1_data.inb();
		m_pic1_data.outb(current & ~pow2(interrupt_device));
	} else if (interrupt_device >= 8 && interrupt_device < 16) {
		int current = m_pic2_data.inb();
		m_pic2_data.outb(current & ~pow2(interrupt_device - 8));
	}
}

void PIC::forbid (int interrupt_device) {
	if (interrupt_device >= 0 && interrupt_device < 8) {
		int current = m_pic1_data.inb();
		m_pic1_data.outb(current | pow2(interrupt_device));
	} else if (interrupt_device >= 8 && interrupt_device < 16) {
		int current = m_pic2_data.inb();
		m_pic2_data.outb(current | pow2(interrupt_device - 8));
	}
}

bool PIC::is_masked (int interrupt_device) {
	if (interrupt_device >= 0 && interrupt_device < 8) {
		int current = m_pic1_data.inb();
		return current & pow2(interrupt_device);
	} else if (interrupt_device >= 8 && interrupt_device < 16) {
		int current = m_pic2_data.inb();
		return current & pow2(interrupt_device - 8);
	}
	return false;
}

void PIC::ack(int irq)
{
	if (irq >= 8)
		m_pic2_command.outb(0x20);
	m_pic1_command.outb(0x20);
}
