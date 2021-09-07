#include "machine/pci.h"

PCI pci;

int PCI::config_read_dword(int bus, int slot, int func, int offset)
{
    int address = (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xfc) | 0x80000000;

    m_port_addr.outl(address);
    return m_port_data.inl();
}

int PCI::config_read_byte(int bus, int slot, int func, int offset)
{
    return (config_read_dword(bus, slot, func, offset) >> ((offset & 3) * 8)) & 0xFF;
}

void PCI::config_write_dword(int bus, int slot, int func, int offset, int value)
{
    int address = (bus << 16) | (slot << 11) | (func << 8) | (offset) | 0x80000000;
    m_port_addr.outl(address);
    return m_port_data.outl(value);
}
