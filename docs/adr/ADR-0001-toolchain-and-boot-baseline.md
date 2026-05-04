# ADR-0001 — Toolchain dan Boot Baseline MCSOS 260502

## Status
Accepted for M0 baseline.

## Context
MCSOS dikembangkan pada host Windows 11 x64, tetapi targetnya adalah bare metal x86_64.
Program kernel tidak boleh bergantung pada ABI host.
Lingkungan build harus dapat direproduksi.

## Decision
1. Build environment menggunakan WSL 2 Linux.
2. Repository berada di filesystem Linux, bukan /mnt/c.
3. Toolchain awal menggunakan Clang/LLVM dan binutils.
4. Smoke test menggunakan clang --target=x86_64-unknown-none.
5. Emulator menggunakan QEMU x86_64.
6. Firmware menggunakan OVMF (UEFI).
7. Bootloader awal direkomendasikan Limine.
8. GCC cross-compiler bersifat opsional di M0.

## Consequences

### Keuntungan
- Setup seragam
- Toolchain mudah diinstall
- Workflow OS dev standar
- Reproducible environment

### Trade-off
- WSL punya boundary VM
- KVM bisa tidak optimal
- Versi tool bisa beda → perlu metadata

## Review Trigger
- Arsitektur berubah
- Toolchain berubah
- Bootloader diganti
- CI diperkenalkan
