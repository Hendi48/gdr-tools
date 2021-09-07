#include "machine/toc.h"

void toc_settle (struct toc* regs, void* tos, toc_kickoff_t kickoff)
{
	void **rsp = tos;
	*(--rsp) = kickoff;
	regs->rsp = rsp;
}
