OUTPUT_FORMAT(binary)
ENTRY(_main)

SECTIONS
{
	. = 0x096C3D00;
	.text :
	{
		*(.text)
	}
	.data :
	{
		*(.data)
		*(.rodata)
	}
    .bss :
    {
		*(.bss)
		*(COMMON)
    }
}
