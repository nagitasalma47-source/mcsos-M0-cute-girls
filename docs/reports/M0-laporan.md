# Laporan Praktikum M0 — MCSOS 260502

## 1. Sampul
Nama Kelompok: Cute Girls

Anggota:
1. Neng Nagita Salma — 25832071004  
2. Anisa Nur Azfa — 25832072003  
3. Lailatul Zulfa — 25832072001  

Kelas: 1A

## 2. Tujuan
Menyiapkan environment pengembangan OS menggunakan WSL2, toolchain, dan QEMU.

## 3. Dasar Teori
WSL2 digunakan sebagai environment Linux. Clang digunakan untuk cross-compilation. ELF adalah format object file. QEMU digunakan sebagai emulator.

## 4. Lingkungan
Metadata toolchain dicatat pada build/meta/toolchain-versions.txt.

## 5. Desain Baseline
Repository terdiri dari docs/, tools/, smoke/, dan build/.

## 6. Langkah Kerja
- Menjalankan tools/check_env.sh
- Menjalankan make smoke
- Verifikasi object dengan readelf, objdump, dan file
- Verifikasi QEMU dan OVMF

## 7. Hasil Uji
| Pengujian | Command | Hasil | Status |
|---|---|---|---|
| Toolchain | bash tools/check_env.sh | Semua tool terdeteksi | PASS |
| Smoke test | make smoke | ELF64 x86-64 relocatable | PASS |
| QEMU | qemu-system-x86_64 --version | QEMU terdeteksi | PASS |
| Git | git log --oneline | Commit tersedia | PASS |

## 8. Analisis
M0 berhasil menyiapkan baseline environment.

## 9. Keamanan dan Reliability
Repo berada di filesystem Linux WSL dan toolchain tercatat.

## 10. Kesimpulan
M0 berhasil dan siap lanjut ke M1. Belum boot kernel.

## 11. Lampiran
- Output check_env
- Output make smoke
- Output readelf
- Commit log
