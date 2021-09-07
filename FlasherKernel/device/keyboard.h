#pragma once

#include "machine/keyctrl.h"
#include "interrupt/gate.h"
#include "machine/key.h"
#include "sync/semaphore.h"

class Keyboard : public Gate, KeyboardController
{
private:
	Keyboard (const Keyboard &copy);

	Semaphore m_semaphore;
	Key m_key;
	bool m_buffer_read;
public:
	Keyboard() : m_semaphore(0), m_buffer_read(true) {};
 
 	Key getkey();
 
	// "Plug in" the keyboard. From then on key presses will be detected.
	void plugin();

	bool prologue();
	void epilogue();
};
