OUTPUT_FORMAT("elf32-mcore-big")
ENTRY(_main)
SECTIONS
{
	.text :
    {
	*(.text)
	. = ALIGN(4096);
    }
    .data :
    {
	*(.data)
	*(.rodata)
	. = ALIGN(4096);
    }
}
