/*
 * ===========================================================================
 *  Testprogramm 2: RV32I Fortgeschritten (02_rv32i_advanced)
 * ===========================================================================
 *
 *  Zielgruppe: voller RV32I-Befehlssatz implementiert. Prueft die Edge Cases:
 *  Shifts (inkl. Vorzeichenerweiterung), signed/unsigned-Vergleiche, alle
 *  Branch-Varianten, vorzeichenbehaftete Teilwort-Loads, unaligned Zugriffe,
 *  AUIPC/LUI, Rekursion, Iteration, globale vs. Stack-Variablen und strlen.
 *
 *  Uebersetzt mit -march=rv32i_zicsr_zifencei -mabi=ilp32.
 * ===========================================================================
 */
#include "io.h"

/* Rekursive Fakultaet: uebt Stack, JAL/JALR, Loads/Stores in Kombination.   */
static uint32_t fak(uint32_t n) {
    if (n <= 1) return 1;
    return n * fak(n - 1);
}

/* Eigene strlen-Implementierung (uebt LBU in einer Schleife).               */
static int32_t my_strlen(const char *s) {
    int32_t n = 0;
    while (s[n] != '\0') {
        n++;
    }
    return n;
}

/* Globale Variablen (Linker-Sections .data/.bss, getrennt vom Stack).       */
int32_t  g_counter = 1000;             /* .data: initialisiert               */
int32_t  g_zero;                       /* .bss : nullinitialisiert            */
uint8_t  g_buf[8];                      /* .bss : Byte-Puffer fuer Loads       */

