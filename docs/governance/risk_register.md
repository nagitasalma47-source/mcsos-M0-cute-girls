# Risk Register MCSOS 260502 — M0

| ID | Risiko | Probabilitas | Dampak | Mitigasi | Owner | Trigger review |
|---|---|---:|---:|---|---|---|
| R-M0-001 | WSL bukan versi 2 | Medium | High | Cek wsl --list --verbose | Toolchain | VERSION != 2 |
| R-M0-002 | Repo di /mnt/c | High | Medium | Pindah ke ~/src/mcsos | Koordinator | pwd menunjukkan /mnt/c |
| R-M0-003 | QEMU tidak ada | Medium | High | Install qemu-system-x86 | Toolchain | command -v gagal |
| R-M0-004 | OVMF tidak ditemukan | Medium | Medium | Cari dengan find /usr/share | Toolchain | file tidak ada |
| R-M0-005 | Compiler target salah | Medium | High | Pakai --target | Verification | readelf salah |
| R-M0-006 | Requirement tidak jelas | Medium | Medium | Buat verification matrix | Documentation | tidak ada evidence |
| R-M0-007 | Konflik branch | Medium | Medium | Pull sebelum commit | Koordinator | konflik git |
| R-M0-008 | Error dihapus | Medium | Medium | Wajib laporan error | Semua | error hilang |
| R-M0-009 | Versi tool tidak dicatat | Medium | High | Jalankan make meta | Verification | metadata kosong |
| R-M0-010 | Scope melebar | Medium | Medium | Ikuti non-goals | Koordinator | ada kernel terlalu awal |
