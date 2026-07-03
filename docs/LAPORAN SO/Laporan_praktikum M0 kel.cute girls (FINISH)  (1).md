# Laporan Praktikum Sistem Operasi Lanjut — MCSOS

**Nama file laporan:** `laporan_praktikum_M0_cute girls.md`  
**Nama sistem operasi:** MCSOS 260502  
**Target default:** x86_64, QEMU, Windows 11 x64 + WSL 2, kernel monolitik pendidikan, C freestanding dengan assembly minimal, POSIX-like subset  
**Dosen:** Muhaemin Sidiq, S.Pd., M.Pd.  
**Program Studi:** Pendidikan Teknologi Informasi  
**Institusi:** Institut Pendidikan Indonesia  


---

## 0. Metadata Laporan

| Atribut | Isi |
|---|---|
| Kode praktikum | `M0` |
| Judul praktikum | `Baseline Requirements, Governance, dan Lingkungan Pengembangan Reproducible MCSOS 260502` |
| Jenis pengerjaan | `Kelompok` |
| Nama kelompok | `cute girls` |
| Anggota kelompok | `Neng Nagita Salma (25832071004) — Anisa Nur Azfa (25832072003) — Lailatul Zulfa (25832072001)` |
| Kelas | `1A` |
| Tanggal praktikum | `2026-05-04` |
| Tanggal pengumpulan | `2026-07-01` |
| Repository | `https://github.com/nagitasalma47-source/mcsos-M0-cute-girls` |
| Branch | `m0/cute-girls` |
| Commit awal | `` 3c4ff72 `` |
| Commit akhir | `` 77e27fe `` |
| Status readiness yang diklaim | `belum siap uji ` |

---

## 1. Sampul

# Laporan Praktikum `M0`  
## `Baseline Requirements, Governance, dan Lingkungan Pengembangan Reproducible MCSOS 260502`

Disusun oleh:

| Nama | NIM | Kelas | Peran |
|---|---|---|---|
| Neng Nagita Salma | 25832071004 | 1A | `dikerjakan bareng` |
| Anisa Nur Azfa | 25832072003 | 1A | `dikerjakan bareng` |
| Lailatul Zulfa | 25832072001 | 1A | `dikerjakan bareng` |

Dosen Pengampu: **Muhaemin Sidiq, S.Pd., M.Pd.**  
Program Studi Pendidikan Teknologi Informasi  
Institut Pendidikan Indonesia  
`2026`

---

## 2. Pernyataan Orisinalitas dan Integritas Akademik

Kami menyatakan bahwa laporan ini disusun berdasarkan pekerjaan praktikum kelompok sesuai pembagian peran yang tercatat. Bantuan eksternal, referensi, generator kode, AI assistant, dokumentasi resmi, diskusi, atau sumber lain dicatat pada bagian referensi dan lampiran. Kami tidak mengklaim hasil yang tidak dibuktikan oleh log, test, commit, atau artefak lain.

| Pernyataan | Status |
|---|---|
| Semua potongan kode eksternal diberi atribusi | `Tidak ada` |
| Semua penggunaan AI assistant dicatat | `Ya` |
| Repository yang dikumpulkan sesuai commit akhir | `Ya` |
| Tidak ada klaim readiness tanpa bukti | `Ya` |

Catatan penggunaan bantuan eksternal:

```text
Alat:
- ChatGPT (AI assistant)

Prompt ringkas:
- Menanyakan cara setup WSL, Git, dan push ke GitHub
- Meminta penjelasan error saat praktikum
- Meminta bantuan penyusunan laporan M0

Sumber:
- Dokumentasi praktikum MCSOS
- ChatGPT

Bagian yang dibantu:
- Penjelasan konsep (repository, Git, WSL)
- Penyelesaian error (EOF, /mnt/c, command terminal)
- Penyusunan laporan dan format tabel

Verifikasi mandiri:
- Menjalankan semua command sendiri di terminal (make check, make smoke, dll)
- Memastikan output sesuai (ELF64 x86-64, toolchain OK)
- Memastikan repository dan GitHub sesuai hasil praktikum.
```

---

## 3. Tujuan Praktikum

Tuliskan tujuan teknis dan konseptual praktikum. Tujuan harus dapat diuji.

1. `Tujuan teknis 1: membangun toolchain reproducible untuk target x86_64-elf`
2. `Tujuan teknis 2: menghasilkan image bootable QEMU dengan serial log`
3. `Tujuan konseptual 1: menjelaskan kontrak boot handoff, linker layout, atau invariant allocator`
4. `Tujuan validasi: menyimpan log build, log QEMU, readelf/objdump evidence, dan test result`

---

## 4. Capaian Pembelajaran Praktikum

Setelah praktikum ini, mahasiswa mampu:

| CPL/CPMK praktikum | Bukti yang harus ditunjukkan |
|---|---|
| Menjelaskan pentingnya lingkungan build yang terisolasi, terdokumentasi, dan reproducible untuk pengembangan sistem operasi | Dokumen readiness review, invariants, assumptions, dan threat model praktikum            |
| Menginstal dan memverifikasi WSL 2 pada Windows 11 x64 sesuai prosedur resmi                                                | Output `wsl --status`, `wsl --list --verbose`, dan screenshot environment WSL2           |
| Menyiapkan distribusi Linux WSL beserta toolchain pengembangan sistem operasi                                               | Output pemeriksaan toolchain, hasil validasi compiler/linker, dan metadata versi tool    |
| Membuat struktur repository awal MCSOS sesuai roadmap pengembangan bertahap                                                 | Struktur direktori repository, branch Git, dan hasil `git status`                        |
| Membuat dokumen baseline requirements, non-goals, assumptions, threat model awal, risk register, dan verification matrix    | File dokumentasi pada direktori `docs/` dan evidence readiness review                    |
| Membuat script validasi environment yang mencatat versi toolchain dan mendeteksi kesalahan konfigurasi                      | Script validasi pada `tools/scripts/` dan output hasil pengecekan environment            |
| Memahami pentingnya evidence teknis seperti log terminal, commit hash, checksum, dan pemeriksaan object file                | Output `git rev-parse HEAD`, SHA-256 artefak, serta hasil `readelf`, `objdump`, dan `nm` |
| Membedakan status siap uji environment, siap uji QEMU, dan siap demonstrasi praktikum berdasarkan evidence teknis           | Bagian readiness review, checkpoint praktikum, dan log validasi environment              |




## 5. Peta Milestone MCSOS

Centang milestone yang menjadi fokus laporan ini. Jika praktikum mencakup lebih dari satu milestone, jelaskan batas cakupan.

| Milestone | Fokus | Status dalam laporan |
|---|---|---|
| M0 | Requirements, governance, baseline arsitektur | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M1 | Toolchain reproducible, Git, QEMU, GDB, metadata build | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M2 | Boot image, kernel ELF64, early console | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M3 | Panic path, linker map, GDB, observability awal | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M4 | Trap, exception, interrupt, timer | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M5 | PMM, VMM, page table, kernel heap | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M6 | Thread, scheduler, synchronization | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M7 | Syscall ABI dan user program loader | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M8 | VFS, file descriptor, ramfs | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M9 | Block layer dan device model | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M10 | Persistent filesystem, mcsfs/ext2-like, recovery | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M11 | Networking stack, packet parsing, UDP/TCP subset | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M12 | Security model, capability/ACL, syscall fuzzing, hardening | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M13 | SMP, scalability, lock stress, NUMA-aware preparation | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M14 | Framebuffer, graphics console, visual regression | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M15 | Virtualization/container subset | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M16 | Observability, update/rollback, release image, readiness review | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
Batas cakupan praktikum:

```text
Praktikum M0 berfokus pada penyiapan baseline pengembangan MCSOS 260502 yang reproducible, meliputi konfigurasi WSL 2, toolchain, struktur repository, smoke test object freestanding, metadata toolchain, serta dokumen governance dan verification baseline. Praktikum ini mencakup validasi environment menggunakan script check_env.sh, pemeriksaan ELF object dengan readelf dan objdump, serta pengelolaan repository menggunakan Git.

Praktikum M0 tidak mencakup implementasi kernel bootable, bootloader, interrupt handler, memory management, scheduler, filesystem, networking, userspace, maupun security enforcement. M0 juga tidak mengklaim sistem operasi siap boot, siap produksi, atau bebas error. Fokus utama M0 hanya memastikan lingkungan pengembangan, proses build, dan evidence engineering siap digunakan untuk milestone berikutnya. :contentReference[oaicite:0]{index=0}
```

---

## 6. Dasar Teori Ringkas

Praktikum M0 menggunakan konsep host, build environment, dan target system. Host yang digunakan adalah Windows 11 x64, sedangkan build environment berada pada Linux WSL 2. Target sistem adalah bare-metal x86_64 yang nantinya dijalankan menggunakan emulator QEMU. Pemisahan antara host dan target penting agar kernel tidak dikompilasi sebagai program Linux biasa. 

WSL 2 digunakan sebagai lingkungan build karena menyediakan kompatibilitas Linux yang lebih stabil untuk toolchain pengembangan sistem operasi. Toolchain yang digunakan meliputi Clang/LLVM, binutils, NASM, QEMU, dan GDB untuk proses kompilasi, inspeksi object, emulasi, dan debugging. 

Konsep cross-compilation digunakan agar compiler menghasilkan object untuk target x86_64 freestanding, bukan executable host Linux. Pada M0 dilakukan smoke test menggunakan flag `--target=x86_64-unknown-none` dan `-ffreestanding` untuk menghasilkan object ELF64 relocatable yang sesuai dengan kebutuhan kernel tahap awal. Pemeriksaan dilakukan menggunakan `readelf` dan `objdump`. 

Git digunakan sebagai version control untuk menjaga traceability setiap perubahan repository. Seluruh commit, log, dan metadata toolchain dicatat sebagai bagian dari evidence-first engineering. Konsep reproducibility diterapkan agar proses build dapat diulang dari clean checkout dengan hasil yang dapat diverifikasi melalui log, checksum, dan metadata toolchain. 

### 6.1 Konsep Sistem Operasi yang Diuji

```text 
Pada praktikum M0, konsep sistem operasi yang diuji masih berada pada tahap awal (baseline), yaitu belum sampai implementasi kernel, melainkan fokus pada lingkungan pengembangan.

Konsep utama yang digunakan meliputi:

1. Host dan Target
Host adalah sistem utama (Windows 11 + WSL 2) yang digunakan untuk melakukan build, sedangkan target adalah arsitektur x86_64 yang menjadi tujuan kompilasi.

2. Cross-compilation
Proses kompilasi dilakukan menggunakan Clang dengan target x86_64-unknown-none, sehingga menghasilkan object file yang tidak bergantung pada sistem operasi host.

3. ELF (Executable and Linkable Format)
Output dari smoke test berupa file ELF64 relocatable yang merupakan format standar object file pada sistem operasi modern.

4. Freestanding environment
Program dikompilasi tanpa dependensi library standar (freestanding), sesuai kebutuhan awal pengembangan kernel.

5. Toolchain
Toolchain meliputi compiler (clang), linker (ld.lld), dan tools lain seperti readelf dan objdump yang digunakan untuk verifikasi hasil build.

6. Reproducible build
Lingkungan dibuat agar hasil build dapat direproduksi, dengan mencatat versi toolchain pada file metadata.

Dengan demikian, M0 berfokus pada validasi lingkungan dan toolchain, bukan pada implementasi kernel sistem operasi.
Jelaskan konsep utama: bootloader, ELF, linker script, trap frame, PMM, VMM, scheduler, VFS, driver, networking, security, atau topik lain sesuai praktikum.
```

### 6.2 Konsep Arsitektur x86_64 yang Relevan

| Konsep | Relevansi pada praktikum | Bukti/verifikasi |
|---|---|---|
| Long mode | Mode 64-bit untuk menjalankan kernel | QEMU, readelf |
| Paging | Mengatur alamat memori virtual | log QEMU |
| GDT | Struktur segmentasi memori | objdump |
| IDT | Penanganan interrupt | serial log |
| Syscall | Interface user ke kernel | pengujian program |` |
` 
### 6.3 Konsep Implementasi Freestanding

| Aspek | Keputusan praktikum |
|---|---|
| Bahasa | `C17 freestanding ` |
| Runtime | `tanpa hosted libc  `|
| ABI | `x86_64 System V`|
| Compiler flags kritis | `ffreestanding, -mno-red-zone` |
| Risiko undefined behavior | `pointer invalid, alignment, integer overflow` |
`

### 6.4 Referensi Teori yang Digunakan

| No. | Sumber | Bagian yang digunakan | Alasan relevansi |
|---|---|---|---|
| 1 | OSDev Wiki (https://wiki.osdev.org) | ELF, Bare Bones, Freestanding | Digunakan sebagai referensi dasar pengembangan OS dan konsep freestanding environment |
| 2 | LLVM/Clang Documentation (https://clang.llvm.org/docs/) | Command-line options, target triple | Menjelaskan penggunaan compiler clang dan flag seperti --target dan -ffreestanding |
| 3 | GNU Binutils Documentation (https://sourceware.org/binutils/docs/) | readelf, objdump | Digunakan untuk memahami output ELF dan analisis object file |
| 4 | Microsoft WSL Documentation (https://learn.microsoft.com/windows/wsl/) | Instalasi dan penggunaan WSL2 | Digunakan untuk setup environment Linux di Windows |
| 5 | Git Documentation (https://git-scm.com/docs) | Basic Git commands | Digunakan untuk manajemen repository dan version control | |

`

## 7. Lingkungan Praktikum

### 7.1 Host dan Target