void main(void) {
    put_str("Testprogramm 2: RV32I Fortgeschritten\n");

    /* --- 1. Shifts SLL/SRL/SRA ------------------------------------------ */
    test_group("Shifts SLL/SRL/SRA");
    volatile uint32_t uv = 0x12345678u;
    volatile uint32_t un = 0xFF000000u;          /* als unsigned             */
    volatile int32_t  sn = (int32_t)0xFF000000u; /* als signed (negativ)     */
    volatile int32_t  sp = 0x40000000;           /* positiv                  */
    ASSERT_EQ("SLL 0x12345678 << 4", 0x23456780u, uv << 4);
    ASSERT_EQ("SRL 0x12345678 >> 4", 0x01234567u, uv >> 4);
    ASSERT_EQ("SRL (unsigned) 0xFF000000 >> 8", 0x00FF0000u, un >> 8);
    ASSERT_EQ("SRA (signed neg) >> 8", 0xFFFF0000u, (uint32_t)(sn >> 8));
    ASSERT_EQ("SRA (signed pos) >> 8", 0x00400000u, (uint32_t)(sp >> 8));

    /* --- 2. Signed vs. unsigned Vergleiche (SLT/SLTU) ------------------- */
    test_group("SLT / SLTU");
    volatile int32_t  s_m1 = -1, s_p1 = 1;
    volatile uint32_t u_max = 0xFFFFFFFFu, u_one = 1;
    ASSERT_EQ("SLT  -1 < 1 (signed)",   1, (s_m1 < s_p1));
    ASSERT_EQ("SLTU -1 < 1 (unsigned)", 0, (u_max < u_one));

    /* --- 3. Branches BLT/BGE/BLTU/BGEU ---------------------------------- */
    test_group("Branches BLT/BGE/BLTU/BGEU");
    volatile int32_t  a = -5, b = 3;
    volatile uint32_t ua = 3, ub = 5, umax = 0xFFFFFFFFu;
    int32_t r;
    if (a < b)   r = 1; else r = 0;   ASSERT_EQ("BLT  -5 < 3",      1, r);
    if (b >= a)  r = 1; else r = 0;   ASSERT_EQ("BGE   3 >= -5",    1, r);
    if (ua < ub) r = 1; else r = 0;   ASSERT_EQ("BLTU  3 < 5",      1, r);
    if (umax >= ub) r = 1; else r = 0;ASSERT_EQ("BGEU max >= 5",    1, r);

    /* --- 4. LB/LBU mit positivem und negativem Byte --------------------- */
    test_group("LB / LBU (Vorzeichenerweiterung)");
    g_buf[0] = 0x7F;                  /* positiv                            */
    g_buf[1] = 0x80;                  /* negativ als signed                 */
    {
        volatile int8_t  *sb = (volatile int8_t  *)g_buf;
        volatile uint8_t *bu = (volatile uint8_t *)g_buf;
        ASSERT_EQ("LB  0x7F", 0x0000007Fu, (uint32_t)(int32_t)sb[0]);
        ASSERT_EQ("LBU 0x7F", 0x0000007Fu, (uint32_t)bu[0]);
        ASSERT_EQ("LB  0x80 (->negativ)", 0xFFFFFF80u, (uint32_t)(int32_t)sb[1]);
        ASSERT_EQ("LBU 0x80", 0x00000080u, (uint32_t)bu[1]);
    }

    /* --- 5. LH/LHU mit positivem und negativem Halbwort ----------------- */
    test_group("LH / LHU (Vorzeichenerweiterung)");
    {
        volatile uint16_t *hw = (volatile uint16_t *)g_buf; /* SH zum Setzen */
        volatile int16_t  *sh = (volatile int16_t  *)g_buf;
        hw[0] = 0x7FFF;              /* positiv                             */
        hw[1] = 0x8000;             /* negativ als signed                  */
        ASSERT_EQ("LH  0x7FFF", 0x00007FFFu, (uint32_t)(int32_t)sh[0]);
        ASSERT_EQ("LHU 0x7FFF", 0x00007FFFu, (uint32_t)hw[0]);
        ASSERT_EQ("LH  0x8000 (->negativ)", 0xFFFF8000u, (uint32_t)(int32_t)sh[1]);
        ASSERT_EQ("LHU 0x8000", 0x00008000u, (uint32_t)hw[1]);
    }

    /* --- 6. Unaligned Word-Zugriff (SW/LW auf ungerader Adresse) -------- */
    test_group("Unaligned Word-Zugriff");
    {
        volatile uint32_t *wp = (volatile uint32_t *)(g_buf + 1); /* +1: ungerade */
        *wp = 0xDEADBEEFu;
        ASSERT_EQ("SW/LW @ ungerade Adresse", 0xDEADBEEFu, *wp);
    }

    /* --- 7. AUIPC: pc-relative Adressierung ----------------------------- */
    test_group("AUIPC (pc-relativ)");
    {
        /* Zwei aufeinanderfolgende AUIPC liefern pc bzw. pc+4 (RV32I, 4 B). */
        uint32_t pc0, pc1;
        __asm__ volatile("auipc %0, 0\n\t"
                         "auipc %1, 0" : "=r"(pc0), "=r"(pc1));
        ASSERT_EQ("Abstand zweier AUIPC == 4", 4, pc1 - pc0);
    }

    /* --- 8. LUI: obere 20 Bit setzen ------------------------------------ */
    test_group("LUI (obere 20 Bit)");
    {
        uint32_t lui_val;
        __asm__ volatile("lui %0, 0x12345" : "=r"(lui_val));
        ASSERT_EQ("LUI 0x12345 -> 0x12345000", 0x12345000u, lui_val);
    }

    /* --- 9. Rekursion: Fakultaet ---------------------------------------- */
    test_group("Rekursion fak(10)");
    ASSERT_EQ("fak(10) == 3628800", 3628800u, fak(10));

    /* --- 10. Iteration: Summe 1..100 ------------------------------------ */
    test_group("Iteration Summe 1..100");
    {
        volatile int32_t sum = 0;
        for (int32_t i = 1; i <= 100; i++) {
            sum += i;
        }
        ASSERT_EQ("Summe 1..100 == 5050", 5050, sum);
    }

    /* --- 11. Globale vs. Stack-Variablen -------------------------------- */
    test_group("Globale vs. Stack-Variablen");
    {
        volatile int32_t local = 7;     /* auf dem Stack                    */
        g_counter += 337;               /* in .data                         */
        g_zero += 42;                   /* in .bss (war 0)                  */
        ASSERT_EQ("Stack-Variable",  7,    local);
        ASSERT_EQ(".data g_counter", 1337, g_counter);
        ASSERT_EQ(".bss  g_zero",    42,   g_zero);
    }

    /* --- 12. Strings: strlen -------------------------------------------- */
    test_group("strlen (LBU in Schleife)");
    ASSERT_EQ("strlen(\"Hallo\") == 5", 5, my_strlen("Hallo"));

    print_summary();
}
