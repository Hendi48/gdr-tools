#include "machine/keyctrl.h"
#include "machine/pic.h"

unsigned char KeyboardController::normal_tab[] =
	{
		0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 225, 39, '\b',
		0, 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 129, '+', '\n',
		0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 148, 132, '^', 0, '#',
		'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0,
		'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
		0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, '<', 0, 0};

unsigned char KeyboardController::shift_tab[] =
	{
		0, 0, '!', '"', 21, '$', '%', '&', '/', '(', ')', '=', '?', 96, 0,
		0, 'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', 154, '*', 0,
		0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 153, 142, 248, 0, 39,
		'Y', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_', 0,
		0, 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '>', 0, 0};

unsigned char KeyboardController::alt_tab[] =
	{
		0, 0, 0, 253, 0, 0, 0, 0, '{', '[', ']', '}', '\\', 0, 0,
		0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '~', 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 230, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '|', 0, 0};

unsigned char KeyboardController::asc_num_tab[] =
	{
		'7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', ','};
unsigned char KeyboardController::scan_num_tab[] =
	{
		8, 9, 10, 53, 5, 6, 7, 27, 2, 3, 4, 11, 51};

bool KeyboardController::key_decoded()
{
	bool done = false;

	// All keys that are introduced by the MF II keyboard (compared to the older AT keyboard)
	// always send a prefix value as first byte.
	if (code == prefix1 || code == prefix2)
	{
		prefix = code;
		return false;
	}

	// Key release is only relevant for CTRL/ALT/SHIFT modifiers
	if (code & break_bit)
	{
		code &= ~break_bit;
		switch (code)
		{
		case 42:
		case 54:
			gather.shift(false);
			break;
		case 56:
			if (prefix == prefix1)
				gather.alt_right(false);
			else
				gather.alt_left(false);
			break;
		case 29:
			if (prefix == prefix1)
				gather.ctrl_right(false);
			else
				gather.ctrl_left(false);
			break;
		}

		// Prefix handled (only valid for a single following key)
		prefix = 0;

		// Can't do anything with a break code alone, so return false
		return false;
	}

	switch (code)
	{
	case 42:
	case 54:
		gather.shift(true);
		break;
	case 56:
		if (prefix == prefix1)
			gather.alt_right(true);
		else
			gather.alt_left(true);
		break;
	case 29:
		if (prefix == prefix1)
			gather.ctrl_right(true);
		else
			gather.ctrl_left(true);
		break;
	case 58:
		gather.caps_lock(!gather.caps_lock());
		set_led(led::caps_lock, gather.caps_lock());
		break;
	case 70:
		gather.scroll_lock(!gather.scroll_lock());
		set_led(led::scroll_lock, gather.scroll_lock());
		break;
	case 69:					// Numlock or Pause ?
		if (gather.ctrl_left()) // Pause key
		{
			// On old keyboards, the pause functionality was only accessible by
			// pressing Ctrl+NumLock. Modern MF-II keyboards therefore send exactly
			// this code combination when the pause key was pressed.
			// Normally, the pause key does not provide an ASCII code, but we check
			// that anyway. In either case, we're now done decoding.
			get_ascii_code();
			done = true;
		}
		else // NumLock
		{
			gather.num_lock(!gather.num_lock());
			set_led(led::num_lock, gather.num_lock());
		}
		break;

	default: // all other keys
		// Read ASCII code from the relevant tables.
		get_ascii_code();
		done = true;
	}

	// Prefix handled (only valid for a single following key)
	prefix = 0;

	return done;
}

void KeyboardController::get_ascii_code()
{
	// Special case scan code 53: This code is used by both the minus key on the main
	// keyboard and the division key on the number block.
	// When the division key was pressed, we adjust the scancode accordingly.
	if (code == 53 && prefix == prefix1) // Division key on number block
	{
		gather.ascii('/');
		gather.scancode(Key::scan::div);
	}

	// Select the correct table according to the modifier bits.
	// For simplicity's sake NumLock takes precedence before Alt,
	// Shift and CapsLock. There is no table for Ctrl.

	else if (gather.num_lock() && !prefix && code >= 71 && code <= 83)
	{
		// When NumLock is enabled and a key on the keypad was pressed, we
		// want return the ASCII and scan codes of the corresponding numerical
		// key instead of the arrow keys.
		// The keys on the cursor block (prefix == prefix1), however, should
		// remain usable.
		gather.ascii(asc_num_tab[code - 71]);
		gather.scancode(scan_num_tab[code - 71]);
	}
	else if (gather.alt_right())
	{
		gather.ascii(alt_tab[code]);
		gather.scancode(code);
	}
	else if (gather.shift())
	{
		gather.ascii(shift_tab[code]);
		gather.scancode(code);
	}
	else if (gather.caps_lock())
	{ // Caps lock is only for characters
		if ((code >= 16 && code <= 26) || (code >= 30 && code <= 40) || (code >= 44 && code <= 50))
		{
			gather.ascii(shift_tab[code]);
			gather.scancode(code);
		}
		else
		{
			gather.ascii(normal_tab[code]);
			gather.scancode(code);
		}
	}
	else
	{
		gather.ascii(normal_tab[code]);
		gather.scancode(code);
	}
}

KeyboardController::KeyboardController() : ctrl_port(0x64), data_port(0x60)
{
	// Turn off all LEDs
	set_led(led::caps_lock, false);
	set_led(led::scroll_lock, false);
	set_led(led::num_lock, false);

	// max speed, min delay
	set_repeat_rate(0, 0);
}

Key KeyboardController::key_hit()
{
	Key invalid;

	if (!(ctrl_port.inb() & outb) || ctrl_port.inb() & auxb)
	{
		return invalid; // either no data available or from mouse
	}

	code = data_port.inb(); // set class member, accessed in key_decoded()
	return key_decoded() ? gather : invalid;
}

void KeyboardController::reboot()
{
	int status;

	// Indicate to the BIOS that a reset was explicitly requested,
	// and no memtest should be performed.

	*(unsigned short *)0x472 = 0x1234;

	// Wait until all commands are processed
	do
	{
		status = ctrl_port.inb();
	} while ((status & inpb) != 0);
	ctrl_port.outb(cpu_reset); // Reset
}

void KeyboardController::set_repeat_rate(int speed, int delay)
{
	if (speed < 0 || speed > 31 || delay < 0 || delay > 3)
	{
		return; // Invalid
	}
	send_cmd(kbd_cmd::set_speed, delay << 5 | speed);
}

void KeyboardController::set_led(char led, bool on)
{
	leds = on ? (leds | led) : (leds & ~led);
	send_cmd(kbd_cmd::set_led, leds);
}

void KeyboardController::send_cmd(int cmd, int data)
{
	bool was_accepting = !pic.is_masked(PIC::keyboard);
	if (was_accepting)
	{
		pic.forbid(PIC::keyboard);
	}

	while (ctrl_port.inb() & inpb)
		; // Ensure input buf empty

	data_port.outb(cmd);

	while (!(ctrl_port.inb() & outb))
		; // Poll for output flag
	if (!(data_port.inb() & kbd_reply::ack))
	{
		return;
	}

	data_port.outb(data);

	while (!(ctrl_port.inb() & outb))
		;
	if (!(data_port.inb() & kbd_reply::ack))
	{
		return;
	}

	if (was_accepting)
	{
		pic.allow(PIC::keyboard);
	}
}
