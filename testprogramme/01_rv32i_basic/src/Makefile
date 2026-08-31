#
# Standalone-Makefile fuer das Testprogramm "01_rv32i_basic".
#
# Alle Quellen liegen im selben Verzeichnis -- es gibt keine externen
# Abhaengigkeiten. Aufruf:  make
#
.PHONY: all clean

# Toolchain wird ueber die PATH-Variable gefunden.
TOOLPREFIX := riscv32-unknown-elf-
CC      := $(TOOLPREFIX)gcc
OBJCOPY := $(TOOLPREFIX)objcopy
OBJDUMP := $(TOOLPREFIX)objdump
SIZE    := $(TOOLPREFIX)size

ARCH    := -march=rv32i_zicsr_zifencei
ABI     := -mabi=ilp32

CFLAGS  := $(ARCH) $(ABI) -Wall -Wextra -O0 -ffreestanding -fno-builtin \
           -std=gnu99 -Werror=implicit-function-declaration
# --no-relax: verhindert, dass die Linker-Relaxation 32-Bit-Befehle der
# eingelinkten libgcc-Soft-Routinen (__udivsi3 usw.) zu komprimierten
# RVC/C-Befehlen umwandelt. Ohne dies enthaelt ein reines RV32I-Programm
# 16-Bit-Befehle, die ein RV32I-Emulator nicht dekodieren kann.
LDFLAGS := $(ARCH) $(ABI) -nostartfiles -T link.ld -Wl,--no-relax

OBJECTS := start.o io.o main.o

all: testprog.elf instruction_mem.bin data_mem.bin testprog.lst

testprog.elf: $(OBJECTS) link.ld
	$(CC) -o $@ $(LDFLAGS) -Wl,--Map,testprog.map $(OBJECTS)
	$(SIZE) $@

instruction_mem.bin: testprog.elf
	$(OBJCOPY) -O binary -j .init -j .text $< $@

data_mem.bin: testprog.elf
	$(OBJCOPY) -O binary -j .rodata -j .data -j .sdata $< $@

testprog.lst: testprog.elf
	$(OBJDUMP) -d $< > $@

%.o: %.c io.h
	$(CC) -c -o $@ $(CFLAGS) $<

%.o: %.S
	$(CC) -c -o $@ $(CFLAGS) $<

clean:
	-$(RM) $(OBJECTS) testprog.elf testprog.map instruction_mem.bin \
	       data_mem.bin testprog.lst
