/*
 * ===========================================================================
 *  Testprogramm 4: RV32IMC (04_rv32imc)
 * ===========================================================================
 *
 *  Zielgruppe: C-Erweiterung (komprimierte 16-Bit-Befehle) ergaenzt.
 *
 *  Anders als die Programme 1-3 ist hier der KERN-TEST, dass der Compiler
 *  ueberhaupt komprimierte Befehle emittiert und der Emulator sie (nach
 *  Expansion auf 32 Bit) korrekt ausfuehrt. Der Code ist daher bewusst aus
 *  ganz normalem C aufgebaut - der Compiler komprimiert ihn unter
 *  -march=rv32imc selbststaendig (C.ADDI, C.MV, C.LWSP/C.SWSP, C.J, C.JR,
 *  C.BEQZ/C.BNEZ, C.SRLI/C.SRAI/C.ANDI, C.LUI, ...).
 *
 *  Verifikation der C-Befehle nach dem Build:
 *      riscv32-unknown-elf-objdump -d testprog.elf | head -50
 *  16-Bit-Opcodes erscheinen dort als 4-stellige Hexzahlen.
 *
 *  Uebersetzt mit -march=rv32imc_zicsr_zifencei -mabi=ilp32.
 * ===========================================================================
 */
#include "io.h"

/* Tiefe Rekursion: drueckt intensiv auf C.LWSP/C.SWSP (Stack-Slots) und
 * C.JAL/C.JR. fib(15) = 610.                                                */
static int32_t fib(int32_t n) {
    if (n < 2) return n;
    return fib(n - 1) + fib(n - 2);
}

/* Kleine, nicht-inline Funktion -> erzeugt C.ADD/C.MV/C.JR in der ABI.      */
static int32_t add3(int32_t a, int32_t b, int32_t c) {
    return a + b + c;
}

void main(void) {
    put_str("Testprogramm 4: RV32IMC (Compressed-Befehle)\n");

    /* --- 1. Auswahl der RV32IM-Tests (compiler-komprimiert) ------------- */
    test_group("RV32IM-Auswahl (indirekt C.* )");
    volatile int32_t x = 17, y = 23;
    volatile int32_t d = 100, m = 7;
    ASSERT_EQ("MUL 17 * 23 == 391", 391, x * y);
    ASSERT_EQ("DIV 100 / 7 == 14",  14,  d / m);
    ASSERT_EQ("REM 100 % 7 == 2",   2,   d % m);
    ASSERT_EQ("add3(10,20,12)==42", 42,  add3(10, 20, 12));

    /* --- 2. Tiefe Rekursion: Fibonacci ---------------------------------- */
    test_group("Rekursion fib(15) (C.LWSP/C.SWSP)");
    ASSERT_EQ("fib(15) == 610", 610, fib(15));

    /* --- 3. Bit-Operationen (typischerweise C.SRLI/C.SRAI/C.ANDI) ------- */
    test_group("Bit-Operationen (C.SRLI/C.SRAI/C.ANDI)");
    volatile uint32_t up = 0x00000080u;          /* positiv                  */
    volatile int32_t  sn = -64;                  /* negativ                  */
    ASSERT_EQ("C.SRLI 0x80 >> 3", 0x00000010u, up >> 3);
    ASSERT_EQ("C.SRAI -64 >> 3 (== -8)", 0xFFFFFFF8u, (uint32_t)(sn >> 3));
    ASSERT_EQ("C.ANDI 0xABCD & 0x0F", 0x0000000Du, (uint32_t)0xABCDu & 0x0Fu);

    /* --- 4. C.LUI mit verschiedenen Werten ------------------------------ */
    test_group("C.LUI (obere Bits)");
    volatile uint32_t l1 = 0x00001000u;
    volatile uint32_t l2 = 0x0001F000u;
    volatile uint32_t l3 = 0xFFFFF000u;          /* negative C.LUI-Immediate  */
    ASSERT_EQ("C.LUI 0x1  -> 0x00001000", 0x00001000u, l1);
    ASSERT_EQ("C.LUI 0x1F -> 0x0001F000", 0x0001F000u, l2);
    ASSERT_EQ("C.LUI neg  -> 0xFFFFF000", 0xFFFFF000u, l3);

    /* --- 5. Schleife mit kleinen Branch-Offsets (C.BEQZ/C.BNEZ) --------- */
    test_group("Schleife (C.BEQZ/C.BNEZ)");
    {
        volatile int32_t sum = 0;
        int32_t i = 10;
        while (i != 0) {            /* C.BNEZ                              */
            sum += i;
            i--;                    /* C.ADDI                              */
        }
        ASSERT_EQ("Summe 10..1 == 55", 55, sum);

        int32_t flag = 0;
        if (sum == 55) flag = 1;    /* Vergleich -> C.BEQZ/C.BNEZ          */
        ASSERT_EQ("flag gesetzt", 1, flag);
    }

    /* --- 6. Weitere komprimierte Befehle -------------------------------- */
    /* Compound-Zuweisungen (rd==rs1) komprimieren zu C.SUB/C.AND/C.OR/C.XOR;
     * "if (x)" erzeugt C.BEQZ; ein indirekter Aufruf C.JALR.               */
    test_group("Weitere C-Befehle (C.SUB/AND/OR/XOR, C.BEQZ, C.JALR)");
    {
        volatile int32_t va = 0x3C, vb = 0x0F;
        int32_t s2 = va; s2 -= vb;  ASSERT_EQ("C.SUB 0x3C - 0x0F", 0x2D, s2);
        int32_t an = va; an &= vb;  ASSERT_EQ("C.AND 0x3C & 0x0F", 0x0C, an);
        int32_t orr = va; orr |= vb;ASSERT_EQ("C.OR  0x3C | 0x0F", 0x3F, orr);
        int32_t xr = va; xr ^= vb;  ASSERT_EQ("C.XOR 0x3C ^ 0x0F", 0x33, xr);

        volatile int32_t nz = 5;
        int32_t beqz_flag = 0;
        if (nz) beqz_flag = 1;      /* "if (x)" -> C.BEQZ ueber Sprung      */
        ASSERT_EQ("C.BEQZ (if(nz))", 1, beqz_flag);

        /* Indirekter Aufruf ueber Funktionszeiger -> C.JALR.               */
        volatile int32_t p = 4, q = 5, r = 6;
        int32_t (*fp)(int32_t, int32_t, int32_t) = add3;
        ASSERT_EQ("C.JALR add3(4,5,6)", 15, fp(p, q, r));
    }

    print_summary();
}
