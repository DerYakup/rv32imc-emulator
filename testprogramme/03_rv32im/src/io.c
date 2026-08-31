/*
 * ===========================================================================
 *  io.c - Implementierung der gemeinsamen Ausgabe-Hilfsfunktionen
 * ===========================================================================
 *
 *  Bewusst freistehend (-ffreestanding, -fno-builtin): keine libc, keine
 *  printf-Familie. Alles wird Zeichen fuer Zeichen ueber die MMIO-Adresse
 *  0x5000 ausgegeben. Die hier erzeugten Stores sind SB-Befehle, die der
 *  Emulator als Zeichenausgabe interpretiert.
 * ===========================================================================
 */
#include "io.h"

#define IO_ADDR ((volatile uint8_t *)0x5000)

int tests_passed = 0;
int tests_failed = 0;

void put_char(char c) {
    *IO_ADDR = (uint8_t)c;          /* erzeugt einen SB-Befehl auf 0x5000     */
}

void put_str(const char *s) {
    while (*s) {
        put_char(*s++);
    }
}

void newline(void) {
    put_char('\n');
}

/* Vorzeichenlose Dezimalausgabe. */
void put_udec(uint32_t n) {
    char buf[10];
    int  i = 0;
    do {
        buf[i++] = (char)('0' + (n % 10u));
        n /= 10u;
    } while (n);
    while (i > 0) {
        put_char(buf[--i]);
    }
}

/* Vorzeichenbehaftete Dezimalausgabe. */
void put_dec(int32_t n) {
    uint32_t u;
    if (n < 0) {
        put_char('-');
        /* Umweg ueber int64_t, damit auch INT32_MIN korrekt negiert wird.    */
        u = (uint32_t)(-(int64_t)n);
    } else {
        u = (uint32_t)n;
    }
    put_udec(u);
}

/* "0x" + genau 8 Hexziffern (Grossbuchstaben). */
void put_hex(uint32_t n) {
    const char *hex = "0123456789ABCDEF";
    put_str("0x");
    for (int s = 28; s >= 0; s -= 4) {
        put_char(hex[(n >> s) & 0xF]);
    }
}

void test_group(const char *name) {
    put_str("\n--- ");
    put_str(name);
    put_str(" ---\n");
}

void print_summary(void) {
    put_str("\nSumme: ");
    put_dec(tests_passed);
    put_str(" von ");
    put_dec(tests_passed + tests_failed);
    put_str(" Tests bestanden\n");
    if (tests_failed == 0) {
        put_str("ALLE TESTS BESTANDEN\n");
    } else {
        put_str("ES GAB FEHLER: ");
        put_dec(tests_failed);
        put_str(" Test(s) fehlgeschlagen\n");
    }
    put_str("----- Programm Ende -----\n");
}
