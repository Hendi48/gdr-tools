#pragma once

#include "machine/io_port.h"

class PCI
{
private:
	IOPort m_port_addr, m_port_data;
    PCI(const PCI &copy);
public:
    PCI() : m_port_addr(0xCF8), m_port_data(0xCFC) {}

    int config_read_dword(int bus, int slot, int func, int offset);
    int config_read_byte(int bus, int slot, int func, int offset);
    void config_write_dword(int bus, int slot, int func, int offset, int value);
};

extern PCI pci;
