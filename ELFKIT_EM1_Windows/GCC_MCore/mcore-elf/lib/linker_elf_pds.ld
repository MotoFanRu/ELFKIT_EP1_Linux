OUTPUT_FORMAT("elf32-mcore-big")
ENTRY(_main)
SECTIONS
{
	.text :
    {
	*(.text)
	. = ALIGN(4096);
    }
    .data ADDR(.text)+SIZEOF(.text):
    {
	*(.data)
	*(.rodata)
	. = ALIGN(4096);
    }
    .bss 0x09000000:
    {
	*(.bss)
	*(COMMON)
	. = ALIGN(4096);
    }
}
