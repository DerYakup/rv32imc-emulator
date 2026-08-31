===========================================================================
 Testprogramm 3: 03_rv32im  --  RV32IM-Tests (M-Erweiterung)
===========================================================================

ZIELGRUPPE
  Die M-Erweiterung (Multiplikation/Division) ist ergaenzt. Dieses Programm
  prueft alle acht M-Befehle samt Vorzeichen-Interpretation und den
  RISC-V-Sonderfaellen.

GETESTETE BEFEHLE
  - MUL     17 * 23 = 391; -7 * 6 = -42 (0xFFFFFFD6);
            0x10000 * 0x10000 = 0 (untere 32 Bit, Overflow)
  - MULH    obere 32 Bit, signed*signed:   (-1)*(-1) -> 0
  - MULHU   obere 32 Bit, unsigned*unsigned: 0xFFFFFFFF^2 -> 0xFFFFFFFE
  - MULHSU  obere 32 Bit, signed*unsigned:  -1 * 0xFFFFFFFF -> 0xFFFFFFFF
            (Ergebnis unterscheidet sich bewusst von MULH und MULHU!)
  - DIV     100/7 = 14; -100/7 = -14 (Round toward zero)
  - DIVU    100/7 = 14
  - REM     100%7 = 2; -100%7 = -2 (Vorzeichen folgt dem Dividend)

SONDERFAELLE (RISC-V-Spezifikation)
  - DIV  durch 0          -> 0xFFFFFFFF (= -1)
  - DIVU durch 0          -> 0xFFFFFFFF
  - REM  bei 0            -> Dividend (123)
  - DIV  INT_MIN / -1     -> INT_MIN (0x80000000)   (signierter Overflow)
  - REM  INT_MIN % -1    -> 0

HINWEIS
  Die M-Befehle werden per Inline-Assembler mit festen Operanden erzeugt.
  So sind die in C undefinierten Faelle (Division durch 0, INT_MIN / -1)
  garantiert vorhanden und werden nicht vom Compiler wegoptimiert.

UEBERSETZUNG
  -march=rv32im_zicsr_zifencei -mabi=ilp32

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
  Erwartete Schlusszeile: "16 von 16 Tests bestanden".
