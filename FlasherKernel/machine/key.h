#pragma once

/**
 * Key, represented by ASCII code, scan code and modifier.
 */
class Key
{
    unsigned char asc;
    unsigned char scan;
    unsigned char modi;

    // Bit masks for modifier
    struct mbit
     {
	enum
	{
	   shift       = 1,
	   alt_left    = 2,
	   alt_right   = 4,
	   ctrl_left   = 8,
	   ctrl_right  = 16,
	   caps_lock   = 32,
	   num_lock    = 64,
	   scroll_lock = 128
	};
     };

public:
    // Default constructor (invalid key)
    Key () : asc (0), scan (0), modi (0) {}

    // Returns whether the key is valid (meaning its scancode is not 0)
    bool valid ()      { return scan != 0; }
    
    // Sets scan code to zero, making this an invalid key
    void invalidate () { scan = 0; }

    void ascii (unsigned char a) { asc = a; }
    void scancode (unsigned char s) { scan = s; }
    unsigned char ascii () { return asc; }
    unsigned char scancode () { return scan; }

    // Functions for setting various modifiers

    void shift (bool pressed)
     { modi = pressed ? modi | mbit::shift : modi & ~mbit::shift; }
    void alt_left (bool pressed)
     { modi = pressed ? modi | mbit::alt_left : modi & ~mbit::alt_left; }
    void alt_right (bool pressed)
     { modi = pressed ? modi | mbit::alt_right : modi & ~mbit::alt_right; }
    void ctrl_left (bool pressed)
     { modi = pressed ? modi | mbit::ctrl_left : modi & ~mbit::ctrl_left; }
    void ctrl_right (bool pressed)
     { modi = pressed ? modi | mbit::ctrl_right : modi & ~mbit::ctrl_right; }
    void caps_lock (bool pressed)
     { modi = pressed ? modi | mbit::caps_lock : modi & ~mbit::caps_lock; }
    void num_lock (bool pressed)
     { modi = pressed ? modi | mbit::num_lock : modi & ~mbit::num_lock; }
    void scroll_lock (bool pressed)
     { modi = pressed ? modi | mbit::scroll_lock : modi & ~mbit::scroll_lock; }

    bool shift ()       { return modi & mbit::shift; }
    bool alt_left ()    { return modi & mbit::alt_left; }
    bool alt_right ()   { return modi & mbit::alt_right; }
    bool ctrl_left ()   { return modi & mbit::ctrl_left; }
    bool ctrl_right ()  { return modi & mbit::ctrl_right; }
    bool caps_lock ()   { return modi & mbit::caps_lock; }
    bool num_lock ()    { return modi & mbit::num_lock; }
    bool scroll_lock () { return modi & mbit::scroll_lock; }
    bool alt ()         { return alt_left ()  | alt_right (); }
    bool ctrl ()        { return ctrl_left () | ctrl_right (); }

    operator char ()    { return (char) asc; }

    // Scan codes for various special keys
    struct scan
    {
	enum
	{
	   f1 = 0x3b, del = 0x53, up=72, down=80, left=75, right=77,
	   div = 8, backspace = 0xE
	};
    };
};
