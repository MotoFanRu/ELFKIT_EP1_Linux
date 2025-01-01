OUTPUT_FORMAT(binary)
ENTRY(APP_LDR_REGISTER)
SECTIONS
{
	. = 0x11240000;
	.text :
	{
		*(.text)
	}
	.data :
	{
		*(.data)
		*(.rodata)
	}
}
