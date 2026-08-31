/*
 * Minimal-Linkerscript fuer Meilenstein 2 (Erste Schritte).
 *
 * Die .text-Sektion beginnt bei Adresse 0, damit der per objcopy
 * extrahierte Bytestrom unmittelbar bei pc = 0 ausgefuehrt werden kann.
 */
ENTRY(_start)
SECTIONS
{
    . = 0x0;
    .text : { *(.text*) }
    /DISCARD/ : { *(.eh_frame) *(.riscv.attributes) *(.comment) }
}
