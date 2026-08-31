/*
 * ===========================================================================
 *  io.h - Gemeinsame Ausgabe- und Test-Hilfsfunktionen
 * ===========================================================================
 *
 *  Alle Ausgaben gehen ueber Memory-Mapped-IO auf Adresse 0x5000: ein per
 *  SB (Store-Byte) dorthin geschriebenes Zeichen gibt der Emulator per
 *  putchar() auf der Standardausgabe aus.
 *
 *  Wird von allen vier Testprogrammen (01_rv32i_basic ... 04_rv32imc)
 *  gemeinsam genutzt; die zugehoerige Implementierung steht in io.c.
 * ===========================================================================
 */
#ifndef IO_H
#define IO_H

#include <stdint.h>

/* --- Ausgabe-Primitive (alle ueber 0x5000) ------------------------------- */
void put_char(char c);            /* ein Byte auf 0x5000                      */
void put_str(const char *s);      /* nullterminierter String                  */
void put_dec(int32_t n);          /* dezimal, mit Vorzeichen                   */
void put_udec(uint32_t n);        /* dezimal, ohne Vorzeichen                  */
void put_hex(uint32_t n);         /* "0x" + 8 Hexziffern                       */
void newline(void);               /* '\n'                                      */

/* --- Test-Zaehler (in io.c definiert) ------------------------------------ */
extern int tests_passed;
extern int tests_failed;

/*
 * ASSERT_EQ vergleicht Erwartungswert und tatsaechlichen Wert. Es gibt beide
 * Werte hexadezimal aus, damit Studierende mit halbfertigem Emulator sofort
 * sehen, WELCHER Einzeltest fehlschlaegt - nicht nur, DASS etwas falsch ist.
 */
#define ASSERT_EQ(name, expected, actual) do {                  \
        put_str("  " name ": erwartet=");                       \
        put_hex((uint32_t)(expected));                          \
        put_str(", tatsaechlich=");                             \
        put_hex((uint32_t)(actual));                            \
        if ((uint32_t)(expected) == (uint32_t)(actual)) {       \
            put_str("  OK\n"); tests_passed++;                  \
        } else {                                                \
            put_str("  FEHLER!\n"); tests_failed++;             \
        }                                                       \
    } while (0)

/* Ueberschrift einer Testgruppe.                                            */
void test_group(const char *name);

/* Schlussbilanz: "Summe: X von Y Tests bestanden" + Programm-Ende-Marke.    */
void print_summary(void);

#endif /* IO_H */
