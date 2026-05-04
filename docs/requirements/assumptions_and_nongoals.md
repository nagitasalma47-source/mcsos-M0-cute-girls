# Assumptions and Non-Goals MCSOS 260502 — M0

## Assumptions
1. Target arsitektur awal adalah x86_64 long mode.
2. Host pengembangan adalah Windows 11 x64.
3. Build dilakukan di WSL 2 Linux environment.
4. Repository utama berada di filesystem Linux WSL.
5. Emulator utama untuk milestone awal adalah QEMU system x86_64.
6. Firmware emulator untuk jalur boot awal adalah OVMF/UEFI.
7. Bootloader awal yang direkomendasikan untuk milestone boot adalah Limine atau setara.
8. Bahasa kernel awal adalah freestanding C17 dengan assembly minimal.
9. Compatibility target awal adalah POSIX-like subset.
10. Setiap milestone harus menghasilkan bukti (log, output, dll).

## Non-goals M0
1. M0 tidak membuat kernel bootable.
2. M0 tidak mengimplementasikan bootloader.
3. M0 tidak membuat linker script final.
4. M0 tidak mengimplementasikan fitur OS (interrupt, paging, scheduler, dll).
5. M0 tidak mengklaim siap produksi.
6. M0 tidak menjamin kompatibilitas semua mesin.
7. M0 tidak melakukan hardware bring-up.
8. M0 tidak wajib reproducible build penuh.
