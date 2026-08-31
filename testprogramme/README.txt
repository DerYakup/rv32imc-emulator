===========================================================================
 Testprogramme fuer den RV32IMC-Emulator
===========================================================================

Dieses Bundle enthaelt vier Testprogramme, mit denen Sie Ihren Emulator
stufenweise pruefen koennen - vom einfachsten RV32I bis zur C-Erweiterung.

  01_rv32i_basic/      Grundlagen RV32I (ADDI, ADD, SUB, LW, SW, BEQ, JAL)
  02_rv32i_advanced/   RV32I-Edge-Cases (Shifts, Vorzeichen, Branches, ...)
  03_rv32im/           M-Erweiterung (MUL/MULH/.../DIV/REM, Sonderfaelle)
  04_rv32imc/          C-Erweiterung (komprimierte 16-Bit-Befehle)

JEDER ORDNER ENTHAELT
  instruction_mem.bin   Instruktionsspeicher
  data_mem.bin          Datenspeicher
  expected_output.txt   Erwartete Programmausgabe eines korrekten Emulators
  README.txt            Welche Befehle dieses Programm prueft

BENUTZUNG
  ./hu-risc-v_emu 01_rv32i_basic/instruction_mem.bin \
                  01_rv32i_basic/data_mem.bin > meine_ausgabe.txt

  Ihre Emulator-Ausgabe enthaelt zusaetzlich Kopf- und Registerzeilen.
  Entscheidend ist der PROGRAMM-Teil von "Testprogramm X" bis
  "----- Programm Ende -----". Vergleich z.B. so:

  ./hu-risc-v_emu .../instruction_mem.bin .../data_mem.bin \
      | sed -n '/^Testprogramm/,/Programm Ende/p' \
      | diff - 01_rv32i_basic/expected_output.txt

  Kein Unterschied  ->  alle Tests bestanden.

LESEHILFE BEI FEHLERN
  Jeder Einzeltest gibt erwarteten und tatsaechlichen Wert hexadezimal aus:
    SLL 0x12345678 << 4: erwartet=0x23456780, tatsaechlich=0x23456780  OK
  Bei einer Abweichung steht dort "FEHLER!" - so sehen Sie sofort, WELCHER
  Befehl in Ihrem Emulator noch nicht stimmt. Am Ende:
    "X von Y Tests bestanden".

Empfohlene Reihenfolge: 01 -> 02 -> 03 -> 04. Erst wenn 01 vollstaendig
durchlaeuft, lohnt sich 02 usw.
