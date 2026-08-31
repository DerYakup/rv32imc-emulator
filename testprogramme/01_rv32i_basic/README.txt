===========================================================================
 Testprogramm 1: 01_rv32i_basic  --  Einfache RV32I-Tests
===========================================================================

ZIELGRUPPE
  Die ersten RV32I-Befehle sind implementiert (ADDI, ADD, SUB, LW, SW,
  BEQ, JAL). Dieses Programm prueft, ob die Grundlagen stimmen.

GETESTETE BEFEHLE / FUNKTIONEN
  - SB            Zeichenausgabe ueber Memory-Mapped-IO (Adresse 0x5000)
  - ADDI / LI     Konstante in Register laden
  - ADD           Addition zweier Werte
  - SUB           Subtraktion
  - AND / OR / XOR  Logische Verknuepfungen mit konstanten Bitmustern
  - BEQ           Bedingter Sprung (genommen und nicht genommen)
  - JAL / JALR    Funktionsaufruf und Ruecksprung (add(a,b))
  - LW            Globales Array lesen
  - SW            Globales Array schreiben und zuruecklesen

BEWUSST NICHT ENTHALTEN
  Negative Zahlen, Shifts, vorzeichenbehaftete Vergleiche, Teilwort-Loads,
  Multiplikation/Division, C-Erweiterung. (Siehe Programme 2 bis 4.)

UEBERSETZUNG
  -march=rv32i_zicsr_zifencei -mabi=ilp32

DATEIEN
  instruction_mem.bin   Instruktionsspeicher (.init + .text)
  data_mem.bin          Datenspeicher (.rodata + .data + .sdata)
  expected_output.txt   Erwartete Programmausgabe eines korrekten Emulators
  testprog.elf          ELF-Datei zum Inspizieren mit objdump
  src/                  Quellcode zum Selbst-Uebersetzen (main.c, start.S,
                        link.ld, io.c, io.h, Makefile)

AUFRUF DES EMULATORS
  ./hu-risc-v_emu instruction_mem.bin data_mem.bin

ASSEMBLER ANSEHEN
  riscv32-unknown-elf-objdump -d testprog.elf

SELBST-UEBERSETZEN (optional)
  cd src && make
  Erzeugt testprog.elf, instruction_mem.bin, data_mem.bin und testprog.lst.
  Die fertigen .bin-Dateien liegen bereits vor; der Build ist nur zur
  Nachvollziehbarkeit gedacht.

SELBST-VERIFIKATION
  ./hu-risc-v_emu instruction_mem.bin data_mem.bin > meine_ausgabe.txt
  diff expected_output.txt meine_ausgabe.txt
  Die Programmausgabe (ab "Testprogramm 1" bis "----- Programm Ende -----")
  muss mit expected_output.txt uebereinstimmen. Die letzte inhaltliche Zeile
  lautet: "14 von 14 Tests bestanden".
