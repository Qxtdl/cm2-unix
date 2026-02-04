
ARCH ?= riscv

TOOLCHAIN ?= riscv64-unknown-elf

ROOT ?= $(PWD)

RAYLIB ?= true
EMULATOR ?= $(ROOT)/emulator/riscv/cm2-riscv-emulator

# the filesystems
FS_SELECT = $(ROOT)/fs/vfs.c $(ROOT)/fs/romfs.c

#source files
CSRCS = $(FS_SELECT) \
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
# make DEBUG=true

DEBUG ?= false

CFLAGS = -march=rv32i -mabi=ilp32 -ffreestanding -Wall -Wextra -Wno-unused-parameter -g -fverbose-asm $(INCL)
ASFLAGS = $(CFLAGS)
LDFLAGS = -nostdlib -nostartfiles -static -march=rv32i -mabi=ilp32


ifeq ($(DEBUG), true)
	CFLAGS += -g -fstack-protector-all -fverbose-asm -O2 -D__DEBUG__
	LDFLAGS += -O2
else
	CFLAGS += -Os -flto
	LDFLAGS += -Os -flto
	ASFLAGS += -fno-lto
endif


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
	/bin/env python3 $(ROOT)/scripts/$(ARCH)_encoder.py image.bin

run: image.bin
	$(MAKE) -C $(EMULATOR) run ROOT="$(EMULATOR)" OUTPUT_ARGS="$(ROOT)/image.bin $(EMULATOR)/emulator-tilesheet/minesweeper.bmp $(EMULATOR)/disk_dump.bin"

tools:
	$(MAKE) -C $(EMULATOR) all ROOT="$(EMULATOR)" RAYLIB="$(RAYLIB)" OPTIMIZE=true

size:
	$(READELF) -S $(MN_FILE)

dump:
	$(TOOLCHAIN)-objdump -d -M no-aliases main.elf >> dump.s.dump
	$(TOOLCHAIN)-objdump -S -d -M no-aliases main.elf >> verbose_dump.s.dump

clean:
	rm -f $(OBJS) $(MN_FILE) image.bin
	rm -rf dump.s.dump
	rm -rf verbose_dump.s.dump

rebuild: clean image size


