# Laporan Praktikum M0 — MCSOS 260502

## 1. Sampul
Nama: (isi)
NIM: (isi)
Kelas: (isi)

## 2. Tujuan
Menyiapkan environment pengembangan OS menggunakan WSL2, toolchain, dan QEMU.

## 3. Dasar Teori
WSL2 digunakan sebagai environment Linux.
Clang digunakan untuk cross-compilation.
ELF adalah format object file.
QEMU digunakan sebagai emulator.

## 4. Lingkungan
(Lihat build/meta/toolchain-versions.txt)

## 5. Desain Baseline
Repository terdiri dari:
- docs/
- tools/
- smoke/
- build/

## 6. Langkah Kerja
- Menjalankan tools/check_env.sh
- Menjalankan make smoke
- Verifikasi dengan readelf
- Verifikasi QEMU

## 7. Hasil Uji

| Pengujian | Command | Hasil | Status |
|----------|--------|------|--------|
| Toolchain | bash tools/check_env.sh | Semua OK | PASS |
| Smoke test | make smoke | ELF64 x86-64 | PASS |
| QEMU | qemu-system-x86_64 --version | Terdeteksi | PASS |
| Git | git log | Commit ada | PASS |

## 8. Analisis
Semua proses berjalan dengan baik. Tidak ditemukan error kritis.

## 9. Keamanan
- Repo tidak di /mnt/c
- Toolchain tercatat
- Tidak menggunakan script tidak jelas

## 10. Failure Mode
Tidak ditemukan kegagalan signifikan selama M0.

## 11. Kesimpulan
M0 berhasil menyiapkan environment.
Belum menjalankan kernel (sesuai scope).

## 12. Lampiran
- Output check_env
- Output make smoke
- Commit log

