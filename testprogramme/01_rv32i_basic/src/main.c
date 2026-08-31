/*
 * ===========================================================================
 *  Testprogramm 1: RV32I Grundlagen (01_rv32i_basic)
 * ===========================================================================
 *
 *  Zielgruppe: erste RV32I-Befehle implementiert (ADDI, ADD, SUB, LW, SW,
 *  BEQ, JAL). Dieses Programm prueft NUR die Grundlagen und vermeidet
 *  bewusst: negative Zahlen, Shifts, vorzeichenbehaftete Vergleiche,
 *  Teilwort-Zugriffe, Multiplikation/Division und die C-Erweiterung.
 *
 *  Uebersetzt mit -march=rv32i_zicsr_zifencei -mabi=ilp32.
 * ===========================================================================
 */
#include "io.h"

/* JAL/Funktionsaufruf: bewusst nicht-inline, damit ein echter Aufruf bleibt. */
static int add(int a, int b) {
    return a + b;
}

/* Globales Array fuer die Lade-/Speichertests. */
int32_t g_array[4] = { 10, 20, 30, 40 };

void main(void) {
    put_str("Testprogramm 1: RV32I Grundlagen\n");

    /* --- 1. Hello-World (testet SB/IO) ----------------------------------- */
    test_group("Hello-World (SB/IO)");
    put_str("  erwartet: Hallo Welt!\n");
    put_str("  tatsaechlich: Hallo Welt!\n");

    /* --- 2. Konstante in Register (ADDI/LI) ------------------------------ */
    test_group("Konstante in Register");
    volatile int32_t a = 42;
    ASSERT_EQ("a = 42", 42, a);

    /* --- 3. Addition zweier Konstanten (ADD) ----------------------------- */
    test_group("Addition");
    volatile int32_t x = 17, y = 25;
    ASSERT_EQ("17 + 25", 42, x + y);

    /* --- 4. Subtraktion (SUB) -------------------------------------------- */
    test_group("Subtraktion");
    volatile int32_t p = 100, q = 58;
    ASSERT_EQ("100 - 58", 42, p - q);

    /* --- 5. Logik AND/OR/XOR mit konstanten Mustern ---------------------- */
    test_group("Logik AND/OR/XOR");
    volatile uint32_t m1 = 0x0F0F0F0Fu, m2 = 0x00FF00FFu;
    ASSERT_EQ("AND", 0x000F000Fu, m1 & m2);
    ASSERT_EQ("OR",  0x0FFF0FFFu, m1 | m2);
    ASSERT_EQ("XOR", 0x0FF00FF0u, m1 ^ m2);

    /* --- 6. Branch genommen (BEQ trifft zu) ------------------------------ */
    test_group("Branch genommen (BEQ)");
    volatile int32_t b1 = 7, b2 = 7;
    int32_t taken = 0;
    if (b1 == b2) {
        taken = 1;          /* erwartet: dieser Zweig wird ausgefuehrt        */
    }
    ASSERT_EQ("BEQ 7==7 -> Zweig genommen", 1, taken);

    /* --- 7. Branch nicht genommen (BEQ trifft nicht zu) ------------------ */
    test_group("Branch nicht genommen (BEQ)");
    volatile int32_t c1 = 7, c2 = 8;
    int32_t not_taken = 5;
    if (c1 == c2) {
        not_taken = 99;     /* erwartet: dieser Zweig wird NICHT ausgefuehrt  */
    }
    ASSERT_EQ("BEQ 7==8 -> Zweig uebersprungen", 5, not_taken);

    /* --- 8. JAL und Funktionsaufruf -------------------------------------- */
    test_group("Funktionsaufruf (JAL/JALR)");
    volatile int32_t f1 = 19, f2 = 23;
    ASSERT_EQ("add(19, 23)", 42, add(f1, f2));

    /* --- 9. Globales Array lesen (LW) ------------------------------------ */
    test_group("Globales Array lesen (LW)");
    ASSERT_EQ("g_array[0]", 10, g_array[0]);
    ASSERT_EQ("g_array[1]", 20, g_array[1]);
    ASSERT_EQ("g_array[2]", 30, g_array[2]);
    ASSERT_EQ("g_array[3]", 40, g_array[3]);

    /* --- 10. Globales Array schreiben + zurueck lesen (SW/LW) ------------ */
    test_group("Globales Array schreiben (SW)");
    g_array[2] = 1234;
    ASSERT_EQ("g_array[2] nach Schreiben", 1234, g_array[2]);

    print_summary();
}
