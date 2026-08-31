#
# Standalone-Makefile fuer das Testprogramm "04_rv32imc".
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

ARCH    := -march=rv32imc_zicsr_zifencei
ABI     := -mabi=ilp32

CFLAGS  := $(ARCH) $(ABI) -Wall -Wextra -O0 -ffreestanding -fno-builtin \
           -std=gnu99 -Werror=implicit-function-declaration
LDFLAGS := $(ARCH) $(ABI) -nostartfiles -T link.ld

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

# Zusatzziel: zaehlt die im ELF tatsaechlich emittierten verschiedenen
# komprimierten (16-Bit-)Befehle. Dient der Verifikation, dass die
# C-Erweiterung aktiv ist (Akzeptanzkriterium: >= 20 verschiedene c.*-Befehle).
.PHONY: verify-compressed
verify-compressed: testprog.elf
	@echo "Verschiedene komprimierte (c.*) Befehle im ELF:"
	@$(OBJDUMP) -d -M no-aliases testprog.elf \
		| grep -oE '\bc\.[a-z0-9.]+' | sort -u | tr '\n' ' '; echo
	@n=$$($(OBJDUMP) -d -M no-aliases testprog.elf | grep -oE '\bc\.[a-z0-9.]+' | sort -u | wc -l); \
	echo "Anzahl verschiedener c.*-Befehle: $$n"; \
	if [ "$$n" -ge 20 ]; then echo ">>> OK: >= 20 C-Befehle"; \
	else echo ">>> ZU WENIG: < 20 C-Befehle"; exit 1; fi
