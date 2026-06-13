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
SRC_C += fs/mcsfs1/mcsfs1.c

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



# ===== M8 Kernel Heap =====

M8_BUILD_DIR := build/m8

.PHONY: m8-clean m8-kmem-host-test m8-kmem-freestanding m8-audit m8-all

m8-clean:
> rm -rf $(M8_BUILD_DIR)

$(M8_BUILD_DIR):
> mkdir -p $(M8_BUILD_DIR)

m8-kmem-freestanding: | $(M8_BUILD_DIR)
> clang -std=c17 -Wall -Wextra -Werror -Iinclude -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -c kernel/mm/kmem.c -o $(M8_BUILD_DIR)/kmem.freestanding.o

m8-kmem-host-test: | $(M8_BUILD_DIR)
> clang -std=c17 -Wall -Wextra -Werror -Iinclude tests/test_kmem.c kernel/mm/kmem.c -o $(M8_BUILD_DIR)/m8_kmem_host_test
> $(M8_BUILD_DIR)/m8_kmem_host_test

m8-audit:
> nm -u $(M8_BUILD_DIR)/kmem.freestanding.o

m8-all: m8-clean m8-kmem-freestanding m8-kmem-host-test m8-audit


# ===== M10 Syscall =====

M10_CFLAGS := -std=c17 -Wall -Wextra -Werror -Iinclude
M10_KERNEL_CFLAGS := $(M10_CFLAGS) -Ikernel/include -target x86_64-elf -ffreestanding -fno-stack-protector -fno-builtin -mno-red-zone -O2 -g
M10_HOST_CFLAGS := $(M10_CFLAGS) -O2 -g -DMCSOS_HOST_TEST

build/test_syscall_host: tests/test_syscall_host.c kernel/syscall/syscall.c include/mcsos/syscall.h
> mkdir -p build
> $(HOSTCC) $(M10_HOST_CFLAGS) tests/test_syscall_host.c kernel/syscall/syscall.c -o build/test_syscall_host

build/syscall.o: kernel/syscall/syscall.c include/mcsos/syscall.h
> mkdir -p build
> $(CC) $(M10_KERNEL_CFLAGS) -c kernel/syscall/syscall.c -o build/syscall.o

build/syscall_entry.o: kernel/arch/x86_64/syscall_entry.S
> mkdir -p build
> $(CC) -target x86_64-elf -c kernel/arch/x86_64/syscall_entry.S -o build/syscall_entry.o

build/m10_syscall_combined.o: build/syscall.o build/syscall_entry.o
> ld -r build/syscall.o build/syscall_entry.o -o build/m10_syscall_combined.o

m10-host-test: build/test_syscall_host
> ./build/test_syscall_host

m10-audit: build/m10_syscall_combined.o
> $(NM) -u build/m10_syscall_combined.o > build/nm_undefined.txt
> $(READELF) -h build/m10_syscall_combined.o > build/readelf_header.txt
> $(OBJDUMP) -dr build/m10_syscall_combined.o > build/objdump.txt
> sha256sum build/test_syscall_host build/m10_syscall_combined.o > build/SHA256SUMS
> grep -q "x86_64_syscall_int80_stub" build/objdump.txt


.PHONY: iso
iso: build
> cp build/mcsos-m5.elf iso_root/boot/kernel.elf
> xorriso -as mkisofs \
	-b boot/limine/limine-bios-cd.bin \
	-no-emul-boot -boot-load-size 4 -boot-info-table \
	--efi-boot boot/limine/limine-uefi-cd.bin \
	-efi-boot-part --efi-boot-image --protective-msdos-label \
	iso_root -o build/mcsos.iso
> limine/limine bios-install build/mcsos.iso
> sha256sum build/mcsos.iso > build/mcsos.iso.sha256
> @echo "[ISO] build/mcsos.iso ready"

HOST_CFLAGS := -std=c17 -Wall -Wextra -Werror -O2 -g
FREESTANDING_CFLAGS := -target x86_64-elf -std=c17 -ffreestanding -fno-builtin -fno-stack-protector -fno-pic -mno-red-zone -Wall -Wextra -Werror -O2 -g

.PHONY: m15-all

m15-all: artifacts/m15/test_mcsfs1 artifacts/m15/mcsfs1.o artifacts/m15/mcsfs1.rel.o
> ./artifacts/m15/test_mcsfs1 | tee artifacts/m15/host_test.txt
> nm -u artifacts/m15/mcsfs1.rel.o | tee artifacts/m15/nm_undefined.txt
> test ! -s artifacts/m15/nm_undefined.txt
> readelf -h artifacts/m15/mcsfs1.rel.o | tee artifacts/m15/readelf_header.txt
> objdump -dr artifacts/m15/mcsfs1.rel.o | tee artifacts/m15/objdump.txt >/dev/null
> sha256sum artifacts/m15/* | tee artifacts/m15/SHA256SUMS.txt

artifacts/m15/test_mcsfs1: tests/m15/test_mcsfs1.c fs/mcsfs1/mcsfs1.c fs/mcsfs1/mcsfs1.h
> mkdir -p artifacts/m15
> $(CC) $(HOST_CFLAGS) -I. tests/m15/test_mcsfs1.c fs/mcsfs1/mcsfs1.c -o $@

artifacts/m15/mcsfs1.o: fs/mcsfs1/mcsfs1.c fs/mcsfs1/mcsfs1.h
> mkdir -p artifacts/m15
> $(CC) $(FREESTANDING_CFLAGS) -I. -c fs/mcsfs1/mcsfs1.c -o $@

artifacts/m15/mcsfs1.rel.o: artifacts/m15/mcsfs1.o
> ld -r $< -o $@

