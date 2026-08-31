/*
 * ===========================================================================
 *  start.S - Gemeinsamer Startup-Code fuer alle vier Testprogramme
 * ===========================================================================
 *
 *  Basiert auf der 2022er Vorlage. Setzt den Stackpointer an das obere Ende
 *  des Datenspeichers, ruft main() auf und beendet das Programm per EBREAK,
 *  sobald main() zurueckkehrt (der Emulator haelt bei EBREAK an).
 * ===========================================================================
 */
.section .init
.global _start
_start:
    lui   sp, 0x3FF      /* sp = 0x003FF000 (oberes Ende des Datenspeichers) */
    jal   main           /* Hauptprogramm aufrufen                          */
    ebreak               /* Programmende signalisieren (Emulator haelt an)  */
1:  j     1b             /* Sicherheitsnetz: Endlosschleife                 */
