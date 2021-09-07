#pragma once

#include "machine/io_port.h"
#include "machine/key.h"

class KeyboardController
{
private:
    KeyboardController(const KeyboardController &copy);

private:
    unsigned char code;
    unsigned char prefix;
    Key gather;
    char leds;

    const IOPort ctrl_port; // Status (R), Control (W)
    const IOPort data_port; // Output (R), Input (W)

    // Status register bits
    enum
    {
        outb = 0x01,
        inpb = 0x02,
        auxb = 0x20
    };

    // Commands
    struct kbd_cmd
    {
        enum
        {
            set_led = 0xed,
            set_speed = 0xf3
        };
    };
    enum
    {
        cpu_reset = 0xfe
    };

    // LED names
    struct led
    {
        enum
        {
            caps_lock = 4,
            num_lock = 2,
            scroll_lock = 1
        };
    };

    // Replies
    struct kbd_reply
    {
        enum
        {
            ack = 0xfa
        };
    };

    // Constants for decoding
    enum
    {
        break_bit = 0x80,
        prefix1 = 0xe0,
        prefix2 = 0xe1
    };

    static unsigned char normal_tab[];
    static unsigned char shift_tab[];
    static unsigned char alt_tab[];
    static unsigned char asc_num_tab[];
    static unsigned char scan_num_tab[];

    /*! \brief Retrieves make and brake events from the keyboard.
     *
     * \return true if decoding is complete, otherwise false.
     */
    bool key_decoded();

    // Uses tables to get the ASCII code from scancodes and modifiers of the pressed key
    void get_ascii_code();
    void send_cmd(int cmd, int data);

public:
    // Initialization: Turn off all LEDs and set repeat rate to max
    KeyboardController();

    /*! \brief Retrieve the keyboard event
     *
     * Retrieves make and brake events from the keyboard.
     * If a valid (non special) key was pressed, the scan code is determined
     * using \ref key_decoded() into a \ref Key object.
     * Events on special keys like \key{Shift}, \key{Alt}, \key{CapsLock} etc.
     * are stored and applied on subsequent keystrokes,
     * while no valid key is retrieved.
     *
     * \return Reference to an object which will contain the pressed \ref Key.
     *         Check whether it was valid using Key::valid().
     */
    Key key_hit();

    // Reboots the computer
    void reboot();

    /*! \brief Configure the repeat rate of the keyboard
     *
     * \param delay configures how long a key must be pressed before the repetition begins.
     * \param speed determines how fast the key codes should follow each other.
     *              Valid values are between `0` (30 characters per second) and
     *              `31` (2 characters per second).
     */
    void set_repeat_rate(int speed, int delay);

    /*! \brief Enable or disable a keyboard LED
     *
     *  \param led LED to enable or disable
     *  \param on `true` will enable the specified LED, `false` disable
     */
    void set_led(char led, bool on);
};
