#include "machine/cgascr.h"
#include <inttypes.h>

#define HIBYTE(x) ((x) >> 8)
#define CURSOR_HIGH 14
#define CURSOR_LOW 15

CGAScreen::CGAScreen () : 
   index_port (0x3d4), data_port (0x3d5)
{
}

void CGAScreen::show(int x, int y, char c, unsigned char attrib) {
	char* videoOffset = (char*) (uint64_t) (0xB8000 + y * 160 + 2 * x);
	*videoOffset = c;
	*(videoOffset+1) = attrib;
}

void CGAScreen::setpos(int x, int y) {
	uint16_t data = y * 80 + x;
	index_port.outb(CURSOR_LOW);
	data_port.outb(data);
	index_port.outb(CURSOR_HIGH);
	data_port.outb(HIBYTE(data));
}

void CGAScreen::getpos(int &x, int &y) {
	uint16_t data;
	index_port.outb(CURSOR_LOW);
	data = data_port.inb();
	index_port.outb(CURSOR_HIGH);
	data |= (data_port.inb() << 8);
	x = data % 80;
	y = data / 80;
}

void CGAScreen::print(char* text, int length, unsigned char attrib) {
	int x, y;
	getpos(x, y);
	for (int i = 0; i < length; i++) {
		if (*text != '\n') {
			show(x, y, *text++, attrib);
			x++;
			if (x >= 80) {
				x = 0;
				checked_line_inc(y);
			}
		} else {
			checked_line_inc(y);
			text++;
			x = 0;
		}
	}
	setpos(x, y);
	
}

#define LINE_COUNT 25
#define LINE_SIZE 80*2

void CGAScreen::checked_line_inc(int &y) {
	y++;
	if (y == LINE_COUNT) {
		scroll();
		y = LINE_COUNT - 1;
	}
}

void CGAScreen::scroll() {
	char* dst = (char*) (uint64_t) 0xB8000;
	char* src = dst + 160;
	for (int y = 1; y < LINE_COUNT; y++) {
		for (int x = 0; x < LINE_SIZE; x++) {
			*dst++ = *src++;
		}
	}
	// Clear last line
	for (int x = 0; x < LINE_SIZE; x++) {
		*dst++ = (x & 1) ? 7 : 0;
	}
}

