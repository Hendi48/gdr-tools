#include "device/cgastr.h"
#include "interrupt/secure.h"

CGAStream kout;

void CGAStream::flush() {
	print(&buf[0], index, 7);
	index = 0;
}
