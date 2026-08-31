/*
 * ===========================================================================
 *  Meilenstein 2: Sichtbare Ausgabe (LUI + SB)
 * ===========================================================================
 *
 *  Schreibt nacheinander die Zeichen 'H', 'a', 'l', 'l', 'o', '\n' auf die
 *  Memory-Mapped-IO-Adresse 0x5000. Der Emulator interpretiert jedes per
 *  SB dorthin geschriebene Byte als Zeichenausgabe und ruft putchar() auf.
 *
 *  Erwartete Ausgabe beim Lauf:
 *      Hallo
 *
 *  Das abschliessende Nullwort wirkt wie in Meilenstein 1 als natuerliches
 *  Programmende, solange im Skelett der default-Fall in CPU_execute() den
 *  pc unveraendert laesst.
 * ===========================================================================
 */
.section .text
.global _start
_start:
    lui   x6, 0x5            /* x6 = 0x5000 (IO-Adresse)        */

    addi  x5, x0, 72         /* 'H' = 0x48                      */
    sb    x5, 0(x6)

    addi  x5, x0, 97         /* 'a' = 0x61                      */
    sb    x5, 0(x6)

    addi  x5, x0, 108        /* 'l' = 0x6C                      */
    sb    x5, 0(x6)

    addi  x5, x0, 108        /* 'l' = 0x6C                      */
    sb    x5, 0(x6)

    addi  x5, x0, 111        /* 'o' = 0x6F                      */
    sb    x5, 0(x6)

    addi  x5, x0, 10         /* '\n' = 0x0A                     */
    sb    x5, 0(x6)

    .word 0x00000000         /* Programmende                    */
