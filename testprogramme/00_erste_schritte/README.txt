===========================================================================
 Testprogramme zum Abschnitt "Erste Schritte" der Aufgabenstellung
===========================================================================

Diese beiden Mini-Programme begleiten den Einstiegsabschnitt der
Aufgabenstellung. Sie ueben mit hand-decodierbaren Befehlen (ADDI, LUI, SB)
und sind absichtlich extrem kurz gehalten.

UNTERVERZEICHNISSE
  meilenstein_1_addi/     "addi x1, x0, 42" -- Regfile-Dump zeigt "1: 2A"
  meilenstein_2_hallo/    LUI + ADDI + SB-Folge -- gibt "Hallo" aus

INHALT JEDES UNTERVERZEICHNISSES
  instruction_mem.bin     Fertig uebersetzter Instruktionsspeicher
  data_mem.bin            Datenspeicher (16 Nullbytes)
  expected_output.txt     Erwartete Ausgabe / Regfile-Endzustand
  testprog.elf            ELF-Datei zum Inspizieren mit objdump
  src/                    Quellcode zum Selbst-Uebersetzen
    main.S                  Assembler mit kommentierten Befehlen
    link.ld                 Minimal-Linkerscript (.text bei Adresse 0)
    Makefile                Standalone-Build (keine externen Pfade)

AUFRUF DES EMULATORS
  ./hu-risc-v_emu meilenstein_1_addi/instruction_mem.bin \
                  meilenstein_1_addi/data_mem.bin
  ./hu-risc-v_emu meilenstein_2_hallo/instruction_mem.bin \
                  meilenstein_2_hallo/data_mem.bin

PROGRAMMENDE
  Beide .bin enden mit einem Nullwort (0x00000000). Solange im Skelett die
  default-Behandlung in CPU_execute() den pc unveraendert laesst, wirkt
  jeder noch nicht implementierte Opcode als natuerliches Programmende --
  der Emulator haelt also genau an der ersten Nullinstruktion.

ASSEMBLER ANSEHEN
  riscv32-unknown-elf-objdump -d meilenstein_1_addi/testprog.elf
  riscv32-unknown-elf-objdump -d meilenstein_2_hallo/testprog.elf

SELBST-UEBERSETZEN (optional)
  cd meilenstein_1_addi/src && make    # erzeugt instruction_mem.bin (8 B)
  cd meilenstein_2_hallo/src && make   # erzeugt instruction_mem.bin (56 B)
  Die fertigen .bin-Dateien liegen bereits vor; der Build ist nur zur
  Nachvollziehbarkeit gedacht.

SELBST-VERIFIKATION
  Meilenstein 1 -- der Regfile-Dump muss "1: 2A" enthalten:
    ./hu-risc-v_emu meilenstein_1_addi/instruction_mem.bin \
                    meilenstein_1_addi/data_mem.bin | grep "^1:"
  Meilenstein 2 -- die Zeichenausgabe vor dem Regfile-Dump:
    ./hu-risc-v_emu meilenstein_2_hallo/instruction_mem.bin \
                    meilenstein_2_hallo/data_mem.bin
    # erwartet:  Hallo
