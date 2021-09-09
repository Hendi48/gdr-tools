OUTPUT_FORMAT("binary")

ENTRY(_start)

SECTIONS {
	. = 0x80000000;
	.cache : { * (.cache); }
}
