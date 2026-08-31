#
# Standalone-Makefile fuer Meilenstein 2 (Erste Schritte).
#
# Erzeugt die instruction_mem.bin (56 Byte) und data_mem.bin (16 Nullbytes)
# direkt aus dem Assemblerquelltext main.S. Aufruf:  make
#
.PHONY: all clean

# Toolchain wird ueber die PATH-Variable gefunden.
TOOLPREFIX := riscv32-unknown-elf-
CC      := $(TOOLPREFIX)gcc
OBJCOPY := $(TOOLPREFIX)objcopy
OBJDUMP := $(TOOLPREFIX)objdump

ARCH    := -march=rv32i_zicsr_zifencei
ABI     := -mabi=ilp32

ASFLAGS := $(ARCH) $(ABI)
LDFLAGS := $(ARCH) $(ABI) -nostartfiles -nostdlib -T link.ld

all: testprog.elf instruction_mem.bin data_mem.bin testprog.lst

main.o: main.S
	$(CC) -c -o $@ $(ASFLAGS) $<

testprog.elf: main.o link.ld
	$(CC) -o $@ $(LDFLAGS) main.o

instruction_mem.bin: testprog.elf
	$(OBJCOPY) -O binary -j .text $< $@

data_mem.bin:
	head -c 16 /dev/zero > $@

testprog.lst: testprog.elf
	$(OBJDUMP) -d $< > $@

clean:
	-$(RM) main.o testprog.elf instruction_mem.bin data_mem.bin testprog.lst
