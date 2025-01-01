OUTPUT_FORMAT("elf32-mcore-big")
ENTRY(ELF_Entry)
SECTIONS
{
    .text :
    {
    	*(.text)
    }
    .data :
    {
    	*(.data)
    	*(.rodata)
        *(.bss)
        *(COMMON)
    }
}
