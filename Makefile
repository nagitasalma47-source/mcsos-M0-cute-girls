HOSTCC ?= cc
.RECIPEPREFIX := >
SHELL := /usr/bin/env bash

BUILD_DIR := build

KERNEL := $(BUILD_DIR)/mcsos-m5.elf
PANIC_KERNEL := $(BUILD_DIR)/mcsos-m5.panic.elf

MAP := $(BUILD_DIR)/mcsos-m5.map
PANIC_MAP := $(BUILD_DIR)/mcsos-m5.panic.map

DISASM := $(BUILD_DIR)/disassembly.txt
SYMS := $(BUILD_DIR)/symbols.txt

CC := clang
LD := ld.lld
OBJDUMP := objdump
READELF := readelf
NM := nm

all: build inspect

build: $(KERNEL)

COMMON_CFLAGS := --target=x86_64-unknown-none-elf -std=c17 -ffreestanding -fno-builtin -fno-stack-protector -fno-stack-check -fno-pic -fno-pie -fno-lto -m64 -march=x86-64 -mabi=sysv -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mcmodel=kernel -Wall -Wextra -Werror -Ikernel/arch/x86_64/include -Ikernel/include -Iinclude

CFLAGS := $(COMMON_CFLAGS)

PANIC_CFLAGS := $(COMMON_CFLAGS) -DMCSOS_M3_TRIGGER_PANIC=1

LDFLAGS := -nostdlib -static -z max-page-size=0x1000 -T linker.ld

SRC_C := $(shell find kernel -name '*.c' | LC_ALL=C sort)
SRC_C += src/vmm.c

OBJ := $(patsubst %.c,$(BUILD_DIR)/normal/%.o,$(SRC_C))
PANIC_OBJ := $(patsubst %.c,$(BUILD_DIR)/panic/%.o,$(SRC_C))

SRC_S := $(shell find kernel -name '*.S' | LC_ALL=C sort)

OBJ += $(patsubst %.S,$(BUILD_DIR)/normal/%.o,$(SRC_S))
PANIC_OBJ += $(patsubst %.S,$(BUILD_DIR)/panic/%.o,$(SRC_S))

.PHONY: all build panic inspect audit clean distclean

panic: $(PANIC_KERNEL)

$(BUILD_DIR)/normal/%.o: %.S

> mkdir -p $(dir $@)
> $(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/panic/%.o: %.S

> mkdir -p $(dir $@)
> $(CC) $(PANIC_CFLAGS) -c $< -o $@

$(BUILD_DIR)/normal/%.o: %.c

> mkdir -p $(dir $@)
> $(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/panic/%.o: %.c

> mkdir -p $(dir $@)
> $(CC) $(PANIC_CFLAGS) -c $< -o $@

$(KERNEL): $(OBJ) linker.ld

> mkdir -p $(BUILD_DIR)
> $(LD) $(LDFLAGS) -Map=$(MAP) -o $@ $(OBJ)

$(PANIC_KERNEL): $(PANIC_OBJ) linker.ld

> mkdir -p $(BUILD_DIR)
> $(LD) $(LDFLAGS) -Map=$(PANIC_MAP) -o $@ $(PANIC_OBJ)

inspect: $(KERNEL)

> $(READELF) -h $(KERNEL) > $(BUILD_DIR)/readelf-header.txt
> $(READELF) -S $(KERNEL) > $(BUILD_DIR)/readelf-sections.txt
> $(READELF) -l $(KERNEL) > $(BUILD_DIR)/readelf-program-headers.txt
> $(NM) -n $(KERNEL) > $(SYMS)
> $(NM) -u $(KERNEL) > $(BUILD_DIR)/undefined.txt
> $(OBJDUMP) -d -Mintel $(KERNEL) > $(DISASM)

> grep -q 'ELF64' $(BUILD_DIR)/readelf-header.txt
> grep -q 'Machine:[[:space:]]*Advanced Micro Devices X86-64' $(BUILD_DIR)/readelf-header.txt
> grep -q 'kmain' $(SYMS)
> grep -q 'kernel_panic_at' $(SYMS)
> grep -q 'cpu_halt_forever' $(DISASM)

audit: inspect panic

> ! $(NM) -u $(KERNEL) | grep .
> ! $(NM) -u $(PANIC_KERNEL) | grep .
> grep -q 'kernel_panic_at' $(DISASM)
> $(READELF) -S $(KERNEL) | grep -q '.text'
> $(READELF) -S $(KERNEL) | grep -q '.rodata'

grade: all

> grep -q 'isr_stub_32' $(SYMS)
> grep -q 'pic_remap' $(SYMS)
> grep -q 'pit_configure_hz' $(SYMS)
> grep -q 'timer_on_irq0' $(SYMS)
> grep -q 'x86_64_trap_dispatch' $(SYMS)
> @echo "M5 static grade: PASS"

clean:

> rm -rf $(BUILD_DIR)

distclean: clean

> rm -rf iso_root limine



M6_CFLAGS := -std=c17 -Wall -Wextra -Werror -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -Iinclude

build/pmm.o: src/pmm.c include/pmm.h include/types.h
> mkdir -p build
> $(CC) $(M6_CFLAGS) -c src/pmm.c -o build/pmm.o

build/test_pmm_host: src/pmm.c tests/test_pmm_host.c include/pmm.h include/types.h
> mkdir -p build
> $(HOSTCC) -std=c17 -Wall -Wextra -Werror -Iinclude src/pmm.c tests/test_pmm_host.c -o build/test_pmm_host

check-m6: build/pmm.o build/test_pmm_host
> ./build/test_pmm_host
> nm -u build/pmm.o | tee build/pmm.undefined.txt
> test ! -s build/pmm.undefined.txt
> objdump -dr build/pmm.o > build/pmm.objdump.txt

qemu:
> qemu-system-x86_64 -kernel $(KERNEL) -serial stdio -no-reboot -no-shutdown

run-qemu-gdb:
> qemu-system-x86_64 -cdrom build/mcsos.iso -serial stdio -s -S

M7_CFLAGS := -std=c17 -Wall -Wextra -Werror -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -Iinclude
M7_HOST_CFLAGS := -std=c17 -Wall -Wextra -Werror -DMCSOS_HOST_TEST -Iinclude

build/vmm.o: src/vmm.c include/vmm.h include/types.h
> mkdir -p build
> $(CC) $(M7_CFLAGS) -c src/vmm.c -o build/vmm.o

build/test_vmm_host: src/vmm.c tests/test_vmm_host.c include/vmm.h include/types.h
> mkdir -p build
> $(HOSTCC) $(M7_HOST_CFLAGS) src/vmm.c tests/test_vmm_host.c -o build/test_vmm_host

check-m7: build/vmm.o build/test_vmm_host
> ./build/test_vmm_host
> nm -u build/vmm.o | tee build/vmm.undefined.txt
> test ! -s build/vmm.undefined.txt
> objdump -dr build/vmm.o > build/vmm.objdump.txt
> grep -q "invlpg" build/vmm.objdump.txt
> grep -q "cr3" build/vmm.objdump.txt
> @echo "M7 VMM host tests PASS"

