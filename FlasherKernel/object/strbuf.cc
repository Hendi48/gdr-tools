#include "object/strbuf.h"

void Stringbuffer::put(char c)
{
	buf[index++] = c;
	if (index == sizeof(buf)) {
		flush();
	}
}
