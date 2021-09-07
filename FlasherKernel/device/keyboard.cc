#include "device/keyboard.h"
#include "machine/plugbox.h"
#include "machine/pic.h"
#include "machine/cpu.h"

void Keyboard::plugin()
{
	plugbox.assign(Plugbox::keyboard, *this);
	pic.allow(PIC::keyboard);
}

bool Keyboard::prologue()
{
	// Store key and request epilogue
	Key key = key_hit();
	if (!key.valid()) {
		return false;
	}

	if (key.ctrl() && key.alt() && key.scancode() == Key::scan::del) {
		reboot();
	}

	m_key = key;
	return true;
}

void Keyboard::epilogue()
{
	if (m_buffer_read) {
		m_semaphore.signal();
		m_buffer_read = false;
	}
}

Key Keyboard::getkey()
{
	m_semaphore.wait();

	cpu.disable_int(); // disallow interrupts while copying m_key to stack
	Key key = m_key;
	m_buffer_read = true;
	cpu.enable_int();

	return key;
}
