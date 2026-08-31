===========================================================================
 Testprogramm 4: 04_rv32imc  --  RV32IMC-Tests (C-Erweiterung)
===========================================================================

ZIELGRUPPE
  Die C-Erweiterung (komprimierte 16-Bit-Befehle) ist ergaenzt.

BESONDERHEIT
  Anders als die Programme 1-3 ist hier der KERN-TEST, dass der Compiler
  ueberhaupt komprimierte Befehle emittiert und der Emulator sie nach der
  Expansion auf 32 Bit korrekt ausfuehrt. Der Code besteht aus normalem C,
  das der Compiler unter -march=rv32imc selbststaendig komprimiert.

GETESTETE INHALTE
  - Auswahl der RV32IM-Tests (MUL/DIV/REM) -> indirekt haeufige C-Befehle
    (C.ADDI, C.MV, C.ADD, C.LWSP, C.SWSP, C.J, C.JR, C.JAL, C.LI, ...)
  - Tiefe Rekursion: fib(15) = 610 (intensiv C.LWSP/C.SWSP, C.JAL/C.JR)
  - Bit-Operationen: C.SRLI / C.SRAI (>> 3, beide Vorzeichen), C.ANDI (& 0x0F)
  - C.LUI mit verschiedenen Werten (auch negative Immediate)
  - Schleife mit kleinen Offsets: C.BNEZ / C.BEQZ
  - Compound-Zuweisungen: C.SUB / C.AND / C.OR / C.XOR
  - Indirekter Aufruf: C.JALR

VERIFIKATION DER C-ERWEITERUNG (im Quell-Bundle)
  make verify-compressed
    -> listet die im ELF emittierten c.*-Befehle und prueft, dass es
       mindestens 20 verschiedene sind. Aktuell: 23 verschiedene C-Befehle:
       c.add c.addi c.addi16sp c.addi4spn c.and c.andi c.beqz c.bnez
       c.ebreak c.j c.jal c.jalr c.jr c.li c.lui c.lwsp c.mv c.or c.slli
       c.srai c.srli c.swsp c.xor
  Hintergrund: 16-Bit-Befehle erscheinen im Disassembly (objdump -d) als
  4-stellige Hexzahlen; mit "-M no-aliases" zeigt objdump die c.*-Mnemonics.

UEBERSETZUNG
  -march=rv32imc_zicsr_zifencei -mabi=ilp32

DATEIEN
  instruction_mem.bin   Instruktionsspeicher (.init + .text)
  data_mem.bin          Datenspeicher (.rodata + .data + .sdata)
  expected_output.txt   Erwartete Programmausgabe eines korrekten Emulators
  testprog.elf          ELF-Datei zum Inspizieren mit objdump
  src/                  Quellcode zum Selbst-Uebersetzen (main.c, start.S,
                        link.ld, io.c, io.h, Makefile)

AUFRUF DES EMULATORS
  ./hu-risc-v_emu instruction_mem.bin data_mem.bin

ASSEMBLER ANSEHEN (komprimierte Befehle als 4-stellige Hex)
  riscv32-unknown-elf-objdump -d testprog.elf
  riscv32-unknown-elf-objdump -d -M no-aliases testprog.elf   # c.*-Mnemonics

SELBST-UEBERSETZEN (optional)
  cd src && make
  Erzeugt testprog.elf, instruction_mem.bin, data_mem.bin und testprog.lst.
  Zusaetzlich:
    make verify-compressed   # listet alle emittierten c.*-Befehle
  Die fertigen .bin-Dateien liegen bereits vor; der Build ist nur zur
  Nachvollziehbarkeit gedacht.

SELBST-VERIFIKATION
  ./hu-risc-v_emu instruction_mem.bin data_mem.bin > meine_ausgabe.txt
  diff expected_output.txt meine_ausgabe.txt
  Erwartete Schlusszeile: "19 von 19 Tests bestanden".
