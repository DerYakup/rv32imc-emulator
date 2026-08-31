/*
 * ===========================================================================
 *  link.ld - Gemeinsames Linkerscript fuer alle vier Testprogramme
 * ===========================================================================
 *
 *  Harvard-Architektur: zwei getrennte Speicherregionen.
 *    iram : Instruktionsspeicher (.init, .text)   -> instruction_mem.bin
 *    dram : Datenspeicher (.rodata, .data, .bss)   -> data_mem.bin
 *
 *  WICHTIG: Die Daten beginnen bei Adresse 0x0 und muessen UNTERHALB der
 *  Memory-Mapped-IO-Adresse 0x5000 bleiben. Deshalb ist dram bewusst klein
 *  gehalten (0x4000 = 16 KiB); ein Datenzugriff darf niemals zufaellig auf
 *  0x5000 fallen. Der Stack liegt mit sp = 0x3FF000 weit darueber und kollidert
 *  nicht mit der IO-Adresse.
 * ===========================================================================
 */
ENTRY(_start)
MEMORY
{
    iram (rx) : ORIGIN = 0x80000000, LENGTH = 0x10000
    dram (rw) : ORIGIN = 0x00000000, LENGTH = 0x4000
}
SECTIONS
{
    .init   : { *(.init*)   } > iram
    .text   : { *(.text*)   } > iram
    .rodata : { *(.rodata*) } > dram
    .data   : { *(.data*)   } > dram
    .sdata  : { *(.sdata*)  } > dram
    .bss    : { *(.sbss*) *(.bss*) *(COMMON) } > dram
}
__stack_top = 0x3FF000;
