#pragma once

#include "machine/io_port.h"

/**
 * Graphical console output
 */
class CGAScreen
{
private:
	CGAScreen(const CGAScreen &copy);
private:
    const IOPort index_port; // Index register (W)
    const IOPort data_port; // Data register (R/W)
    void checked_line_inc(int &y);
    void scroll();
public:
	CGAScreen();

	void show(int x, int y, char c, unsigned char attrib);
	void setpos(int x, int y);
	void getpos(int &x, int &y);
	void print(char* text, int length, unsigned char attrib);
};
