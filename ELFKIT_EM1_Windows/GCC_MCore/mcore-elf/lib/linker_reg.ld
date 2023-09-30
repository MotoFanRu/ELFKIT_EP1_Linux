OUTPUT_FORMAT("binary")
ENTRY(_main)
SECTIONS
{
  . = 0x1117AE30;
  .text :
  {
    *(.text)
  }
}
