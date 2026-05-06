# Readiness Review M1 - Toolchain Reproducible

## Identitas

- Nama mahasiswa/kelompok:
  - Neng Nagita Salma
  - Anisa Nur Azfa
  - Lailatul Zulfa
- NIM anggota:
  - 25832071004
  - 25832072003
  - 25832072001
- Kelas: PTI 1A
- Dosen: Muhaemin Sidiq, S.Pd., M.Pd.
- Program Studi: Pendidikan Teknologi Informasi, Institut Pendidikan Indonesia
- Tanggal: 2026-05-06
- Commit hash: belum diisi

## Ringkasan hasil

Praktikum M1 berhasil menyelesaikan setup dan validasi lingkungan pengembangan MCSOS pada WSL2 Ubuntu 24.04. Seluruh toolchain utama berhasil terdeteksi dan tervalidasi, termasuk compiler, linker, debugger, QEMU, dan OVMF. Proof freestanding ELF64 x86_64 berhasil dibuat tanpa undefined symbol dan reproducibility check menunjukkan hash yang konsisten. Repository berada pada filesystem Linux WSL dan seluruh acceptance criteria M1 terpenuhi sehingga lingkungan dinyatakan siap untuk M2.

## Evidence checklist

| Evidence | Path | Status | Catatan |
|---|---|---|---|
| Toolchain versions | `build/meta/toolchain-versions.txt` | OK | Toolchain berhasil dicatat |
| Host readiness | `build/meta/host-readiness.txt` | OK | CPU dan memory tervalidasi |
| QEMU capabilities | `build/meta/qemu-capabilities.txt` | OK | q35 dan OVMF tersedia |
| Freestanding object | `build/proof/freestanding_probe.o` | OK | ELF relocatable berhasil dibuat |
| Freestanding ELF | `build/proof/freestanding_probe.elf` | OK | ELF executable berhasil dibuat |
| ELF header | `build/proof/readelf-header.txt` | OK | ELF64 x86_64 tervalidasi |
| ELF sections | `build/proof/readelf-sections.txt` | OK | Section ELF berhasil dianalisis |
| Disassembly | `build/proof/objdump-disassembly.txt` | OK | Disassembly berhasil dibuat |
| Undefined symbol report | `build/proof/nm-undefined.txt` | OK | Tidak ada undefined symbol |
| Reproducibility hash | `build/repro/sha256-run1.txt`, `build/repro/sha256-run2.txt` | OK | Hash build konsisten |

## Acceptance criteria M1

| Kriteria | Lulus/Gagal | Bukti |
|---|---|---|
| Repository berada di filesystem Linux WSL | Lulus | `pwd`, `check_toolchain.sh` |
| Semua tool wajib tersedia | Lulus | `check_toolchain.sh` |
| `make meta` berhasil | Lulus | metadata generated |
| `make check` berhasil | Lulus | toolchain validation |
| `make proof` berhasil | Lulus | freestanding ELF generated |
| `make qemu-probe` berhasil | Lulus | q35 dan OVMF detected |
| `make repro` berhasil | Lulus | reproducible hash verified |
| `make test` berhasil dari clean checkout | Lulus | `OK: M1 test suite passed` |
| `nm-undefined.txt` kosong | Lulus | no undefined symbol |
| Hasil `readelf` menunjukkan ELF64 x86_64 | Lulus | `readelf-header.txt` |

## Known limitations

1. Belum menggunakan cross compiler khusus seperti `x86_64-elf-gcc`.
2. Belum terdapat CI/CD pipeline otomatis.
3. Belum ada boot image kernel MCSOS.
4. Belum dilakukan hardware boot test langsung selain validasi QEMU.
5. Belum terdapat automated integration test.

## Risiko dan mitigasi

1. Risiko salah target architecture pada compiler. Mitigasi: validasi menggunakan `readelf` dan target triple.
2. Risiko repository corruption akibat filesystem Windows. Mitigasi: repository disimpan di filesystem Linux WSL.
3. Risiko dependency toolchain berubah tanpa dokumentasi. Mitigasi: seluruh versi tool dicatat pada metadata readiness.

## Readiness decision

- [ ] Belum siap lanjut M2.
- [ ] Siap lanjut M2 dengan catatan.
- [x] Siap lanjut M2.

Alasan keputusan:

Seluruh acceptance criteria M1 berhasil dipenuhi, seluruh script validasi berjalan sukses, reproducibility proof konsisten, dan lingkungan pengembangan dinyatakan stabil untuk melanjutkan ke M2.
