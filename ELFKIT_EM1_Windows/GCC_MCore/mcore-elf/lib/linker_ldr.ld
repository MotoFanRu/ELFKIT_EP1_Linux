OUTPUT_FORMAT("binary")
OUTPUT_ARCH(mcore)
ENTRY(_main)

SECTIONS{
    . = 0x09200000;

	.text :
    {
	*(.text)
	. = ALIGN(4096);
    }
    .data :
    {
	*(.data)
	. = ALIGN(4096);
    }
    .bss :
    {
	*(.bss)
	*(COMMON)
	. = ALIGN(4096);
    }
}
