ENTRY(_start)

MEMORY
{
  ROM (rx)   : ORIGIN = 0x0000, LENGTH = 16K /* kernel .text and .rodata */
  RAM (rwx)  : ORIGIN = 0x2800, LENGTH = 8K /* kernel .data .bss and stack */
  MMIO (rw) : ORIGIN = 0xFC00, LENGTH = 1K /* 1k mmio */
}

SECTIONS
{
  .text : {
    KEEP(*(.text.start))
    *(.text*)
  } > ROM

  .rodata : {
    *(.rodata*)
    *(.srodata*)
  } > ROM

  .data : {
     *(.data*) 
  } > RAM

  .bss  : {
    *(.bss*)
    *(.sbss*)
    *(COMMON) 
  } > RAM
  
  _stack_top = ORIGIN(RAM) + LENGTH(RAM);
  _stack_bottom = _stack_top - 2K;
}