| Komponen | Nilai |
|---|---|
| Host OS | `Windows 11 x64 build 26200.8246` |
| Lingkungan build | `WSL 2 Ubuntu` |
| Target ISA | `x86_64` |
| Target ABI | ` x86_64-unknown-none ` |
| Emulator | `QEMU versi 8.2.2` |
| Firmware emulator | `QEMU x86_64 dengan firmware OVMF/UEFI untuk pengujian kernel MCSOS` |
| Debugger | `GDB` |
| Build system | `Make` |
| Bahasa utama | `C17 freestanding` |
| Assembly | `NASM` |
`

### 7.2 Versi Toolchain

Tempel output versi toolchain berikut. Jalankan dari clean shell WSL.

```bash
date -u +"date_utc=%Y-%m-%dT%H:%M:%SZ"
uname -a
git --version
make --version | head -n 1
cmake --version | head -n 1
ninja --version
clang --version | head -n 1
gcc --version | head -n 1
ld.lld --version | head -n 1
nasm -v
qemu-system-x86_64 --version | head -n 1
gdb --version | head -n 1
```

Output:

```text
date_utc=2026-05-05T02:45:33Z
Linux ASUS 6.6.87.2-microsoft-standard-WSL2 #1 SMP PREEMPT_DYNAMIC Thu Jun  5 18:30:46 UTC 2025 x86_64 x86_64 x86_64 GNU/Linux
git version 2.43.0
GNU Make 4.3
cmake version 3.28.3
1.11.1
Ubuntu clang version 18.1.3 (1ubuntu1)
gcc (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
Ubuntu LLD 18.1.3 (compatible with GNU linkers)
NASM version 2.16.01
QEMU emulator version 8.2.2 (Debian 1:8.2.2+ds-0ubuntu1.16)
GNU gdb (Ubuntu 15.1-1ubuntu1~24.04.1) 15.1
```

### 7.3 Lokasi Repository

| Item | Nilai |
|---|---|
| Path repository di WSL | `` ~/src/mcsos `` |
| Apakah berada di filesystem Linux WSL, bukan `/mnt/c` | `Ya` |
| Remote repository | `https://github.com/nagitasalma47-source/mcsos-M0-cute-girls` |
| Branch | `m0/cute-girls` |
| Commit hash awal | `` 3c4ff72 `` |
| Commit hash akhir | `` 77e27fe `` |

---

## 8. Repository dan Struktur File

### 8.1 Struktur Direktori yang Relevan

Tampilkan hanya direktori dan file yang relevan dengan praktikum.

```text
mcsos/
  docs/
    adr/
    architecture/
    governance/
    requirements/
    security/
    testing/
    reports/
  tools/
  smoke/
  build/
    meta/
    smoke/
  Makefile
  README.md

```

### 8.2 File yang Dibuat atau Diubah

| File | Jenis perubahan | Alasan perubahan | Risiko |
|---|---|---|---|
| docs/reports/M0-laporan.md | baru | Menyusun laporan praktikum M0 sebagai dokumentasi hasil kerja | Rendah (hanya dokumentasi) |
| tools/check_env.sh | baru | Script untuk memverifikasi environment dan toolchain | Sedang (jika salah, validasi bisa gagal) |
| smoke/freestanding.c | baru | Source code untuk smoke test freestanding | Rendah (fungsi sederhana) |
| Makefile | baru | Mengatur proses build, check, dan smoke test | Sedang (jika salah, build gagal) |
| docs/requirements/system_requirements.md | baru | Mendefinisikan kebutuhan sistem awal | Rendah (dokumen) |
| docs/security/threat_model.md | ubah | Menyempurnakan analisis risiko keamanan awal | Rendah (dokumen) |
| docs/adr/ADR-0001-toolchain-and-boot-baseline.md | ubah | Menjelaskan keputusan arsitektur awal | Rendah (dokumen) |
| docs/testing/verification_matrix.md | baru | Menentukan metode verifikasi sistem | Rendah (dokumen) |
| README.md | baru | Menjelaskan overview repository | Rendah (dokumen) |` |

### 8.3 Ringkasan Diff

```bash
git status --short
git diff --stat
git log --oneline -n 5
```

Output:

```text
git status --short
(no changes, working tree clean)

git diff --stat
(no changes)

git log --oneline -n 5
77e27fe (HEAD -> m0/cute-girls, origin/m0/cute-girls) M0: finalize ADR and threat model
93bbb32 M0: finalize report with date
5784dda M0: finalize report (kelas 1A)
391f3cc M0: improve report
3c4ff72 M0: add verification matrix
```

---

## 9. Desain Teknis

### 9.1 Masalah yang Diselesaikan

```text
Pada praktikum M0, masalah utama yang diselesaikan adalah penyiapan lingkungan pengembangan sistem operasi yang konsisten dan dapat direproduksi.

Beberapa masalah teknis yang dihadapi antara lain:
- Repository awal belum terinisialisasi sebagai Git repository (error: not a git repository)
- Kebingungan dalam penggunaan perintah terminal seperti pembuatan file laporan dengan redirection (cat << EOF)
- Autentikasi GitHub saat melakukan push memerlukan penggunaan Personal Access Token, bukan password biasa
- Pemahaman penggunaan WSL 2 dan memastikan repository tidak berada di /mnt/c
- Verifikasi toolchain agar sesuai dengan target x86_64-unknown-none
- Memastikan hasil kompilasi berupa object ELF64 relocatable sesuai standar

Masalah-masalah tersebut diselesaikan melalui konfigurasi ulang repository, pemahaman command-line, serta validasi menggunakan tools seperti make check dan readelf.
```

### 9.2 Keputusan Desain

| Keputusan | Alternatif yang dipertimbangkan | Alasan memilih | Konsekuensi |
|---|---|---|---|
| Menggunakan WSL 2 sebagai lingkungan build | Menggunakan Windows native atau WSL 1 | WSL 2 menyediakan kernel Linux yang lebih kompatibel dan stabil untuk development OS | Memerlukan konfigurasi awal dan resource lebih besar |
| Menyimpan repository di ~/src/mcsos (bukan /mnt/c) | Menyimpan di /mnt/c | Menghindari masalah permission dan performa filesystem Windows | Harus bekerja di environment Linux (WSL) |
| Menggunakan Clang dengan target x86_64-unknown-none | Menggunakan GCC default host | Clang mendukung cross-compilation dan lebih fleksibel untuk freestanding environment | Perlu pemahaman flag dan konfigurasi target |
| Menggunakan Makefile sebagai build system | Menggunakan CMake atau Ninja | Makefile lebih sederhana dan cukup untuk kebutuhan M0 | Kurang scalable untuk project besar |
| Menggunakan freestanding environment tanpa libc | Menggunakan hosted environment (dengan libc) | Kernel tidak boleh bergantung pada library OS | Harus mengelola semua fungsi dasar sendiri |` |
`
### 9.3 Arsitektur Ringkas

Tambahkan diagram ASCII atau Mermaid. Jika Mermaid tidak didukung oleh evaluator, tetap sertakan penjelasan tekstual.

```mermaid
flowchart TD
User Command
     ↓
Toolchain (clang, make)
     ↓
Build Output (ELF object)
     ↓
Verification (readelf, objdump)
     ↓
Evidence (log, metadata, laporan)
```

Penjelasan diagram:

```text
Arsitektur praktikum M0 dimulai dari input berupa perintah pengguna seperti make check dan make smoke yang dijalankan pada lingkungan WSL 2.

Perintah tersebut diproses oleh toolchain yang terdiri dari compiler (clang), build system (make), dan tools lain seperti nasm. Toolchain bertanggung jawab untuk menghasilkan output berupa object file dalam format ELF64.

Selanjutnya, hasil build diverifikasi menggunakan tools seperti readelf dan objdump untuk memastikan bahwa object file sesuai dengan target arsitektur x86_64 dan bersifat freestanding.

Hasil verifikasi kemudian dikumpulkan sebagai evidence berupa log, metadata toolchain, dan output command yang dimasukkan ke dalam laporan praktikum.

Batas tanggung jawab:
- User command: memicu proses build dan validasi
- Toolchain: menghasilkan artefak build
- Verification tools: memeriksa validitas output
- Evidence: menyimpan bukti hasil praktikum.
```

### 9.4 Kontrak Antarmuka

| Antarmuka | Pemanggil | Penerima | Precondition | Postcondition | Error path |
|---|---|---|---|---|---|
| `make check` | User melalui terminal | `tools/check_env.sh` | Berada di root repository dan script tersedia | Environment dan toolchain tervalidasi | Jika tool tidak ditemukan, script menampilkan `FAIL` |
| `make smoke` | User melalui terminal | `clang` dan `Makefile` | Source `smoke/freestanding.c` tersedia dan compiler terpasang | Object `build/smoke/freestanding.o` berhasil dibuat | Jika flag/compiler/source salah, build gagal |
| `tools/check_env.sh` | `make check` / user | Tool sistem seperti `git`, `clang`, `qemu`, `gdb` | Repository berada di WSL dan shell dapat menjalankan script | Status tool ditampilkan dan metadata dibuat | Jika tool hilang, status menjadi `FAIL` |
| `m0_smoke_add()` | Smoke source C | CPU / object code x86_64 | Input berupa integer | Menghasilkan hasil penjumlahan sederhana | Jika build target salah, diverifikasi ulang dengan `readelf` |` |
`
### 9.5 Struktur Data Utama

| Struktur data | Field penting | Ownership | Lifetime | Invariant |
|---|---|---|---|---|
| freestanding object (ELF) | header ELF (class, machine, type) | Toolchain (clang) | Dibuat saat make smoke, digunakan saat verifikasi | Harus berformat ELF64, target x86_64, tipe relocatable |
| metadata toolchain | versi tool (git, clang, qemu, dll) | Script check_env.sh | Dibuat saat make check, digunakan dalam laporan | Harus konsisten dan sesuai dengan environment yang digunakan |` |
`
### 9.6 Invariants

Tuliskan invariant yang harus benar sepanjang eksekusi.

1. `Repository harus berada di filesystem Linux WSL (~/src/mcsos), bukan di /mnt/c.`
2. `Semua toolchain yang dibutuhkan (git, clang, make, qemu, dll) harus tersedia dan tervalidasi oleh tools/check_env.sh.`
3. `Hasil kompilasi smoke test harus berupa object ELF64 dengan arsitektur x86_64 dan tipe relocatable.`
4. `4. Proses build harus menggunakan mode freestanding (tanpa dependensi pada library sistem operasi host).`

### 9.7 Ownership, Locking, dan Concurrency

| Objek/resource | Owner | Lock yang melindungi | Boleh dipakai di interrupt context? | Catatan |
|---|---|---|---|---|
| Repository file | User | none | Tidak | Diakses secara manual oleh user melalui terminal |
| Toolchain (clang, make, dll) | System | none | Tidak | Digunakan secara sequential saat build |
| Object build (freestanding.o) | Toolchain | none | Tidak | Dibuat saat make smoke, tidak ada akses paralel |
| Metadata toolchain | Script check_env.sh | none | Tidak | Dibuat saat make check, tidak ada concurrency |` |

Lock order yang berlaku:

```text
Tidak ada lock order yang berlaku pada tahap M0 karena tidak terdapat mekanisme locking maupun concurrency. Seluruh proses berjalan secara sequential.
```

### 9.8 Memory Safety dan Undefined Behavior Risk

| Risiko | Lokasi | Mitigasi | Bukti |
|---|---|---|---|
| Integer overflow| smoke/freestanding.c (fungsi m0_smoke_add) | Menggunakan tipe integer sederhana dan operasi terbatas | Verifikasi melalui objdump dan hasil kompilasi berhasil |
| Uninitialized memory | smoke/freestanding.c | Variabel diinisialisasi sebelum digunakan | Tidak ada warning/error saat build (make smoke) |
| Alignment | build/smoke/freestanding.o | Menggunakan compiler default alignment dari clang | Output readelf menunjukkan struktur ELF valid |
| Out-of-bounds access | smoke/freestanding.c | Tidak menggunakan array atau pointer kompleks | Review kode sederhana dan tidak ada akses memori berbahaya |` |
`
### 9.9 Security Boundary

| Boundary | Data tidak tepercaya | Validasi yang dilakukan | Failure mode aman |
|---|---|---|---|
| User command (make check / make smoke) | Input command dari user | Validasi melalui script tools/check_env.sh | Error ditampilkan di terminal jika tool tidak tersedia |
| Source code input (smoke/freestanding.c) | Isi file source | Kompilasi dengan flag ketat (-Wall -Werror) | Build gagal jika terdapat error |
| Toolchain execution | Tool eksternal (clang, make, dll) | Verifikasi keberadaan tool melalui check_env.sh | Proses dihentikan jika tool tidak ditemukan |
| File system (repository path) | Lokasi repository | Validasi agar tidak berada di /mnt/c | Warning atau error jika path tidak sesuai  |

---

## 10. Langkah Kerja Implementasi

Gunakan tabel berikut untuk setiap langkah. Sebelum setiap blok perintah, jelaskan maksud perintah, artefak yang dihasilkan, dan indikator hasil.

### Langkah 1 — `Verifikasi Lingkungan (WSL & Toolchain)`

Maksud langkah:

```text
Langkah ini dilakukan untuk memastikan bahwa lingkungan pengembangan telah siap digunakan, termasuk memastikan bahwa semua toolchain yang dibutuhkan telah terpasang dan repository berada pada filesystem yang benar.
```

Perintah:

```bash
make check
```

Output ringkas:

```text
[OK] git
[OK] make
[OK] clang
[OK] qemu-system-x86_64
[M0] Metadata written to build/meta/toolchain-versions.txt
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| toolchain-versions.txt | build/meta/ | Menyimpan informasi versi toolchain |` |

Indikator berhasil:

```text
Semua tool ditandai dengan status [OK] dan file metadata berhasil dibuat.
```

### Langkah 2 — `smoke test freestanding`

Maksud langkah:

```text
Langkah ini dilakukan untuk menguji apakah toolchain dapat menghasilkan object file freestanding yang sesuai dengan arsitektur x86_64 tanpa bergantung pada sistem operasi host.
```

Perintah:

```bash
Make Smoke
```

Output ringkas:

```text
clang --target=x86_64-unknown-none ...
ELF Header:
Class: ELF64
Machine: x86-64
Type: REL (Relocatable file)
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| freestanding.o     | build/smoke/ | Object file hasil kompilasi      |
| readelf-header.txt | build/smoke/ | Informasi header ELF             |
| objdump.txt        | build/smoke/ | Disassembly kode hasil kompilasi |` |

Indikator berhasil:
```text
File freestanding.o berhasil dibuat dan hasil verifikasi menunjukkan format ELF64 dengan arsitektur x86_64 dan tipe relocatable.
```
### Langkah 3 —`Verifikasi Repository`

Maksud langkah:

```text
Memastikan repository berada di lokasi yang benar dan tidak berada di /mnt/c.
```
Perintah:

```bash
pwd
git status --short
```
Output ringkas:

```text
/home/nagit/src/mcsos
(nothing to commit, working tree clean)
```
Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `Repository` |`~/src/mcsos` | `Menyimpan seluruh project`|

Indikator berhasil:

```text
Repository berada di filesystem Linux dan tidak ada perubahan yang belum di-commit.
```
### Langkah 4 —`Push ke GitHub`

Maksud langkah:

```text
Mengunggah repository ke GitHub sebagai bukti hasil praktikum.
```
Perintah:

```bash
git push -u origin m0/cute-girls
```
Output ringkas:

```text
new branch m0/cute-girls -> m0/cute-girlsean)
```
Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
|Repository GitHub | https://github.com/nagitasalma47-source/mcsos-M0-cute-girls | Backup dan bukti praktikum |


Indikator berhasil:

```text
Repository berhasil di-push dan dapat diakses di GitHub.t.
```

## 11. Checkpoint Buildable

Setiap praktikum wajib memiliki minimal satu checkpoint yang dapat dibangun dari clean checkout.

| Checkpoint | Perintah | Expected result | Status |
|---|---|---|---|
| Clean build | ` make clean && make build` | `target build belum tersedia pada M0` | `FAIL` |
| Metadata toolchain | `` make meta `` | `build/meta/toolchain-versions.txt ada` | `PASS` |
| Image generation | `` make image `` | `Image belum tersedia pada M0` | `NA` |
| QEMU smoke test | `` make run `` | `Eksekusi QEMU belum tersedia pada M0` | `NA` |
| Test suite | `` make test `` | `Test suite belum tersedia pada M0` | `NA` |

Catatan checkpoint:

```text
Pada tahap M0, hanya checkpoint metadata toolchain yang dapat dijalankan dan berhasil melalui perintah make meta, yang menghasilkan file build/meta/toolchain-versions.txt sebagai bukti bahwa environment dan toolchain telah siap.

Checkpoint lain seperti clean build, image generation, QEMU smoke test, dan test suite belum lulus atau belum tersedia karena praktikum masih berada pada tahap awal dan belum mencakup implementasi kernel, pembuatan image, maupun eksekusi sistem operasi.

Oleh karena itu, status FAIL atau NA pada checkpoint tersebut bukan disebabkan oleh kesalahan implementasi, melainkan karena fitur tersebut belum diimplementasikan pada milestone M0 dan akan tersedia pada tahap selanjutnya (M1 ke atas).
```

---

## 12. Perintah Uji dan Validasi

### 12.1 Build Test

Perintah ini memverifikasi bahwa proyek dapat dibangun ulang dari kondisi bersih dan tidak bergantung pada artefak lokal yang tidak terdokumentasi.

```bash
make clean
make build
```

Hasil:

```text
make clean
make build
make: Nothing to be done for 'build'.
```

Status: `PASS`

### 12.2 Static Inspection

Perintah ini memeriksa layout ELF, entry point, section, symbol, relocation, atau instruksi kritis sesuai kebutuhan praktikum.

```bash
readelf -h build/smoke/freestanding.o
objdump -drwC build/smoke/freestanding.o | head -n 40
```

Hasil penting:

```text
ELF Header:
Class: ELF64
Machine: Advanced Micro Devices X86-64
Type: REL (Relocatable file)

Disassembly:
<m0_smoke_add>:
push   %rbp
mov    %rsp,%rbp
add    ...
ret.
```

Status: `PASS`

### 12.3 QEMU Smoke Test

Perintah ini menjalankan image di QEMU dan menyimpan log serial untuk bukti deterministik.

```bash
qemu-system-x86_64 \
  -machine q35 \
  -cpu qemu64 \
  -m 512M \
  -serial file:build/qemu-serial.log \
  -display none \
  -no-reboot \
  -no-shutdown \
  -cdrom build/mcsos.iso
```

Hasil:

```text
Tidak tersedia pada tahap M0 karena belum ada image (mcsos.iso) yang dapat dijalankan di QEMU.
```

Status: `NA`

### 12.4 GDB Debug Evidence

Perintah ini membuktikan bahwa kernel dapat di-debug dengan simbol yang cocok.

```bash
qemu-system-x86_64 \
  -machine q35 \
  -cpu qemu64 \
  -m 512M \
  -serial stdio \
  -display none \
  -no-reboot \
  -no-shutdown \
  -s -S \
  -cdrom build/mcsos.iso
```

Di terminal lain:

```bash
gdb-multiarch build/kernel.elf
target remote :1234
break kernel_main
continue
info registers
bt
```

Hasil:

```text
Tidak tersedia pada tahap M0 karena belum ada kernel dan image yang dapat dijalankan serta di-debug menggunakan GDB.
```

Status: `NA`

### 12.5 Unit Test

```bash
make test
```

Hasil:

```text
Tidak tersedia pada tahap M0 karena belum ada test suite atau unit test yang diimplementasikan.
```

Status: `NA`

### 12.6 Stress/Fuzz/Fault Injection Test

Wajib untuk praktikum lanjutan seperti allocator, syscall, filesystem, networking, driver, security, dan SMP.

```bash
tidak tersedia pada tahap M0
```

Hasil:

```text
Tidak tersedia pada tahap M0 karena belum ada komponen yang dapat diuji dengan metode stress, fuzz, atau fault injection.
```

Status: `NA`

### 12.7 Visual Evidence

Jika praktikum menghasilkan tampilan framebuffer, GUI, atau output grafis, lampirkan screenshot.

| Screenshot | Lokasi file | Keterangan |
|---|---|---|
### Lampiran F — Screenshot

| No. | File | Keterangan |
|---|---|---|
| 1 | `wsl.exe --list --verbose dan bash tools_check_env.sh .png` | Menunjukkan WSL 2 aktif dan seluruh tool utama berhasil diverifikasi melalui `tools/check_env.sh`. |
| 2 | `cat build_meta_toolchain-versions dan make smoke .txt.png` | Menunjukkan metadata toolchain berhasil dicatat serta target `make smoke` berhasil dijalankan. |
| 3 | `readelf -h build_vmm.o.png` | Menunjukkan object ELF64 x86_64 berhasil terdeteksi melalui `readelf -h`. |
| 4 | `tree -a -L 2.png` | Menunjukkan struktur repository MCSOS pada filesystem Linux WSL. |
| 5 | `git log --oneline -n 3 dan qemu-system-x86_64 --version .png` | Menunjukkan commit repository tervalidasi dan QEMU system x86_64 berhasil terpasang pada environment WSL 2. |

`

## 13. Hasil Uji

### 13.1 Tabel Ringkasan Hasil

| No. | Uji | Expected result | Actual result | Status | Evidence |
|---|---|---|---|---|---|
| 1 | Build Test (make build) | Build berjalan tanpa error | make: Nothing to be done for 'build'. | PASS | Screenshot (34).png |
| 2 | Metadata Toolchain (make meta) | File toolchain-versions.txt terbentuk | Metadata berhasil dibuat di build/meta/ | PASS | Screenshot (34).png |
| 3 | Smoke Test (make smoke) | Object file freestanding.o terbentuk | freestanding.o berhasil dibuat | PASS | Screenshot (35).png, Screenshot (36).png |
| 4 | Static Inspection (readelf) | ELF64 x86_64 valid | Output menunjukkan ELF64 relocatable | PASS | Screenshot (36).png |

Note: Screenshot diambil ulang setelah sesi sebelumnya terhenti; perintah dijalankan kembali dengan hasil yang konsisten.

### 13.2 Log Penting

```text
make build
make: Nothing to be done for 'build'.

make smoke
[build] clang --target=x86_64-unknown-none ...
[ok] freestanding.o generated

readelf -h build/smoke/freestanding.o
Class: ELF64
Machine: Advanced Micro Devices X86-64
Type: REL (Relocatable file)
```

### 13.3 Artefak Bukti

| Artefak | Path | SHA-256 / hash | Fungsi |
|---|---|---|---|
| freestanding.o | build/smoke/freestanding.o | eb9b0dd5b488c91cd1820ce3f94f9a07c96a73b561945e249c50ede9eb1b5c15 | Object file hasil smoke test |
| toolchain-versions.txt | build/meta/toolchain-versions.txt | 94d7345dca4400add402df496e770d68d303caf31abc1ce06daf4c07f45f29a6 | Metadata versi toolchain |

Perintah hash:

```bash
sha256sum build/smoke/freestanding.o
sha256sum build/meta/toolchain-versions.txt


```

---

## 14. Analisis Teknis

### 14.1 Analisis Keberhasilan

```text
Hasil uji pada tahap M0 menunjukkan bahwa lingkungan pengembangan telah berhasil dikonfigurasi dengan benar. Hal ini dibuktikan dengan keberhasilan perintah make build yang tidak menghasilkan error serta make meta yang berhasil menghasilkan file toolchain-versions.txt sebagai metadata versi toolchain.

Selain itu, smoke test melalui make smoke berhasil menghasilkan object file freestanding.o yang kemudian diverifikasi menggunakan readelf. Output menunjukkan bahwa file tersebut berformat ELF64 dengan arsitektur x86_64 dan bertipe relocatable, sesuai dengan invariant yang telah ditentukan pada tahap M0.

Keberhasilan ini menunjukkan bahwa toolchain (clang, make, dan tools lainnya) telah bekerja sesuai desain freestanding environment, tanpa ketergantungan pada sistem operasi host. Dengan demikian, semua komponen dasar yang diperlukan untuk tahap selanjutnya telah tervalidasi dan siap digunakan.
```

### 14.2 Analisis Kegagalan atau Perbedaan Hasil

```text
Pada tahap M0, tidak ditemukan kegagalan kritis dalam proses build maupun verifikasi. Namun, terdapat beberapa perbedaan hasil dibandingkan dengan target akhir sistem operasi.

Gejala yang muncul adalah tidak tersedianya beberapa fitur seperti build kernel lengkap, image generation, eksekusi QEMU, dan unit testing. Hal ini terlihat dari tidak adanya artefak seperti kernel.elf atau mcsos.iso, serta tidak dapat dijalankannya perintah make run atau make test.

Akar penyebabnya adalah karena tahap M0 memang hanya berfokus pada penyiapan lingkungan, toolchain, dan verifikasi awal, sehingga fitur-fitur tersebut belum diimplementasikan.

Bukti pendukung dapat dilihat dari output command yang hanya menghasilkan object file freestanding.o serta metadata toolchain, tanpa adanya file kernel atau image.

Tindakan perbaikan yang direncanakan adalah melanjutkan ke milestone berikutnya (M1) untuk mulai mengimplementasikan toolchain yang lebih lengkap, proses build kernel, serta integrasi dengan QEMU dan GDB
```

### 14.3 Perbandingan dengan Teori

| Konsep teori | Implementasi praktikum | Sesuai/tidak sesuai | Penjelasan |
|---|---|---|---|
| Freestanding environment | Menggunakan clang dengan target x86_64-unknown-none tanpa libc | Sesuai | Kernel harus berjalan tanpa dependensi OS host, sehingga penggunaan freestanding environment sudah tepat |
| Format ELF | Hasil build menghasilkan file freestanding.o berformat ELF64 | Sesuai | Output readelf menunjukkan ELF64 x86_64 relocatable sesuai teori executable format |
| Cross-compilation | Menggunakan target khusus (x86_64-unknown-none) | Sesuai | Compiler tidak menggunakan default host, tetapi target sistem operasi yang akan dibuat |
| Reproducible build | Metadata toolchain disimpan dalam toolchain-versions.txt | Sesuai | Versi toolchain dicatat untuk memastikan build dapat direproduksi |

### 14.4 Kompleksitas dan Kinerja

| Aspek | Estimasi/hasil | Bukti | Catatan |
|---|---|---|---|
| Kompleksitas algoritma | O(1) | Fungsi sederhana m0_smoke_add | Operasi hanya penjumlahan sederhana |
| Waktu build | < 1 detik | Output make build | Build sangat cepat karena hanya compile sederhana |
| Waktu boot QEMU | NA | - | Belum ada image/kernel pada tahap M0 |
| Penggunaan memori | NA | - | Tidak diukur pada tahap M0 |
| Latensi/throughput | NA | - | Belum ada sistem yang berjalan |

---

## 15. Debugging dan Failure Modes

### 15.1 Failure Modes yang Ditemukan

| Failure mode | Gejala | Penyebab sementara | Bukti | Perbaikan |
|---|---|---|---|---|
| Repository belum terhubung ke Git | Perintah git tidak dikenali sebagai repository | Belum menjalankan git init atau belum berada di folder repo | Error "not a git repository" | Menjalankan git init dan memastikan berada di direktori yang benar |
| Gagal push ke GitHub | Diminta username dan password saat push | Menggunakan password biasa, bukan Personal Access Token | Prompt login GitHub di terminal | Menggunakan Personal Access Token sebagai pengganti password |
| Command make build tidak menghasilkan output | Muncul "Nothing to be done for 'build'" | Tidak ada perubahan file sehingga tidak perlu rebuild | Output make build | Menjalankan make clean terlebih dahulu jika ingin rebuild |
| Perbedaan lokasi repository | Repository berada di path yang salah (/mnt/c) | Membuat repo di filesystem Windows | Output pwd menunjukkan /mnt/c | Memindahkan repository ke ~/src/mcsos di WSL |

### 15.2 Failure Modes yang Diantisipasi

| Failure mode | Deteksi | Dampak | Mitigasi |
|---|---|---|---|
| Toolchain tidak lengkap | tools/check_env.sh | Build gagal | Instal ulang tool yang missing |
| Target arsitektur salah | readelf -h | Object tidak sesuai (bukan x86_64) | Gunakan flag --target=x86_64-unknown-none |
| Repository di /mnt/c | pwd | Performa lambat atau permission error | Pindahkan ke ~/src/mcsos |
| Versi toolchain berbeda | toolchain-versions.txt | Build tidak reproducible | Catat dan samakan versi toolchain |

### 15.3 Triage yang Dilakukan

```text
Urutan diagnosis yang dilakukan pada tahap M0 dimulai dari pemeriksaan output command di terminal untuk mengidentifikasi error atau peringatan yang muncul.

Selanjutnya dilakukan pengecekan environment menggunakan tools/check_env.sh untuk memastikan seluruh toolchain tersedia dan sesuai.

Untuk validasi hasil kompilasi, digunakan readelf dan objdump untuk memeriksa format file ELF serta memastikan arsitektur yang dihasilkan adalah x86_64.

Selain itu, dilakukan pengecekan repository menggunakan git status dan git log untuk memastikan tidak ada perubahan yang belum di-commit dan riwayat commit terdokumentasi dengan baik.

Karena pada tahap M0 belum terdapat kernel yang berjalan, maka proses triage belum melibatkan debugging tingkat lanjut seperti GDB, QEMU, atau analisis register.
```

### 15.4 Panic Path

Jika terjadi panic, tempel output panic.

```text
Tidak terdapat panic pada tahap M0 karena sistem operasi belum dijalankan dan belum memiliki kernel maupun runtime environment.

Panic path belum dapat diuji karena praktikum masih berada pada tahap penyiapan environment dan verifikasi toolchain. Pengujian panic path akan dilakukan pada milestone selanjutnya ketika kernel sudah dapat dijalankan dan memiliki mekanisme error handling.
```

---

## 16. Prosedur Rollback

Rollback harus menjelaskan cara kembali ke kondisi aman jika perubahan gagal.

| Skenario rollback | Perintah | Data yang harus diselamatkan | Status |
|---|---|---|---|
| Kembali ke commit awal | git checkout 5784dda | Log dan perubahan laporan | belum diuji |
| Revert commit praktikum | git revert 77e27fe | Log commit dan perubahan file | belum diuji |
| Bersihkan artefak build | make clean | Tidak ada, source aman | teruji |
| Regenerasi image | make image | Tidak ada | belum diuji |

Catatan rollback:

```text
Pada tahap M0, prosedur rollback yang benar-benar diuji adalah make clean untuk membersihkan artefak build.

Sementara itu, perintah git checkout dan git revert belum diuji secara langsung, namun secara teoritis dapat digunakan untuk mengembalikan repository ke kondisi sebelumnya.

Perintah make image belum dapat diuji karena image belum tersedia pada tahap ini.

Risiko dari rollback yang belum diuji adalah kemungkinan kesalahan dalam pemulihan state repository atau kehilangan perubahan yang belum di-backup..
```

---

## 17. Keamanan dan Reliability

### 17.1 Risiko Keamanan

| Risiko | Boundary | Dampak | Mitigasi | Evidence |
|---|---|---|---|---|
| Toolchain tidak valid / terinfeksi | Toolchain execution | Build menghasilkan output tidak terpercaya | Verifikasi dengan tools/check_env.sh dan gunakan sumber resmi | Screenshot make meta |
| Path repository di /mnt/c | File system boundary | Permission error atau performa buruk | Gunakan ~/src/mcsos di WSL | Output pwd |
| File build tertimpa / corrupt | Build artifact | Hasil kompilasi tidak valid | Gunakan make clean sebelum build ulang | Screenshot make build |
| Input command tidak valid | User command | Build gagal atau error | Validasi manual dan cek output terminal | Screenshot make smoke |

### 17.2 Reliability dan Data Integrity

| Risiko reliability | Dampak | Deteksi | Mitigasi |
|---|---|---|---|
| Build gagal | Proses tidak dapat dilanjutkan | Output error pada terminal | Periksa toolchain dengan make check |
| Artefak build tidak konsisten | Hasil kompilasi tidak valid | Perbedaan hasil build atau warning | Gunakan make clean sebelum build ulang |
| Repository tidak sinkron | Perubahan hilang atau konflik | git status / git log | Commit dan push secara berkala |
| Toolchain tidak lengkap | Perintah build tidak berjalan | tools/check_env.sh menunjukkan [FAIL] | Instal ulang tool yang diperlukan |

### 17.3 Negative Test

| Negative test | Input buruk | Expected result | Actual result | Status |
|---|---|---|---|---|
| Repository path check | Repository berada di `/mnt/c` | Script memberi warning atau menolak path tidak sesuai | Repository dipindahkan ke `/home/nagit/src/mcsos` | PASS |
| Missing tool check | Tool wajib tidak tersedia | `tools/check_env.sh` menampilkan status gagal | Semua tool sudah terdeteksi `[OK]` | PASS |
| Wrong target object | Object bukan x86_64 | `readelf -h` menunjukkan machine tidak sesuai | Output menunjukkan `Advanced Micro Devices X86-64` | PASS |
| QEMU run tanpa image | `build/mcsos.iso` belum tersedia | QEMU run belum dilakukan pada M0 | Tidak diuji karena belum ada image | NA |

---

## 18. Pembagian Kerja Kelompok

Isi bagian ini hanya jika praktikum dikerjakan berkelompok. Untuk pengerjaan individu, tulis “Tidak berlaku”.

| Nama | NIM | Peran | Kontribusi teknis | Commit/artefak |
|---|---|---|---|---|
| `Neng Nagita Salma` | `25832071004` | `Anggota kelompok` | `Kerja bersama` | `Commit 77e27fe dan artifact praktikum M0` |
| `Anisa Nur Azfa` | `25832072003` | `Anggota kelompok` | `Kerja bersama` | `Commit 77e27fe dan artifact praktikum M0` |
| `Lailatul Zulfa` | `25832072001` | `Anggota kelompok` | `Kerja bersama` | `Commit 77e27fe dan artifact praktikum M0` |
### 18.1 Mekanisme Koordinasi

```text
Koordinasi dalam kelompok dilakukan secara kolaboratif dengan pendekatan kerja bersama pada setiap tahap praktikum.

Pengelolaan kode dilakukan menggunakan satu branch utama (m0/cute-girls) tanpa pemisahan branch individu, sesuai kesepakatan kelompok dan arahan dosen.

Setiap perubahan dilakukan secara bergantian dan langsung di-commit ke repository, kemudian diverifikasi bersama melalui git log dan hasil build di terminal.

Diskusi dilakukan secara langsung dan melalui komunikasi online untuk memastikan setiap anggota memahami langkah yang dilakukan, termasuk saat setup environment, build test, dan smoke test.

Tidak terdapat konflik merge yang signifikan karena seluruh anggota bekerja secara terkoordinasi dan tidak melakukan perubahan secara bersamaan pada bagian yang sama.
```

### 18.2 Evaluasi Kontribusi

| Anggota | Persentase kontribusi yang disepakati | Bukti | Catatan |
|---|---:|---|---|
| Neng Nagita Salma |                                   33% | commit Git, screenshot build environment, log terminal, dan dokumentasi praktikum | Berkontribusi bersama dalam seluruh tahap praktikum M0 |
| Anisa Nur Azfa    |                                   33% | commit Git, screenshot build environment, log terminal, dan dokumentasi praktikum | Berkontribusi bersama dalam seluruh tahap praktikum M0 |
| Lailatul Zulfa    |                                   34% | commit Git, screenshot build environment, log terminal, dan dokumentasi praktikum | Berkontribusi bersama dalam seluruh tahap praktikum M0 |


Note: Pembagian kontribusi dilakukan secara merata, perbedaan 1% hanya untuk memenuhi total 100%.

## 19. Kriteria Lulus Praktikum

Bagian ini wajib diisi. Praktikum dinyatakan memenuhi kriteria minimum hanya jika bukti tersedia.

| Kriteria minimum | Status | Evidence |
|---|---|---|
| Proyek dapat dibangun dari clean checkout | PASS | Screenshot make build |
| Perintah build terdokumentasi | PASS | Bagian 12.1 Build Test |
| QEMU boot atau test target berjalan deterministik | NA | Belum tersedia pada M0 |
| Semua unit test/praktikum test relevan lulus | NA | Belum tersedia pada M0 |
| Log serial disimpan | NA | Belum tersedia pada M0 |
| Panic path terbaca atau dijelaskan jika belum relevan | PASS | Bagian 15.4 Panic Path |
| Tidak ada warning kritis pada build | PASS | Screenshot make build |
| Perubahan Git terkomit | PASS | git log (commit hash) |
| Desain dan failure mode dijelaskan | PASS | Bagian 9 dan 15 |
| Laporan berisi screenshot/log yang cukup | PASS | Screenshot (34), (35), (36) |

Kriteria tambahan untuk praktikum lanjutan:

| Kriteria lanjutan | Status | Evidence |
|---|---|---|
| Static analysis dijalankan | NA | Tidak dilakukan pada M0 |
| Stress test dijalankan | NA | Tidak tersedia pada M0 |
| Fuzzing atau malformed-input test dijalankan | NA | Tidak tersedia pada M0 |
| Fault injection dijalankan | NA | Tidak tersedia pada M0 |
| Disassembly/readelf evidence tersedia | PASS | Screenshot (36) |
| Review keamanan dilakukan | PASS | Bagian 17 |
| Rollback diuji | PASS | make clean |


## 20. Readiness Review

Pilih satu status dengan alasan berbasis bukti.

| Status | Definisi | Pilihan |
|---|---|---|
| Belum siap uji | Build/test belum stabil atau bukti belum cukup | `[v]` |
| Siap uji QEMU | Build bersih, QEMU/test target berjalan, log tersedia | `[ ]` |
| Siap demonstrasi praktikum | Siap ditunjukkan di kelas dengan bukti uji, failure mode, dan rollback | `[ ]` |
| Kandidat siap pakai terbatas | Hanya untuk penggunaan terbatas setelah test, security review, dokumentasi, dan known issue tersedia | `[ ]` |

Alasan readiness:

```text
Berdasarkan hasil praktikum M0, proyek telah berhasil membangun environment dan memverifikasi toolchain melalui make build, make meta, make smoke, serta validasi ELF menggunakan readelf.

Namun, sistem belum memiliki kernel, image, maupun kemampuan untuk dijalankan pada QEMU. Oleh karena itu, proyek belum siap untuk tahap pengujian lebih lanjut dan dikategorikan sebagai belum siap uji.
```

Known issues:

| No. | Issue | Dampak | Workaround | Target perbaikan |
|---|---|---|---|---|
| 1 | Belum ada kernel dan image | Tidak dapat dijalankan di QEMU | Lanjut ke milestone berikutnya | M1 |
| 2 | Belum ada unit test | Tidak dapat melakukan validasi otomatis | Pengujian manual | M1 |
| 3 | Belum ada panic handling | Tidak bisa uji error runtime | Akan diimplementasikan | M2 |

Keputusan akhir:

```text
Berdasarkan bukti build, hasil smoke test, serta verifikasi ELF menggunakan readelf, praktikum ini telah berhasil pada tahap penyiapan environment. Namun, karena belum terdapat kernel dan kemampuan eksekusi di QEMU, maka hasil praktikum ini dikategorikan sebagai belum siap uji dan akan dilanjutkan pada milestone berikutnya.
```

---

## 21. Rubrik Penilaian 100 Poin

| Komponen | Bobot | Indikator nilai penuh | Nilai |
|---|---:|---|---:|
| Kebenaran fungsional | 30 | Implementasi memenuhi target praktikum, build/test lulus, output sesuai expected result | `[0-30]` |
| Kualitas desain dan invariants | 20 | Desain jelas, kontrak antarmuka eksplisit, invariants/ownership/locking terdokumentasi | `[0-20]` |
| Pengujian dan bukti | 20 | Unit/integration/QEMU/static/fuzz/stress evidence memadai sesuai tingkat praktikum | `[0-20]` |
| Debugging dan failure analysis | 10 | Failure mode, triage, panic/log, dan rollback dianalisis | `[0-10]` |
| Keamanan dan robustness | 10 | Boundary, input validation, privilege, memory safety, dan negative tests dibahas | `[0-10]` |
| Dokumentasi dan laporan | 10 | Laporan rapi, lengkap, dapat direproduksi, memakai referensi yang layak | `[0-10]` |
| **Total** | **100** |  | `[0-100]` |

Catatan penilai:

```text
[Diisi dosen/asisten.]
```

---

## 22. Kesimpulan

### 22.1 Yang Berhasil

```text
Pada praktikum M0, proses setup environment dan toolchain berhasil dilakukan dengan baik. Hal ini dibuktikan dengan keberhasilan menjalankan perintah make build tanpa error, serta make meta yang menghasilkan file toolchain-versions.txt sebagai metadata versi toolchain.

Selain itu, smoke test melalui make smoke berhasil menghasilkan object file freestanding.o. Validasi menggunakan readelf menunjukkan bahwa file tersebut berformat ELF64 dengan arsitektur x86_64, sesuai dengan target sistem.

Hasil ini menunjukkan bahwa lingkungan pengembangan telah siap dan memenuhi syarat untuk melanjutkan ke tahap berikutnya.
```

### 22.2 Yang Belum Berhasil

```text
Pada tahap M0, sistem operasi belum dapat dijalankan karena belum terdapat kernel, image bootable, maupun integrasi dengan QEMU.

Selain itu, belum tersedia unit test, panic handling, serta mekanisme debugging lanjutan seperti GDB. Oleh karena itu, pengujian runtime dan validasi sistem secara menyeluruh belum dapat dilakukan.
```

### 22.3 Rencana Perbaikan

```text
Langkah selanjutnya adalah melanjutkan ke milestone berikutnya (M1) untuk mulai mengimplementasikan toolchain yang lebih lengkap serta proses build kernel.

Selain itu, akan dilakukan integrasi dengan QEMU untuk memungkinkan sistem dijalankan dan diuji secara langsung. Pengembangan juga akan mencakup penambahan unit test, logging, serta mekanisme debugging untuk meningkatkan keandalan sistem.
```

---

## 23. Lampiran

### Lampiran A — Commit Log

```text
77e27fe M0: finalize ADR and threat model
93bbb32 M0: finalize report with date
5784dda M0: finalize report (kelas 1A)
391f3cc M0: improve report
3c4ff72 M0: add verification matrix
```

### Lampiran B — Diff Ringkas

```diff
Perubahan berfokus pada penambahan dokumentasi, metadata toolchain, serta konfigurasi awal praktikum M0.ta.
```

### Lampiran C — Log Build Lengkap

```text
make clean
make build
make: Nothing to be done for 'build'.
```

### Lampiran D — Log QEMU Lengkap

```text
Tidak tersedia pada tahap M0 karena belum ada image yang dapat dijalankan di QEMU.
```

### Lampiran E — Output Readelf/Objdump

```text
ELF Header:
Class: ELF64
Machine: Advanced Micro Devices X86-64
Type: REL (Relocatable file)
```

### Lampiran F — Screenshot

| No. | File | Keterangan |
|---|---|---|
### Lampiran F — Screenshot

| No. | File | Keterangan |
|---|---|---|
| 1 | `wsl.exe --list --verbose dan bash tools_check_env.sh .png` | Menunjukkan WSL 2 aktif dan seluruh tool utama berhasil diverifikasi melalui `tools/check_env.sh`. |
| 2 | `cat build_meta_toolchain-versions dan make smoke .txt.png` | Menunjukkan metadata toolchain berhasil dicatat serta target `make smoke` berhasil dijalankan. |
| 3 | `readelf -h build_vmm.o.png` | Menunjukkan object ELF64 x86_64 berhasil terdeteksi melalui `readelf -h`. |
| 4 | `tree -a -L 2.png` | Menunjukkan struktur repository MCSOS pada filesystem Linux WSL. |
| 5 | `git log --oneline -n 3 dan qemu-system-x86_64 --version .png` | Menunjukkan commit repository tervalidasi dan QEMU system x86_64 berhasil terpasang pada environment WSL 2. |

### Lampiran G — Bukti Tambahan

```text
Tidak ada bukti tambahan pada tahap M0
```

---

## 24. Daftar Referensi

Gunakan format IEEE. Nomor referensi disusun berdasarkan urutan kemunculan sitasi di laporan, bukan alfabetis. Contoh format:

```text
[1] Microsoft, “How to install Linux on Windows with WSL,” Microsoft Learn. Accessed: May 2,
2026. [Online]. Available: 
https://learn.microsoft.com/en-us/windows/wsl/install
 (https://learn.micros
oft.com/en-us/windows/wsl/install)
[2] Microsoft, “Advanced settings configuration in WSL,” Microsoft Learn. Accessed: May 2,
2026. [Online]. Available: 
https://learn.microsoft.com/en-us/windows/wsl/wsl-config
crosoft.com/en-us/windows/wsl/wsl-config)
 (https://learn.mi
[3] QEMU Project, “Invocation,” QEMU Documentation. Accessed: May 2, 2026. [Online].
Available: 
https://www.qemu.org/docs/master/system/invocation.html
 (https://www.qemu.org/docs/m
aster/system/invocation.html)
[4] QEMU Project, “GDB usage,” QEMU Documentation. Accessed: May 2, 2026. [Online].
Available: 
https://www.qemu.org/docs/master/system/gdb.html
stem/gdb.html)
 (https://www.qemu.org/docs/master/sy
[5] GNU Project, “Prerequisites for GCC,” Installing GCC. Accessed: May 2, 2026. [Online].
Available: 
https://gcc.gnu.org/install/prerequisites.html
 (https://gcc.gnu.org/install/prerequisites.html)
[6] GNU Project, “Installing GCC: Building,” Installing GCC. Accessed: May 2, 2026. [Online].
Available: 
https://gcc.gnu.org/install/build.html
 (https://gcc.gnu.org/install/build.html)
[7] LLVM Project, “Cross-compilation using Clang,” Clang Documentation. Accessed: May 2,
2026. [Online]. Available: 
https://clang.llvm.org/docs/CrossCompilation.html
ocs/CrossCompilation.html)
 (https://clang.llvm.org/d
[8] Limine Project, “Limine,” Limine Bootloader. Accessed: May 2, 2026. [Online]. Available:
https://limine-bootloader.org/
 (https://limine-bootloader.org/)
[9] Ubuntu, “Package Search Results — qemu-system-x86,” Ubuntu Packages. Accessed: May
2, 2026. [Online]. Available: 
https://packages.ubuntu.com/qemu-system-x86(https://packages.ubuntu.com/qemu-system-x86)
```

Referensi yang benar-benar dipakai dalam laporan:

```text
1 R. H. Arpaci-Dusseau and A. C. Arpaci-Dusseau, Operating Systems: Three Easy Pieces. Madison, WI, USA: Arpaci-Dusseau Books, 2018. [Online]. Available: https://pages.cs.wisc.edu/~remzi/OSTEP/. Accessed: May 5, 2026.

2 Intel Corporation, Intel 64 and IA-32 Architectures Software Developer’s Manual. [Online]. Available: https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html. Accessed: May 5, 2026.

3 Advanced Micro Devices, AMD64 Architecture Programmer’s Manual. [Online]. Available: https://www.amd.com/system/files/TechDocs/24594.pdf. Accessed: May 5, 2026.

4 GNU Project, “GNU Make Manual.” [Online]. Available: https://www.gnu.org/software/make/manual/. Accessed: May 5, 2026.

5 LLVM Project, “Clang Documentation.” [Online]. Available: https://clang.llvm.org/docs/. Accessed: May 5, 2026.
```

---

## 25. Checklist Final Sebelum Pengumpulan

| Checklist | Status |
|---|---|
| Semua placeholder [isi ...] sudah diganti | Ya |
| Metadata laporan lengkap | Ya |
| Commit awal dan akhir dicatat | Ya |
| Perintah build dan test dapat dijalankan ulang | Ya |
| Log build dilampirkan | Ya |
| Log QEMU/test dilampirkan | Tidak |
| Artefak penting diberi hash | Ya |
| Desain, invariants, ownership, dan failure modes dijelaskan | Ya |
| Security/reliability dibahas | Ya |
| Readiness review tidak berlebihan | Ya |
| Rubrik penilaian diisi atau disiapkan | Ya |
| Referensi memakai format IEEE | Ya |
| Laporan disimpan sebagai Markdown | Ya |

---

## 26. Pernyataan Pengumpulan

Kami mengumpulkan laporan ini bersama artefak pendukung pada commit:

```text
77e27fe
```

Status akhir yang diklaim:

```text
belum siap uji
```

Ringkasan satu paragraf:

```text
Pada praktikum M0, telah berhasil dilakukan setup environment dan verifikasi toolchain menggunakan make build, make meta, dan make smoke. Hasil build menunjukkan tidak adanya error, serta object file freestanding.o berhasil dihasilkan dan divalidasi menggunakan readelf sebagai ELF64 dengan arsitektur x86_64. Selain itu, metadata toolchain berhasil disimpan sebagai bukti reproducibility. Namun, sistem operasi belum dapat dijalankan karena belum terdapat kernel, image, dan integrasi dengan QEMU. Oleh karena itu, praktikum ini dikategorikan belum siap uji dan akan dilanjutkan pada milestone berikutnya untuk implementasi kernel dan pengujian runtime.
```
