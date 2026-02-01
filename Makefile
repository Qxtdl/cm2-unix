
ARCH ?= riscv

TOOLCHAIN ?= riscv64-unknown-elf

ROOT ?= .

#source files
CSRCS = $(wildcard $(ROOT)/fs/*.c) \
	   $(wildcard $(ROOT)/kernel/*.c) \
	   $(wildcard $(ROOT)/arch/$(ARCH)/*.c) \
	   $(wildcard $(ROOT)/lib/*.c)

ASRCS = $(wildcard $(ROOT)/arch/$(ARCH)/*.S)

#include options
INCL ?= -I$(ROOT)/include

#linker file
LNKF = $(ROOT)/arch/$(ARCH)/linker.ld

# link path options
LNKP ?=

MN_FILE ?= main.elf

# IF debugging stack faliures:
# Uncomment below code and switch -O2 flag to -Os

OPTIMIZATION = -O2

CFLAGS ?= -march=rv32i -mabi=ilp32 -ffreestanding $(OPTIMIZATION) -Wall -Wextra -Wno-unused-parameter $(INCL) \
	-g -fstack-protector-all -fverbose-asm

LDFLAGS ?= -nostdlib -nostartfiles -static -march=rv32i -mabi=ilp32 $(OPTIMIZATION)

OBJS = $(CSRCS:%.c=%.o) $(ASRCS:%.S=%.o)

CC = $(TOOLCHAIN)-gcc 
OBJCOPY = $(TOOLCHAIN)-objcopy
READELF = $(TOOLCHAIN)-readelf

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

$(MN_FILE): $(OBJS)
	$(CC) $(LDFLAGS) -T $(LNKF) $(OBJS) -o $@

image: $(MN_FILE)
	$(OBJCOPY) -O binary $(MN_FILE) image.bin
	python $(ROOT)/arch/$(ARCH)/$(ARCH)_encoder.py image.bin

size:
	$(READELF) -S $(MN_FILE)

dump:
	$(TOOLCHAIN)-objdump -d -M no-aliases main.elf >> dump.s
	$(TOOLCHAIN)-objdump -S -d -M no-aliases main.elf >> verbose_dump.s

clean:
	rm -f $(OBJS) $(MN_FILE) image.bin
	rm -rf dump.s
	rm -rf verbose_dump.s
	rm -rf preprocessed.c

rebuild: clean image size


