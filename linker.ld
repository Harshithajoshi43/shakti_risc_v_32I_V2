ENTRY(_start)

MEMORY {
  RAM (rwx) : ORIGIN = 0x0000, LENGTH = 6K
}

SECTIONS {
  . = 0x0;

  .text : {
    *(.text*)
  } > RAM

  .rodata : {
    *(.rodata*)
  } > RAM

  .data : {
    *(.data*)
  } > RAM

  .bss : {
    *(.bss*)
    *(COMMON)
  } > RAM
}
