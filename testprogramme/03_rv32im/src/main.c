/*
 * ===========================================================================
 *  Testprogramm 3: RV32IM (03_rv32im)
 * ===========================================================================
 *
 *  Zielgruppe: M-Erweiterung (MUL/MULH/MULHU/MULHSU, DIV/DIVU/REM/REMU)
 *  ergaenzt. Prueft Vorzeichen-Interpretationen sowie die RISC-V-Sonderfaelle
 *  Division durch 0 (fuer DIV/DIVU/REM/REMU) und den Ueberlauf INT_MIN / -1
 *  (fuer DIV/REM).
 *
 *  Die M-Befehle werden ueber Inline-Assembler erzwungen: so sind die exakten
 *  Operanden garantiert, und die in C undefinierten Faelle (Division durch 0,
 *  INT_MIN / -1) werden nicht vom Compiler "wegoptimiert".
 *
 *  Uebersetzt mit -march=rv32im_zicsr_zifencei -mabi=ilp32.
 * ===========================================================================
 */
#include "io.h"

static uint32_t do_mul(int32_t a, int32_t b) {
    uint32_t r; __asm__ volatile("mul %0,%1,%2"    : "=r"(r) : "r"(a), "r"(b)); return r;
}
static uint32_t do_mulh(int32_t a, int32_t b) {
    uint32_t r; __asm__ volatile("mulh %0,%1,%2"   : "=r"(r) : "r"(a), "r"(b)); return r;
}
static uint32_t do_mulhu(uint32_t a, uint32_t b) {
    uint32_t r; __asm__ volatile("mulhu %0,%1,%2"  : "=r"(r) : "r"(a), "r"(b)); return r;
}
static uint32_t do_mulhsu(int32_t a, uint32_t b) {
    uint32_t r; __asm__ volatile("mulhsu %0,%1,%2" : "=r"(r) : "r"(a), "r"(b)); return r;
}
static uint32_t do_div(int32_t a, int32_t b) {
    uint32_t r; __asm__ volatile("div %0,%1,%2"    : "=r"(r) : "r"(a), "r"(b)); return r;
}
static uint32_t do_divu(uint32_t a, uint32_t b) {
    uint32_t r; __asm__ volatile("divu %0,%1,%2"   : "=r"(r) : "r"(a), "r"(b)); return r;
}
static uint32_t do_rem(int32_t a, int32_t b) {
    uint32_t r; __asm__ volatile("rem %0,%1,%2"    : "=r"(r) : "r"(a), "r"(b)); return r;
}
static uint32_t do_remu(uint32_t a, uint32_t b) {
    uint32_t r; __asm__ volatile("remu %0,%1,%2"   : "=r"(r) : "r"(a), "r"(b)); return r;
}

void main(void) {
    put_str("Testprogramm 3: RV32IM (Multiplikation/Division)\n");

    /* --- Multiplikation: untere 32 Bit (MUL) ---------------------------- */
    test_group("MUL (untere 32 Bit)");
    ASSERT_EQ("17 * 23 == 391",       391u,        do_mul(17, 23));
    ASSERT_EQ("-7 * 6 == -42",        0xFFFFFFD6u, do_mul(-7, 6));        /* -42 */
    ASSERT_EQ("0x10000 * 0x10000 (Overflow) == 0", 0u, do_mul(0x10000, 0x10000));

    /* --- Multiplikation: obere 32 Bit (MULH/MULHU/MULHSU) --------------- */
    test_group("MULH / MULHU / MULHSU");
    /* MULH (-1)*(-1): Produkt = 1 -> obere 32 Bit = 0                      */
    ASSERT_EQ("MULH   (-1)*(-1) obere = 0", 0u, do_mulh(-1, -1));
    /* MULHU 0xFFFFFFFF^2 = 0xFFFFFFFE00000001 -> obere = 0xFFFFFFFE        */
    ASSERT_EQ("MULHU  0xFFFFFFFF^2 obere = 0xFFFFFFFE",
              0xFFFFFFFEu, do_mulhu(0xFFFFFFFFu, 0xFFFFFFFFu));
    /* MULHSU -1 (signed) * 0xFFFFFFFF (unsigned) = 0xFFFFFFFF00000001      */
    /*        -> obere = 0xFFFFFFFF (unterscheidet sich von MULH und MULHU) */
    ASSERT_EQ("MULHSU -1 * 0xFFFFFFFF obere = 0xFFFFFFFF",
              0xFFFFFFFFu, do_mulhsu(-1, 0xFFFFFFFFu));

    /* --- Division und Rest (Normalfaelle) ------------------------------- */
    test_group("DIV / DIVU / REM / REMU");
    ASSERT_EQ("DIV  100 / 7 == 14",   14u,         do_div(100, 7));
    ASSERT_EQ("DIV  -100 / 7 == -14", 0xFFFFFFF2u, do_div(-100, 7));   /* -14 (toward 0) */
    ASSERT_EQ("DIVU 100 / 7 == 14",   14u,         do_divu(100, 7));
    ASSERT_EQ("REM  100 % 7 == 2",   2u,          do_rem(100, 7));
    ASSERT_EQ("REM  -100 % 7 == -2", 0xFFFFFFFEu, do_rem(-100, 7));   /* -2 (Vorzeichen=Dividend) */
    ASSERT_EQ("REMU 100 % 7 == 2",   2u,          do_remu(100u, 7u));
    /* 0xFFFFFFF6 = -10 als signed; REMU interpretiert ihn vorzeichenlos:    */
    /*   4294967286u / 7 = 613566755, Rest = 1 (unterscheidet sich von REM)   */
    ASSERT_EQ("REMU 0xFFFFFFF6 % 7 == 1", 1u, do_remu(0xFFFFFFF6u, 7u));

    /* --- Sonderfaelle gemaess RISC-V-Spezifikation ---------------------- */
    test_group("Sonderfaelle (Division durch 0, Overflow)");
    ASSERT_EQ("DIV  x / 0 == 0xFFFFFFFF",  0xFFFFFFFFu, do_div(123, 0));
    ASSERT_EQ("DIVU x / 0 == 0xFFFFFFFF",  0xFFFFFFFFu, do_divu(123, 0));
    ASSERT_EQ("REM  x % 0 == x (=123)",   123u,        do_rem(123, 0));
    ASSERT_EQ("REMU x % 0 == x (=123)",   123u,        do_remu(123u, 0u));
    /* INT_MIN / -1: signierter Overflow -> rd = INT_MIN, REM = 0          */
    ASSERT_EQ("DIV  INT_MIN / -1 == INT_MIN",
              0x80000000u, do_div((int32_t)0x80000000u, -1));
    ASSERT_EQ("REM  INT_MIN % -1 == 0",
              0u, do_rem((int32_t)0x80000000u, -1));

    print_summary();
}
