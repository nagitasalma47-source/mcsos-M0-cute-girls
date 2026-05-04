# Threat Model Awal MCSOS 260502 — M0

## Assets
| Asset | Alasan dilindungi |
|---|---|
| Source code repository | Menentukan perilaku kernel dan tools. |
| Toolchain | Compiler/linker yang salah dapat menghasilkan artefak salah. |
| Build scripts | Script dapat menyisipkan flag berbahaya atau target salah. |
| Documentation baseline | Menjadi sumber requirement dan acceptance criteria. |
| Generated artifacts | Image/log/map dapat menjadi bukti penilaian. |
| Signing keys masa depan | Belum dibuat pada M0, tetapi harus direncanakan. |

## Actors
| Actor | Capability |
|---|---|
| Mahasiswa | Mengubah repository dan menjalankan build. |
| Anggota kelompok | Mengubah branch dan dokumen. |
| Dosen/asisten | Melakukan review dan penilaian. |
| Dependency eksternal | Menyediakan paket, source, dan tools. |
| Malicious local process | Dapat memodifikasi file jika permission buruk. |

## Trust boundaries
1. Windows host ↔ WSL Linux environment.
2. Repository source ↔ generated build output.
3. Package manager ↔ toolchain lokal.
4. Script praktikum ↔ shell pengguna.
5. QEMU guest masa depan ↔ host environment.

## Initial threats and mitigations
| Threat | Dampak | Mitigasi M0 |
|---|---|---|
| Repository ditempatkan di /mnt/c | Build tidak reproducible | Pindah ke ~/src/mcsos |
| Compiler tanpa target | Object salah ABI | Pakai --target dan readelf |
| Tool tidak tercatat | Tidak bisa audit | Simpan metadata |
| Script sembarangan | Supply-chain risk | Pakai source resmi |
| Klaim berlebihan | Penilaian salah | Pakai evidence |
| Kurang koordinasi tim | Integrasi gagal | Dokumentasi jelas |

## Out of scope M0
1. Secure Boot
2. TPM
3. Kernel security
4. Syscall fuzzing
