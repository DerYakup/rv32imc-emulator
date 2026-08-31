/*
 * ===========================================================================
 *  Meilenstein 1: Der erste Befehl (ADDI)
 * ===========================================================================
 *
 *  Ein einziger Befehl, der sich vollstaendig per Hand decodieren laesst:
 *
 *      addi x1, x0, 42      # 0x02A00093  (regfile[1] = 0 + 42 = 42)
 *
 *  Nach dem Lauf erscheint im Regfile-Dump: "1: 2A".
 *
 *  Das anschliessende Nullwort wirkt als "natuerliches Programmende":
 *  solange der default-Fall in CPU_execute() den pc NICHT veraendert, haelt
 *  Ihr Emulator an der ersten unbekannten Instruktion (dem Nullwort) an.
 *
 *  Erzeugung der instruction_mem.bin (siehe Makefile): die .text-Sektion
 *  wird mit objcopy roh ins Binaerformat extrahiert. Daraus ergibt sich:
 *      93 00 A0 02   00 00 00 00     (Little-Endian, 8 Byte)
 * ===========================================================================
 */
.section .text
.global _start
_start:
    addi  x1, x0, 42         /* 0x02A00093 */
    .word 0x00000000         /* Programmende */
