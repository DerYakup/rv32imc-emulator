===========================================================================
 Testprogramm 2: 02_rv32i_advanced  --  Anspruchsvolle RV32I-Tests
===========================================================================

ZIELGRUPPE
  Der volle RV32I-Befehlssatz ist implementiert. Dieses Programm prueft
  Edge Cases - insbesondere Vorzeichen-Erweiterung und Vergleichssemantik.

GETESTETE BEFEHLE / FUNKTIONEN
  - SLL / SRL / SRA  Shifts; SRA mit negativem Wert (Vorzeichenerweiterung),
                     SRL mit negativem Wert (logisch, ohne Vorzeichen)
  - SLT / SLTU       Signed- vs. Unsigned-Vergleich (-1 < 1)
  - BLT/BGE/BLTU/BGEU  Alle bedingten Sprungvarianten
  - LB / LBU         Byte-Load mit/ohne Vorzeichenerweiterung (0x7F, 0x80)
  - LH / LHU         Halbwort-Load mit/ohne Vorzeichenerweiterung
  - SB / SH          Teilwort-Stores
  - SW / LW          Unaligned Word-Zugriff (ungerade Adresse)
  - AUIPC            pc-relative Adressierung (Abstand zweier AUIPC = 4)
  - LUI              Obere 20 Bit setzen (0x12345 -> 0x12345000)
  - JAL / JALR       Rekursive Fakultaet fak(10) = 3628800 (Stack-intensiv)
  - Schleife         Summe 1..100 = 5050
  - Linker-Sections  Globale (.data/.bss) vs. Stack-Variablen
  - LBU in Schleife  strlen("Hallo") = 5

HINWEIS
  fak() benutzt eine Multiplikation, die auf RV32I (ohne M-Erweiterung) per
  Software-Routine (__mulsi3 aus libgcc) ausgefuehrt wird - reine
  RV32I-Befehlsfolge, keine MUL-Instruktion noetig.

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
  Erwartete Schlusszeile: "28 von 28 Tests bestanden".
