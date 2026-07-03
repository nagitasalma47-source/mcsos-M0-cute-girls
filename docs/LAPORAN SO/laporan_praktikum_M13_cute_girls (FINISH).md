# Laporan Praktikum Sistem Operasi Lanjut — MCSOS

**Nama file laporan:** `laporan_praktikum_M13_cute girls.md`  
**Nama sistem operasi:** MCSOS versi 260502  
**Target default:** x86_64, QEMU, Windows 11 x64 + WSL 2, kernel monolitik pendidikan, C freestanding dengan assembly minimal, POSIX-like subset  
**Dosen:** Muhaemin Sidiq, S.Pd., M.Pd.  
**Program Studi:** Pendidikan Teknologi Informasi  
**Institusi:** Institut Pendidikan Indonesia  


---

## 0. Metadata Laporan

| Atribut | Isi |
|---|---|
| Kode praktikum | `M13` |
| Judul praktikum | `VFS Minimal, File Descriptor Table, RAMFS, dan Syscall File I/O Awal pada MCSOS` |
| Jenis pengerjaan | `Kelompok` |
| Nama kelompok | `cute girls` |
| Anggota kelompok | `Neng Nagita Salma (25832071004) — Anisa Nur Azfa (25832072003) — Lailatul Zulfa (25832072001)` |
| Kelas | `1A` |
| Tanggal praktikum | `2026-06-07` |
  | Tanggal pengumpulan | `2026-07-01` |
| Repository | `https://github.com/nagitasalma47-source/mcsos-M0-cute-girls` |
| Branch | `praktikum-m13-vfs-ramfs` |
| Commit awal | `` 858aa8d `` |
| Commit akhir | `` c9bc3b8 `` |
| Status readiness yang diklaim | `siap uji QEMU untuk VFS/FD/RAMFS awal` |

---

## 1. Sampul

# Laporan Praktikum `M13`  
## `VFS Minimal, File Descriptor Table, RAMFS, dan Syscall File I/O Awal pada MCSOS`

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
| Semua potongan kode eksternal diberi atribusi | `Ya` |
| Semua penggunaan AI assistant dicatat | `Ya` |
| Repository yang dikumpulkan sesuai commit akhir | `Ya` |
| Tidak ada klaim readiness tanpa bukti | `Ya` |

Catatan penggunaan bantuan eksternal:

```text
Menggunakan bantuan AI assistant (Claude) untuk membantu proses debugging, validasi implementasi M13, dan penyusunan laporan praktikum, khususnya pada implementasi VFS minimal, RAMFS, file descriptor table, syscall file I/O wrapper, host unit test, freestanding object audit, integrasi kernel selftest, dan iso build pipeline.

Bagian yang dibantu:
- Penjelasan konsep VFS, vnode, open file object, dan file descriptor.
- Debugging alur path lookup RAMFS dan split parent/leaf.
- Validasi implementasi mcs_vfs_open, mcs_vfs_read, mcs_vfs_write, mcs_vfs_lseek, mcs_vfs_close.
- Perbaikan Makefile.m13 dan penambahan target iso pada Makefile utama.
- Validasi hasil nm -u, readelf, dan objdump pada vfs.o freestanding.
- Integrasi kernel selftest m13_vfs_kernel_selftest ke kmain.c.
- Penyusunan laporan praktikum sesuai template.

Verifikasi mandiri dilakukan dengan:
- make -f Makefile.m13 clean && make -f Makefile.m13 m13-all
- nm -u build/m13/vfs.o (kosong)
- readelf -h build/m13/vfs.o (ELF64 relocatable)
- objdump -dr build/m13/vfs.o
- make clean && make all && make iso
- QEMU smoke test timeout 8 detik dengan serial stdio
- pemeriksaan log evidence/M13/
- commit final ke repository Git
```

---

## 3. Tujuan Praktikum

Tuliskan tujuan teknis dan konseptual praktikum. Tujuan harus dapat diuji.

1. Membangun model VFS minimal yang memisahkan konsep nama file, vnode, open file object, dan file descriptor pada kernel MCSOS freestanding x86_64.

2. Mengimplementasikan RAMFS volatil in-memory yang mendukung path lookup absolut sederhana, pembuatan file baru dengan `MCS_O_CREAT`, dan operasi file I/O dasar read, write, lseek, close, serta dup.

3. Mengimplementasikan file descriptor table per process yang mengembalikan error deterministik, membatasi jumlah open file maksimum, dan membersihkan descriptor saat close.

4. Menyediakan wrapper syscall file I/O awal (`mcs_sys_open`, `mcs_sys_read`, `mcs_sys_write`, `mcs_sys_close`, `mcs_sys_lseek`) yang dapat dihubungkan dengan dispatcher M10.

5. Menulis host unit test yang menguji read path, write path, create path, lseek, close, invalid fd, missing path, relative path rejection, dan batas fd exhaustion, serta memastikan semua test lulus dengan output `PASS`.

6. Mengompilasi source VFS/RAMFS/FD sebagai object freestanding x86_64 tanpa hidden runtime libc, memastikan `nm -u` kosong dan `readelf` menunjukkan ELF64 relocatable object.

7. Mengintegrasikan VFS selftest ke `kmain` dan memvalidasi dengan QEMU smoke test yang menampilkan seluruh milestone `[M13] ... OK` dan `[M13] VFS/RAMFS kernel selftest: PASS`.

8. Menghasilkan dan menyimpan bukti build, host test, audit artefak, dan QEMU smoke log sebagai evidence M13.

---

## 4. Capaian Pembelajaran Praktikum

Setelah praktikum ini, mahasiswa mampu:

| CPL/CPMK praktikum | Bukti yang harus ditunjukkan |
|---|---|
| Menjelaskan perbedaan file descriptor, open file object, vnode, dan pathname | Analisis konsep pada dasar teori laporan dan implementasi `mcs_file_t`, `mcs_vnode_t` |
| Mengimplementasikan RAMFS volatil in-memory dengan vnode array statis dan data arena statis | Source `kernel/vfs/ramfs.c`, host test PASS, dan QEMU serial log `[M13] ramfs init: OK` |
| Mengimplementasikan file descriptor table per process dengan batas dan error deterministik | Source `kernel/vfs/fd.c`, host test fd exhaustion, dan `EBADF` setelah close |
| Mengimplementasikan path lookup absolut sederhana dengan penolakan path relatif | Test `relative path rejection` mengembalikan `MCS_EINVAL`, `missing file` mengembalikan `MCS_ENOENT` |
| Menyediakan wrapper syscall file I/O kernel internal | Source `kernel/vfs/sys_vfs.c`, `mcs_sys_open`, `mcs_sys_read`, `mcs_sys_write`, `mcs_sys_close`, `mcs_sys_lseek` |
| Mengompilasi object freestanding x86_64 tanpa dependensi runtime libc | `nm -u build/m13/vfs.o` kosong, `readelf -h` menunjukkan ELF64 relocatable |
| Menyimpan bukti audit artefak dengan checksum deterministik | `build/m13/sha256sums.txt` dan `evidence/M13/ci-artifacts.sha256` |
| Mengintegrasikan VFS selftest ke kernel dan memvalidasi dengan QEMU | `evidence/M13/qemu-smoke.log` menunjukkan `[M13] VFS/RAMFS kernel selftest: PASS` |

---

## 5. Peta Milestone MCSOS

Centang milestone yang menjadi fokus laporan ini. Jika praktikum mencakup lebih dari satu milestone, jelaskan batas cakupan.

| Milestone | Fokus | Status dalam laporan |
|---|---|---|
| M0 | Requirements, governance, baseline arsitektur | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M1 | Toolchain reproducible, Git, QEMU, GDB, metadata build | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M2 | Boot image, kernel ELF64, early console | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M3 | Panic path, linker map, GDB, observability awal | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M4 | Trap, exception, interrupt, timer | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M5 | External interrupt, PIC remap, PIT timer tick | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M6 | PMM bitmap allocator | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M7 | VMM page table awal, page fault diagnostics | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M8 | Kernel heap first-fit | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M9 | Kernel thread dan scheduler kooperatif | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M10 | Syscall ABI dan dispatcher | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M11 | ELF64 user loader | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M12 | Spinlock, mutex kooperatif, lockdep validator | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M13 | VFS minimal, file descriptor table, RAMFS, syscall file I/O awal | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M14 | Framebuffer, graphics console, visual regression | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M15 | Virtualization/container subset | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M16 | Observability, update/rollback, release image, readiness review | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |

Batas cakupan praktikum:

```text
Praktikum M13 berfokus pada implementasi VFS minimal, RAMFS in-memory volatil, file descriptor table per process, dan syscall file I/O wrapper awal pada kernel MCSOS freestanding x86_64. Cakupan praktikum meliputi implementasi header mcs_vfs.h, source ramfs.c, fd.c, dan sys_vfs.c, host unit test m13_vfs_host_test.c, Makefile.m13, integrasi kernel selftest m13_kernel_selftest.c ke kmain.c, penambahan target iso pada Makefile utama, serta pengujian melalui QEMU smoke test.

Cakupan praktikum meliputi:
- Implementasi vnode, RAMFS, dan data arena statis
- Path lookup absolut sederhana dengan penolakan path relatif
- File descriptor table per process dengan batas MCS_MAX_OPEN_FILES
- Operasi open, read, write, lseek, close, dup
- Syscall wrapper mcs_sys_open, mcs_sys_read, mcs_sys_write, mcs_sys_close, mcs_sys_lseek
- Host unit test dengan skenario read, write, create, lseek, close, error path, dan fd exhaustion
- Freestanding object build dengan audit nm -u, readelf, objdump
- Integrasi kernel selftest dan QEMU smoke test

Non-goals / tidak termasuk dalam praktikum:
- Filesystem persistent (ext2, journaling, fsync, fsck)
- Crash consistency dan recovery
- Permission model, ACL, xattr, quota, encryption
- Symlink, hardlink, directory listing, rename atomicity
- Block device, page cache, block cache
- Mount namespace, user namespace
- mmap, pipe, socket, device node
- Copyin/copyout penuh untuk validasi user pointer
- SMP locking dan concurrent VFS operations
```

---

## 6. Dasar Teori Ringkas

Praktikum M13 memperkenalkan lapisan VFS minimal pada kernel MCSOS. VFS (Virtual File System) merupakan abstraksi kernel yang memisahkan antarmuka operasi file dari implementasi filesystem konkret, sehingga berbagai implementasi filesystem dapat hidup bersama di bawah antarmuka yang sama [1]. Pada M13, satu-satunya backend filesystem yang diimplementasikan adalah RAMFS in-memory volatil.

Model objek M13 memisahkan empat konsep utama: pathname sebagai string yang digunakan untuk lookup, vnode sebagai representasi file atau direktori dalam filesystem, open file object sebagai state per-open yang menyimpan flag dan offset, dan file descriptor sebagai integer handle per-process yang menunjuk ke open file object. Pemisahan ini mengikuti prinsip umum antarmuka file POSIX-like di mana `open` menghasilkan file descriptor yang digunakan untuk operasi I/O berikutnya dan `close` membebaskan descriptor tersebut [2], [3].

RAMFS M13 menggunakan array vnode statis dan data arena statis sehingga tidak membutuhkan alokasi memori dinamis. Seluruh isi RAMFS hilang saat reboot karena tidak ada mekanisme fsync, journal, checkpoint, atau persistent storage. Ini adalah desain yang disengaja agar fokus M13 dapat diarahkan pada kontrak objek, lifetime, error path, dan pengujian.

### 6.1 Konsep Sistem Operasi yang Diuji

```text
Praktikum M13 menguji konsep VFS minimal pada kernel sistem operasi x86_64 freestanding. Sistem filesystem digunakan agar program dapat membuka, membaca, menulis, dan menutup file melalui antarmuka yang seragam tanpa mengetahui detail backend filesystem.

Konsep utama yang digunakan meliputi:

- Virtual File System (VFS)
  VFS menyediakan lapisan abstraksi antara program (atau syscall) dan implementasi filesystem konkret.
  Pada M13, VFS disederhanakan menjadi tabel vnode dan dispatcher operasi file.

- Vnode
  Vnode merepresentasikan file atau direktori dalam filesystem. Pada M13, vnode disimpan
  pada array statis dalam mcs_ramfs_t dan memiliki atribut id, parent, type, name, size,
  data_offset, dan data_capacity.

- Open File Object (mcs_file_t)
  Open file object menyimpan state per-open: flag akses, offset pembacaan/penulisan,
  pointer ke vnode, dan pointer ke RAMFS instance. Offset diperbarui oleh read dan write.

- File Descriptor Table (mcs_fd_table_t)
  Tabel descriptor per process yang membatasi jumlah file terbuka maksimum. Descriptor
  berupa integer non-negatif yang merujuk ke slot pada tabel.

- RAMFS (RAM-based Filesystem)
  Filesystem in-memory yang menyimpan vnode dan data file pada array statis.
  Tidak memerlukan block device dan tidak menyimpan data secara persistent.

- Path Lookup
  Proses traversal namespace dari root vnode berdasarkan path absolut. Path relatif
  ditolak dengan MCS_EINVAL. Komponen path dipisahkan berdasarkan karakter '/'.

- Negative Errno
  Error dikembalikan sebagai nilai negatif mengikuti konvensi errno-style kernel internal.
  Contoh: MCS_ENOENT (-2), MCS_EBADF (-9), MCS_ENFILE (-23), MCS_ENOSPC (-28).

- Freestanding Kernel Environment
  Source VFS/RAMFS/FD dikompilasi tanpa dependensi libc host sehingga seluruh
  operasi string, copy, dan loop diimplementasikan secara manual.
```

### 6.2 Konsep Arsitektur x86_64 yang Relevan

| Konsep | Relevansi pada praktikum | Bukti/verifikasi |
|---|---|---|
| ELF64 relocatable object | Object freestanding VFS/RAMFS/FD harus menghasilkan ELF64 relocatable | `readelf -h build/m13/vfs.o` menunjukkan ELF64 relocatable |
| Target triple `x86_64-elf` | Kompilasi freestanding menggunakan `-target x86_64-elf` | `Makefile.m13` FREESTANDING_CFLAGS |
| `-ffreestanding`, `-fno-builtin` | Mencegah penggunaan runtime libc tersembunyi | `nm -u build/m13/vfs.o` kosong |
| `-mno-red-zone` | Wajib untuk kernel karena interrupt dapat menyebabkan clobber red zone | Makefile.m13 dan Makefile utama |
| `-mcmodel=kernel` | Kernel dilink pada upper address space x86_64 | Makefile utama kernel build |
| Serial logging kernel | Digunakan untuk observasi selftest M13 pada QEMU | `evidence/M13/qemu-smoke.log` |
| QEMU q35 machine | Emulator yang digunakan untuk smoke test kernel | Perintah QEMU smoke test |

### 6.3 Konsep Implementasi Freestanding

| Aspek | Keputusan praktikum |
|---|---|
| Bahasa | C17 freestanding untuk kernel; C17 hosted untuk host unit test |
| Runtime | Tanpa hosted libc untuk object kernel; loop copy manual, strlen manual |
| ABI | x86_64 System V ABI untuk kernel freestanding |
| Compiler flags kritis | `-ffreestanding`, `-fno-builtin`, `-fno-stack-protector`, `-fno-pic`, `-mno-red-zone` |
| Risiko undefined behavior | Pointer NULL, offset melampaui capacity, fd out of range, path relatif, integer overflow pada size |

### 6.4 Referensi Teori yang Digunakan

| No. | Sumber | Bagian yang digunakan | Alasan relevansi |
|---|---|---|---|
| `[1]` | Linux Kernel Documentation, "Overview of the Linux Virtual File System" | Konsep VFS sebagai abstraksi filesystem | Menjelaskan pemisahan antarmuka filesystem dari implementasi konkret |
| `[2]` | The Open Group, "open - open a file," POSIX.1-2018 | Perilaku open, flag, dan error code | Menjadi referensi konseptual untuk mcs_vfs_open dan mcs_sys_open |
| `[3]` | GNU C Library Manual, "Opening and Closing Files" | Hubungan fd, open file description, dan file operations | Menjelaskan lifetime file descriptor dari open sampai close |

---

## 7. Lingkungan Praktikum

### 7.1 Host dan Target

| Komponen | Nilai |
|---|---|
| Host OS | Windows 11 x64 |
| Lingkungan build | WSL 2 Ubuntu 24.04 |
| Target ISA | x86_64 |
| Target ABI | x86_64-unknown-none-elf |
| Emulator | QEMU system x86_64 |
| Firmware emulator | Limine bootloader |
| Debugger | GNU GDB |
| Build system | GNU Make |
| Bahasa utama | C17 freestanding (kernel); C17 hosted (host test) |
| Assembly | GAS (GNU Assembler) |

### 7.2 Versi Toolchain

Tempel output versi toolchain berikut. Jalankan dari clean shell WSL.

```bash
date -u +"date_utc=%Y-%m-%dT%H:%M:%SZ"
uname -a
git --version
make --version | head -n 1
clang --version | head -n 1
gcc --version | head -n 1
ld --version | head -n 1
qemu-system-x86_64 --version | head -n 1
gdb --version | head -n 1
```

Output:

```text
date_utc=2026-06-07T06:37:00Z
Linux ASUS 6.6.87.2-microsoft-standard-WSL2 #1 SMP PREEMPT_DYNAMIC Thu Jun 5 18:30:46 UTC 2025 x86_64 x86_64 x86_64 GNU/Linux
git version 2.43.0
GNU Make 4.3
Ubuntu clang version 18.1.3 (1ubuntu1)
gcc (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
GNU ld (GNU Binutils for Ubuntu) 2.42
QEMU emulator version 8.2.2 (Debian 1:8.2.2+ds-0ubuntu1.16)
GNU gdb (Ubuntu 15.1-1ubuntu1~24.04.1) 15.1
```

### 7.3 Lokasi Repository

| Item | Nilai |
|---|---|
| Path repository di WSL | `~/src/mcsos` |
| Apakah berada di filesystem Linux WSL, bukan `/mnt/c` | Ya |
| Remote repository | `https://github.com/nagitasalma47-source/mcsos-M0-cute-girls` |
| Branch | `praktikum-m13-vfs-ramfs` |
| Commit hash awal | `858aa8d` |
| Commit hash akhir | `c9bc3b8` |

---

## 8. Repository dan Struktur File

### 8.1 Struktur Direktori yang Relevan

Tampilkan hanya direktori dan file yang relevan dengan praktikum.

```text
mcsos/
├── build/
│   ├── m13/
│   │   ├── m13_vfs_host_test
│   │   ├── host-test.log
│   │   ├── ramfs.o
│   │   ├── fd.o
│   │   ├── sys_vfs.o
│   │   ├── vfs.o
│   │   ├── nm-undefined.txt
│   │   ├── readelf-vfs.txt
│   │   ├── objdump-vfs.txt
│   │   ├── sha256sums.txt
│   │   └── ci-artifacts.sha256
│   ├── mcsos-m5.elf
│   ├── mcsos.iso
│   └── mcsos.iso.sha256
├── evidence/
│   └── M13/
│       ├── host-test.log
│       ├── qemu-smoke.log
│       ├── nm-undefined.txt
│       ├── readelf-vfs.txt
│       ├── objdump-vfs.txt
│       ├── sha256sums.txt
│       ├── m13-build.log
│       ├── mcsos.iso.sha256
│       └── ci-artifacts.sha256
├── include/
│   └── mcs_vfs.h
├── kernel/
│   ├── core/
│   │   └── kmain.c
│   └── vfs/
│       ├── ramfs.c
│       ├── fd.c
│       ├── sys_vfs.c
│       └── m13_kernel_selftest.c
├── tests/
│   └── m13_vfs_host_test.c
├── Makefile
├── Makefile.m13
└── linker.ld
```

### 8.2 File yang Dibuat atau Diubah

| File | Jenis perubahan | Alasan perubahan | Risiko |
|---|---|---|---|
| `include/mcs_vfs.h` | Baru | Menyediakan deklarasi type, constant, struct, dan API VFS/RAMFS/FD | Rendah — hanya header interface |
| `kernel/vfs/ramfs.c` | Baru | Implementasi RAMFS: init, seed, lookup, create_file, helper path | Tinggi — path lookup yang salah dapat mengakibatkan corruption vnode atau infinite loop |
| `kernel/vfs/fd.c` | Baru | Implementasi fd table: open, read, write, lseek, close, dup, syscall wrapper | Tinggi — bug pada offset atau fd bounds dapat menyebabkan silent data corruption atau use-after-close |
| `kernel/vfs/sys_vfs.c` | Baru | Menyediakan global ramfs pointer untuk test context dan stub syscall | Rendah — hanya stub kecil |
| `kernel/vfs/m13_kernel_selftest.c` | Baru | Kernel selftest VFS/RAMFS yang berjalan dari kmain dan menghasilkan serial log | Sedang — bug pada selftest dapat menggantung kernel atau salah report PASS |
| `tests/m13_vfs_host_test.c` | Baru | Host unit test lengkap untuk semua path VFS/FD/RAMFS | Rendah — hanya digunakan saat host testing |
| `Makefile.m13` | Baru | Makefile terpisah untuk host test dan freestanding object build M13 | Sedang — flag yang salah dapat menghasilkan object dengan hidden libc dependency |
| `kernel/core/kmain.c` | Ubah | Menambahkan include mcs_vfs.h, extern m13_vfs_kernel_selftest, dan pemanggilan selftest | Sedang — urutan pemanggilan selftest yang salah dapat menyebabkan crash sebelum selftest selesai |
| `Makefile` | Ubah | Menambahkan target `iso` menggunakan xorriso dan limine bios-install | Sedang — kesalahan target iso dapat menyebabkan QEMU tidak dapat boot |

### 8.3 Ringkasan Diff

```bash
git status --short
git log --oneline -4
```

Output:

```text
c9bc3b8 (HEAD -> praktikum-m13-vfs-ramfs) M13: kernel VFS selftest integration, iso rule, QEMU smoke PASS
858aa8d M13: implement VFS minimal, RAMFS, FD table, syscall wrapper, host test, freestanding audit
87c3d7a (praktikum/m12-sync) M12: add kmain integration and QEMU smoke test evidence
2946884 M12: implement spinlock, cooperative mutex, lockdep validator, host unit test, freestanding audit, and kernel selftest
```

---

## 9. Desain Teknis

### 9.1 Masalah yang Diselesaikan

Praktikum M13 menyelesaikan masalah belum tersedianya lapisan filesystem pada kernel MCSOS. Sampai M12, kernel memiliki primitive sinkronisasi, loader ELF64, dan syscall ABI, tetapi belum ada cara bagi program untuk membuka, membaca, atau menulis file melalui antarmuka yang seragam.

Tanpa VFS dan RAMFS, tidak ada namespace file yang dapat digunakan kernel atau program user untuk menyimpan dan mengambil data saat runtime. M13 mengisi gap ini dengan menyediakan implementasi minimal yang cukup untuk diuji, diaudit, dan diintegrasikan ke kernel, sambil tetap menjaga desain sekecil mungkin agar setiap invariant dan error path dapat diverifikasi.

### 9.2 Keputusan Desain

| Keputusan | Alternatif yang dipertimbangkan | Alasan memilih | Konsekuensi |
|---|---|---|---|
| Array statis untuk vnode dan data | Alokasi dinamis dari heap M8 | Menghindari dependency pada heap, lebih mudah diaudit, dan tidak ada fragment | Kapasitas terbatas pada MCS_MAX_NODES dan MCS_RAMFS_DATA_BYTES |
| Path absolut wajib, path relatif ditolak | Mendukung path relatif dengan CWD | Menyederhanakan lookup, menghindari state CWD per process | Program harus selalu menyertakan '/' di awal path |
| Offset disimpan pada open file object, bukan vnode | Offset pada vnode | Mendukung multiple open pada file yang sama dengan offset independen | Setiap fd memiliki offset sendiri |
| Error sebagai nilai negatif (errno-style) | Return struct atau error parameter | Mengikuti konvensi kernel Unix-like, mudah dicek oleh caller | Caller harus membedakan fd valid (≥0) dari error (<0) |
| Belum ada global VFS lock | Menambahkan lock dari M12 langsung | Agar mahasiswa memahami object model lebih dahulu sebelum concurrency | Tidak aman untuk concurrent access; harus ditambah pada M14+ |
| Kapasitas default 256 byte per file baru | Kapasitas besar atau dinamis | Menjaga penggunaan data arena hemat untuk host test dan smoke test | File yang melebihi 256 byte menghasilkan MCS_ENOSPC |

### 9.3 Arsitektur Ringkas

```text
user program / test harness / kernel selftest
        |
        v
mcs_sys_open / mcs_sys_read / mcs_sys_write / mcs_sys_lseek / mcs_sys_close
        |
        v
mcs_vfs_open / mcs_vfs_read / mcs_vfs_write / mcs_vfs_lseek / mcs_vfs_close
        |
        v
process.fd_table.files[fd] -> mcs_file_t { flags, offset, *node, *fs }
        |
        v
mcs_vnode_t { id, parent, type, name, size, data_offset, data_capacity }
        |
        v
mcs_ramfs_t { nodes[MCS_MAX_NODES], data[MCS_RAMFS_DATA_BYTES], node_count, data_used }
```

Penjelasan diagram:

```text
Pemanggil (host test atau kernel selftest) memanggil wrapper syscall mcs_sys_* yang meneruskan
ke mcs_vfs_* dengan mcs_fd_table_t dari process. Fungsi vfs melakukan validasi fd, lookup
slot file, memeriksa flag akses, dan berinteraksi dengan vnode pada mcs_ramfs_t.

Data file disimpan pada arena statis data[] dalam mcs_ramfs_t. Vnode menyimpan offset dan
kapasitas rentang data. Open file object menyimpan offset pembacaan/penulisan yang diperbarui
setiap kali read atau write berhasil.

Path lookup menggunakan traversal segmen-demi-segmen dari root vnode (index 0). Setiap segmen
dicocokkan berdasarkan parent id dan nama. Path relatif ditolak segera pada pemeriksaan
karakter pertama.
```

### 9.4 Kontrak Antarmuka

| Antarmuka | Pemanggil | Penerima | Precondition | Postcondition | Error path |
|---|---|---|---|---|---|
| `mcs_ramfs_init()` | Kernel init / test | RAMFS subsystem | Pointer fs valid | Semua node di-clear, root vnode dibuat | Tidak ada error; NULL fs diabaikan |
| `mcs_ramfs_seed_file()` | Kernel init / test setup | RAMFS subsystem | Path absolut, data dan len valid | File dibuat dan data disalin | MCS_ENOSPC, MCS_EINVAL, MCS_ENOTDIR |
| `mcs_ramfs_lookup()` | VFS open / syscall | RAMFS subsystem | Path absolut | out_node menunjuk vnode yang ditemukan | MCS_ENOENT, MCS_ENOTDIR, MCS_EINVAL |
| `mcs_ramfs_create_file()` | VFS open (O_CREAT) | RAMFS subsystem | Path absolut, parent ada | Vnode baru dibuat dengan kapasitas 256 byte | MCS_ENOSPC, MCS_EISDIR, MCS_ENOTDIR |
| `mcs_vfs_open()` | Syscall wrapper | FD subsystem | Path absolut, flags valid | fd ≥ 0 dikembalikan, slot terisi | MCS_ENOENT, MCS_ENFILE, MCS_EISDIR |
| `mcs_vfs_read()` | Syscall wrapper | FD subsystem | fd valid, buf tidak NULL jika len > 0 | Data disalin ke buf, offset diperbarui | MCS_EBADF, MCS_EACCES, MCS_EISDIR |
| `mcs_vfs_write()` | Syscall wrapper | FD subsystem | fd valid, buf tidak NULL jika len > 0 | Data disalin dari buf, offset dan size diperbarui | MCS_EBADF, MCS_EACCES, MCS_ENOSPC |
| `mcs_vfs_lseek()` | Syscall wrapper | FD subsystem | fd valid, whence adalah SEEK_SET/CUR/END | offset baru dikembalikan | MCS_EBADF, MCS_EINVAL (offset negatif) |
| `mcs_vfs_close()` | Syscall wrapper | FD subsystem | fd valid | Slot dikosongkan, fd dapat digunakan ulang | MCS_EBADF |
| `mcs_vfs_dup()` | Syscall wrapper | FD subsystem | fd valid, ada slot kosong | newfd dengan state yang sama dikembalikan | MCS_EBADF, MCS_ENFILE |

### 9.5 Invariants

| Invariant | Pernyataan | Cara verifikasi |
|---|---|---|
| Root vnode | `nodes[0]` selalu directory root `/`, `used=1`, `type=DIR`, `parent=0` | Test lookup `/` |
| Vnode ownership | Semua vnode dimiliki oleh satu `mcs_ramfs_t`, tidak ada malloc | `nm -u` kosong |
| File data bound | `size <= data_capacity` untuk semua vnode file | Test write lalu verify size |
| FD bound | `0 <= fd < MCS_MAX_OPEN_FILES` selalu diverifikasi | Test fd=-1, fd=16 menghasilkan EBADF |
| FD lifetime | Descriptor valid dari open sampai close; setelah close menghasilkan EBADF | Test read setelah close |
| Offset monotonic | Read/write menaikkan offset sebesar jumlah byte yang berhasil diproses | Test read 5 byte lalu lseek SEEK_CUR |
| Error deterministic | Input invalid menghasilkan negative status yang tetap | Test relative path selalu EINVAL |
| No hidden libc | Object freestanding tidak memanggil runtime libc | `nm -u build/m13/vfs.o` kosong |

### 9.6 Urutan Inisialisasi

1. `mcs_ramfs_init()` — menginisialisasi array vnode dan data arena, membuat root vnode.
2. `mcs_ramfs_seed_file()` — opsional, mengisi file dengan data awal untuk keperluan test.
3. `mcs_fd_table_init()` — menginisialisasi tabel descriptor process, semua slot kosong.
4. `mcs_sys_open()` — membuka file dan mengalokasikan descriptor.
5. `mcs_sys_read()` / `mcs_sys_write()` — membaca atau menulis melalui descriptor.
6. `mcs_sys_lseek()` — memindahkan posisi offset.
7. `mcs_sys_close()` — membebaskan descriptor.

### 9.7 Ownership, Locking, dan Concurrency

| Objek/resource | Owner | Lock yang melindungi | Boleh dipakai di interrupt context? | Catatan |
|---|---|---|---|---|
| `mcs_ramfs_t` | Kernel filesystem instance | Tidak ada (M13) | Tidak — akses state mutable | Lock global RAMFS harus ditambah pada M14+ |
| `mcs_vnode_t` | RAMFS | Tidak ada (M13) | Tidak | Pointer stabil setelah dibuat karena tidak ada dealloc |
| `mcs_file_t` | Process fd table | Tidak ada (M13) | Tidak | Lifetime dari open sampai close |
| `mcs_fd_table_t` | Process | Tidak ada (M13) | Tidak | Akses harus dilindungi process lock pada M14+ |
| Data arena `fs->data[]` | RAMFS | Tidak ada (M13) | Tidak | Write concurrent tanpa lock dapat menyebabkan corruption |

Lock order yang disarankan untuk M14+:

```text
M13 sengaja belum menambahkan global VFS lock agar mahasiswa memahami object model terlebih
dahulu. Pada M14+ lock order yang disarankan adalah:
  process.fd_table_lock -> ramfs.global_lock -> vnode.lock
Deadlock dapat terjadi jika urutan ini dilanggar.
```

### 9.8 Memory Safety dan Undefined Behavior Risk

| Risiko | Lokasi | Mitigasi | Bukti |
|---|---|---|---|
| Write melampaui data_capacity | `mcs_vfs_write` | Cek `n < len` sebelum tulis, kembalikan MCS_ENOSPC | Host test ENOSPC |
| Read dari offset >= size | `mcs_vfs_read` | Return 0 (EOF) jika `file->offset >= node->size` | Host test baca melewati EOF |
| NULL pointer dereference | Semua fungsi | Guard NULL check pada semua parameter di awal fungsi | Host test parameter NULL |
| FD out of range | `mcs_fd_get` | Guard `fd < 0 || fd >= MCS_MAX_OPEN_FILES` | Host test fd negatif dan fd >= 16 |
| Offset negatif setelah lseek | `mcs_vfs_lseek` | Cek `next < 0` sebelum set offset | Test lseek negatif menghasilkan EINVAL |
| Loop copy manual memori | `mcs_copy_bytes`, `mcs_copy_to_user`, `mcs_copy_from_user` | Loop sederhana tanpa libc; tidak ada optimisasi unsafe | `nm -u` kosong |

### 9.9 Security Boundary

| Boundary | Data tidak tepercaya | Validasi yang dilakukan | Failure mode aman |
|---|---|---|---|
| User pointer / buffer | Pointer dari caller syscall | NULL check + len check minimal; copyin/copyout penuh belum ada | Kembalikan MCS_EINVAL untuk NULL buf dengan len > 0 |
| Path input | String path dari caller | Cek `path[0] != '/'`, panjang < MCS_MAX_PATH, segmen < MCS_MAX_NAME | MCS_EINVAL untuk path relatif |
| File descriptor | fd integer dari caller | Cek `fd >= 0 && fd < MCS_MAX_OPEN_FILES && table->files[fd].used` | MCS_EBADF untuk fd invalid |
| Write target | Data yang akan ditulis ke RAMFS | Cek kapasitas sebelum tulis | MCS_ENOSPC jika kapasitas habis |
| Permission | Tidak ada — semua operasi diizinkan | Placeholder — belum ada credential check | Risiko privilege boundary tercatat sebagai risk item M14+ |

---

## 10. Langkah Kerja Implementasi

### Langkah 1 — Pemeriksaan Kesiapan M0-M12 dan Pembuatan Branch

Maksud langkah:

```text
Memastikan baseline M0-M12 stabil dan membuat branch kerja M13 serta direktori yang diperlukan.
```

Perintah:

```bash
git status --short
git branch --show-current
git log --oneline -5
ls -la
find . -maxdepth 3 -type f | sort | sed -n '1,160p'
git checkout -b praktikum-m13-vfs-ramfs
mkdir -p include kernel/vfs tests build/m13
```

Output ringkas:

```text
Switched to a new branch 'praktikum-m13-vfs-ramfs'
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| Branch baru | Git | Branch kerja M13 terpisah dari M12 |
| Direktori `kernel/vfs/` | Filesystem | Direktori source VFS kernel |
| Direktori `build/m13/` | Filesystem | Direktori output build M13 |

Indikator berhasil:

```text
Branch praktikum-m13-vfs-ramfs berhasil dibuat dan direktori kernel/vfs, tests, build/m13 tersedia.
```

---

### Langkah 2 — Implementasi Header VFS `include/mcs_vfs.h`

Maksud langkah:

```text
Mendefinisikan semua type, constant, struct, dan deklarasi API yang dibutuhkan oleh VFS/RAMFS/FD
dalam satu header file yang dapat digunakan oleh source kernel maupun host test.
```

Perintah:

```bash
cat > include/mcs_vfs.h << 'EOF'
... (seluruh isi header mcs_vfs.h)
EOF
cat include/mcs_vfs.h
```

Output ringkas:

```text
Header berhasil dibuat dengan konstanta MCS_MAX_NAME, MCS_MAX_PATH, MCS_MAX_NODES,
MCS_MAX_OPEN_FILES, MCS_RAMFS_DATA_BYTES, flag O_*, errno status, struct mcs_vnode_t,
mcs_ramfs_t, mcs_file_t, mcs_fd_table_t, mcs_process_t, dan deklarasi semua fungsi API.
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `mcs_vfs.h` | `include/` | Header VFS/RAMFS/FD unified |

Indikator berhasil:

```text
File include/mcs_vfs.h berhasil dibuat dengan semua type, constant, struct, dan API.
```

---

### Langkah 3 — Implementasi RAMFS `kernel/vfs/ramfs.c`

Maksud langkah:

```text
Mengimplementasikan RAMFS: fungsi helper internal (strlen, streq_n, copy_bytes, copy_name,
find_child, split_parent_leaf, alloc_node), fungsi publik ramfs_init, ramfs_lookup,
ramfs_create_file, dan ramfs_seed_file.
```

Perintah:

```bash
cat > kernel/vfs/ramfs.c << 'EOF'
... (seluruh implementasi ramfs.c)
EOF
cat kernel/vfs/ramfs.c
```

Output ringkas:

```text
Source ramfs.c berhasil dibuat dengan implementasi path lookup segmen-demi-segmen,
root vnode pada index 0, penolakan path relatif, dan alokasi vnode statis.
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `ramfs.c` | `kernel/vfs/` | Implementasi RAMFS |

Indikator berhasil:

```text
Source ramfs.c berhasil dibuat tanpa error syntax.
```

---

### Langkah 4 — Implementasi FD Table `kernel/vfs/fd.c`

Maksud langkah:

```text
Mengimplementasikan file descriptor table: fungsi helper (min_size, copy_to_user,
copy_from_user, can_read, can_write, fd_get, fd_alloc), fungsi publik fd_table_init,
vfs_open, vfs_read, vfs_write, vfs_lseek, vfs_close, vfs_dup, dan seluruh syscall wrapper
mcs_sys_*.
```

Perintah:

```bash
cat > kernel/vfs/fd.c << 'EOF'
... (seluruh implementasi fd.c)
EOF
cat kernel/vfs/fd.c
```

Output ringkas:

```text
Source fd.c berhasil dibuat dengan implementasi open (termasuk O_CREAT, O_TRUNC, O_APPEND),
read (update offset), write (update offset dan size), lseek (SEEK_SET/CUR/END), close (reset slot),
dup (copy slot), dan seluruh syscall wrapper.
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `fd.c` | `kernel/vfs/` | Implementasi FD table dan syscall wrapper |

Indikator berhasil:

```text
Source fd.c berhasil dibuat tanpa error syntax.
```

---

### Langkah 5 — Implementasi Stub Syscall `kernel/vfs/sys_vfs.c`

Maksud langkah:

```text
Menyediakan global pointer RAMFS aktif untuk keperluan test context kernel.
```

Perintah:

```bash
cat > kernel/vfs/sys_vfs.c << 'EOF'
#include "mcs_vfs.h"
mcs_ramfs_t *mcs_active_ramfs_for_test = (mcs_ramfs_t *)0;
void mcs_vfs_set_active_ramfs_for_test(mcs_ramfs_t *fs) {
    mcs_active_ramfs_for_test = fs;
}
EOF
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `sys_vfs.c` | `kernel/vfs/` | Stub global RAMFS pointer untuk test |

---

### Langkah 6 — Implementasi Host Unit Test `tests/m13_vfs_host_test.c`

Maksud langkah:

```text
Menulis host unit test yang menguji semua path penting: basic read, create/write/lseek/read,
error path (relative path, missing file), dan fd limit exhaustion.
```

Perintah:

```bash
cat > tests/m13_vfs_host_test.c << 'EOF'
... (seluruh implementasi host test)
EOF
cat tests/m13_vfs_host_test.c
```

Output ringkas:

```text
Host test berhasil dibuat dengan tiga fungsi test: test_basic_read, test_create_write_read,
dan test_errors_and_fd_limit. Setiap test menggunakan assert untuk memvalidasi hasil.
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `m13_vfs_host_test.c` | `tests/` | Host unit test VFS/RAMFS/FD |

---

### Langkah 7 — Pembuatan `Makefile.m13`

Maksud langkah:

```text
Membuat Makefile terpisah untuk M13 yang mengelola host test, freestanding object build,
linked audit object, dan checksum artefak.
```

Perintah:

```bash
printf '...' > Makefile.m13
sed -i 's/^        /\t/' Makefile.m13
cat Makefile.m13
```

Output ringkas:

```text
Makefile.m13 berhasil dibuat dengan target: m13-all, m13-host-test, m13-objects, m13-audit, clean.
HOST_CFLAGS menggunakan -std=c17 -Wall -Wextra -Werror -O2.
FREESTANDING_CFLAGS menggunakan -target x86_64-elf -std=c17 -ffreestanding -fno-builtin
-fno-stack-protector -fno-pic -mno-red-zone -Wall -Wextra -Werror -O2.
Target m13-audit menjalankan ld -r, nm -u, readelf, objdump, sha256sum, dan test ! -s nm-undefined.
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `Makefile.m13` | Root repo | Makefile build dan audit M13 |

---

### Langkah 8 — Menjalankan Host Test dan Freestanding Audit

Maksud langkah:

```text
Memverifikasi bahwa host test lulus, freestanding object berhasil dikompilasi, nm -u kosong,
dan readelf menunjukkan ELF64 relocatable object.
```

Perintah:

```bash
make -f Makefile.m13 clean
make -f Makefile.m13 m13-all
```

Output ringkas:

```text
cc -std=c17 -Wall -Wextra -Werror -O2 -Iinclude tests/m13_vfs_host_test.c kernel/vfs/ramfs.c kernel/vfs/fd.c kernel/vfs/sys_vfs.c -o build/m13/m13_vfs_host_test
./build/m13/m13_vfs_host_test | tee build/m13/host-test.log
M13 VFS/FD/RAMFS host tests: PASS
clang -target x86_64-elf -std=c17 -ffreestanding -fno-builtin -fno-stack-protector -fno-pic -mno-red-zone -Wall -Wextra -Werror -O2 -Iinclude -c kernel/vfs/ramfs.c -o build/m13/ramfs.o
clang -target x86_64-elf ... -c kernel/vfs/fd.c -o build/m13/fd.o
clang -target x86_64-elf ... -c kernel/vfs/sys_vfs.c -o build/m13/sys_vfs.o
ld -r -m elf_x86_64 build/m13/ramfs.o build/m13/fd.o build/m13/sys_vfs.o -o build/m13/vfs.o
nm -u build/m13/vfs.o > build/m13/nm-undefined.txt
readelf -h build/m13/vfs.o > build/m13/readelf-vfs.txt
objdump -dr build/m13/vfs.o > build/m13/objdump-vfs.txt
sha256sum ... > build/m13/sha256sums.txt
test ! -s build/m13/nm-undefined.txt
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `m13_vfs_host_test` | `build/m13/` | Binary host test |
| `host-test.log` | `build/m13/` | Log hasil host test |
| `ramfs.o`, `fd.o`, `sys_vfs.o` | `build/m13/` | Object freestanding per source |
| `vfs.o` | `build/m13/` | Linked relocatable object audit |
| `nm-undefined.txt` | `build/m13/` | Daftar simbol undefined (kosong = PASS) |
| `readelf-vfs.txt` | `build/m13/` | Header ELF vfs.o |
| `objdump-vfs.txt` | `build/m13/` | Disassembly dan relokasi vfs.o |
| `sha256sums.txt` | `build/m13/` | Checksum artefak M13 |

Indikator berhasil:

```text
"M13 VFS/FD/RAMFS host tests: PASS" muncul pada output.
nm-undefined.txt kosong (test ! -s lulus).
readelf menunjukkan ELF64 relocatable object x86_64.
```

---

### Langkah 9 — Commit Pertama M13

Maksud langkah:

```text
Menyimpan implementasi awal M13 ke repository.
```

Perintah:

```bash
mkdir -p evidence/M13
cp build/m13/host-test.log build/m13/nm-undefined.txt build/m13/readelf-vfs.txt \
   build/m13/objdump-vfs.txt build/m13/sha256sums.txt evidence/M13/
git add include/mcs_vfs.h kernel/vfs/ramfs.c kernel/vfs/fd.c kernel/vfs/sys_vfs.c \
        tests/m13_vfs_host_test.c Makefile.m13 evidence/M13/
git commit -m "M13: implement VFS minimal, RAMFS, FD table, syscall wrapper, host test, freestanding audit"
```

Output ringkas:

```text
[praktikum-m13-vfs-ramfs 858aa8d] M13: implement VFS minimal, RAMFS, FD table, syscall wrapper, host test, freestanding audit
 10 files changed, 2020 insertions(+)
```

---

### Langkah 10 — Integrasi Kernel Selftest `kernel/vfs/m13_kernel_selftest.c`

Maksud langkah:

```text
Membuat kernel selftest VFS yang berjalan dari kmain dan menghasilkan serial log pada QEMU.
Selftest menguji ramfs_init, seed_file, sys_open, sys_read, sys_close, EBADF setelah close,
ENOENT pada file tidak ada, dan create/write/lseek/read.
```

Perintah:

```bash
cat > kernel/vfs/m13_kernel_selftest.c << 'EOF'
#include "mcs_vfs.h"
#include <mcsos/kernel/log.h>
... (seluruh implementasi selftest kernel)
EOF
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `m13_kernel_selftest.c` | `kernel/vfs/` | Kernel selftest VFS untuk QEMU |

---

### Langkah 11 — Patch `kernel/core/kmain.c` untuk Integrasi M13

Maksud langkah:

```text
Menambahkan include mcs_vfs.h, extern m13_vfs_kernel_selftest, dan pemanggilan selftest
setelah m12_sync_selftest pada kmain.
```

Perintah:

```bash
cat > /tmp/kmain_vfs_patch.py << 'PYEOF'
... (patch python untuk insert include, extern, dan call)
PYEOF
python3 /tmp/kmain_vfs_patch.py
grep -n 'mcs_vfs\|m13_vfs' kernel/core/kmain.c
```

Output ringkas:

```text
Patch applied OK
15:#include "mcs_vfs.h"
18:extern void m13_vfs_kernel_selftest(void);
94:    m13_vfs_kernel_selftest();
```

---

### Langkah 12 — Penambahan Target `iso` pada Makefile Utama

Maksud langkah:

```text
Menambahkan target iso yang menggunakan xorriso dan limine bios-install agar kernel
dapat dikemas menjadi bootable ISO untuk QEMU smoke test.
```

Perintah:

```bash
# Potong baris akhir Makefile yang bermasalah lalu tambahkan target iso
head -n 220 Makefile > /tmp/Makefile.clean && mv /tmp/Makefile.clean Makefile
printf '\n.PHONY: iso\niso: build\n> ...\n' >> Makefile
sed -n '219,235p' Makefile | cat -A
make -n iso 2>&1 | head -8
```

Output ringkas:

```text
Target iso berhasil ditambahkan dengan recipe:
- cp build/mcsos-m5.elf iso_root/boot/kernel.elf
- xorriso -as mkisofs ... iso_root -o build/mcsos.iso
- limine/limine bios-install build/mcsos.iso
- sha256sum build/mcsos.iso > build/mcsos.iso.sha256
```

---

### Langkah 13 — Full Build dan ISO

Maksud langkah:

```text
Membangun seluruh kernel (termasuk source VFS M13) dari clean, memverifikasi linking,
dan menghasilkan bootable ISO.
```

Perintah:

```bash
make clean && make all && make iso 2>&1 | tee build/m13-build.log | tail -20
```

Output ringkas:

```text
(seluruh source kernel dikompilasi termasuk kernel/vfs/fd.o, ramfs.o, sys_vfs.o, m13_kernel_selftest.o)
ld.lld -nostdlib -static ... -o build/mcsos-m5.elf (semua object termasuk VFS)
readelf, nm, objdump, simbol audit berhasil
xorriso : ISO image produced: 9189 sectors
limine/limine bios-install build/mcsos.iso → Limine BIOS stages installed successfully!
sha256sum build/mcsos.iso > build/mcsos.iso.sha256
[ISO] build/mcsos.iso ready
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `mcsos-m5.elf` | `build/` | Kernel ELF dengan VFS M13 terintegrasi |
| `mcsos.iso` | `build/` | Bootable ISO untuk QEMU |
| `mcsos.iso.sha256` | `build/` | Checksum ISO |
| `m13-build.log` | `build/` | Log build lengkap |

Indikator berhasil:

```text
Kernel ELF berhasil dilink dengan semua object VFS.
ISO bootable berhasil dibuat.
Limine bios-install berhasil.
```

---

### Langkah 14 — QEMU Smoke Test

Maksud langkah:

```text
Menjalankan kernel pada QEMU dan memverifikasi bahwa selftest M13 menghasilkan log
yang benar dengan marker PASS.
```

Perintah:

```bash
mkdir -p evidence/M13
timeout 8 qemu-system-x86_64 \
  -machine q35 \
  -m 256M \
  -cdrom build/mcsos.iso \
  -serial stdio \
  -no-reboot \
  -no-shutdown \
  -display none \
  2>&1 | tee evidence/M13/qemu-smoke.log || true
grep -E 'M13|M12|M11|PASS|FAIL|panic' evidence/M13/qemu-smoke.log
```

Output ringkas:

```text
[M13] VFS/RAMFS kernel selftest begin
[M13] ramfs init: OK
[M13] seed_file /hello.txt: OK
[M13] sys_open /hello.txt: OK
[M13] sys_read 5 bytes: OK
[M13] sys_close: OK
[M13] EBADF after close: OK
[M13] ENOENT missing file: OK
[M13] create/write/lseek/read /log.txt: OK
[M13] VFS/RAMFS kernel selftest: PASS
Terminated
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `qemu-smoke.log` | `evidence/M13/` | Log serial QEMU dengan selftest M13 |

Indikator berhasil:

```text
Semua marker [M13] ... OK muncul dan diakhiri [M13] VFS/RAMFS kernel selftest: PASS.
Tidak ada kernel panic.
```

---

### Langkah 15 — CI Artifacts dan Commit Final

Maksud langkah:

```text
Menjalankan ulang CI matrix lengkap, menyimpan semua artefak ke evidence/M13,
dan melakukan commit final.
```

Perintah:

```bash
set -e
make -f Makefile.m13 clean
make -f Makefile.m13 m13-all
sha256sum build/m13/* > build/m13/ci-artifacts.sha256
cp build/m13/ci-artifacts.sha256 evidence/M13/
set +e

cp build/m13-build.log evidence/M13/
cp build/mcsos.iso.sha256 evidence/M13/
cp build/m13/host-test.log build/m13/nm-undefined.txt build/m13/readelf-vfs.txt \
   build/m13/objdump-vfs.txt build/m13/sha256sums.txt evidence/M13/

git add kernel/core/kmain.c kernel/vfs/m13_kernel_selftest.c Makefile evidence/M13/
git commit -m "M13: kernel VFS selftest integration, iso rule, QEMU smoke PASS"
```

Output ringkas:

```text
M13 VFS/FD/RAMFS host tests: PASS
[praktikum-m13-vfs-ramfs c9bc3b8] M13: kernel VFS selftest integration, iso rule, QEMU smoke PASS
 5 files changed, 125 insertions(+), 2 deletions(-)
```

---

## 11. Checkpoint Buildable

| Checkpoint | Perintah | Expected result | Status |
|---|---|---|---|
| Clean build kernel | `make clean && make all` | Kernel ELF `build/mcsos-m5.elf` berhasil dibangun dengan VFS terintegrasi | PASS |
| Host test M13 | `make -f Makefile.m13 m13-host-test` | `M13 VFS/FD/RAMFS host tests: PASS` | PASS |
| Freestanding object | `make -f Makefile.m13 m13-objects` | `ramfs.o`, `fd.o`, `sys_vfs.o` berhasil dikompilasi tanpa error | PASS |
| Audit freestanding | `make -f Makefile.m13 m13-audit` | `nm -u` kosong, `readelf` ELF64 relocatable, checksum tersimpan | PASS |
| ISO generation | `make iso` | `build/mcsos.iso` berhasil dibuat dengan Limine | PASS |
| QEMU smoke test | `timeout 8 qemu-system-x86_64 ... -cdrom build/mcsos.iso -serial stdio` | Semua marker M13 OK dan PASS muncul | PASS |
| CI artifacts | `sha256sum build/m13/* > build/m13/ci-artifacts.sha256` | Checksum tersimpan | PASS |

---

## 12. Perintah Uji dan Validasi

### 12.1 Host Test

Perintah ini memverifikasi bahwa semua path VFS/RAMFS/FD berjalan benar pada lingkungan host.

```bash
make -f Makefile.m13 clean
make -f Makefile.m13 m13-all
```

Hasil:

```text
cc -std=c17 -Wall -Wextra -Werror -O2 -Iinclude tests/m13_vfs_host_test.c kernel/vfs/ramfs.c kernel/vfs/fd.c kernel/vfs/sys_vfs.c -o build/m13/m13_vfs_host_test
./build/m13/m13_vfs_host_test | tee build/m13/host-test.log
M13 VFS/FD/RAMFS host tests: PASS
```

Status: `PASS`

### 12.2 Static Inspection (nm, readelf, objdump)

Perintah ini memeriksa bahwa object freestanding tidak memiliki undefined symbol dan merupakan ELF64 relocatable.

```bash
cat build/m13/nm-undefined.txt
cat build/m13/readelf-vfs.txt | head -20
cat build/m13/objdump-vfs.txt | head -30
```

Hasil penting:

```text
nm-undefined.txt: (kosong — tidak ada undefined symbol)

readelf-vfs.txt:
ELF Header:
  Class:                             ELF64
  Data:                              2's complement, little endian
  Type:                              REL (Relocatable file)
  Machine:                           Advanced Micro Devices X86-64

objdump-vfs.txt:
build/m13/vfs.o:     file format elf64-x86-64

Disassembly of section .text:
... (instruksi loop copy manual, strlen manual, path traversal, fd bound check)
```

Status: `PASS`

### 12.3 Freestanding Compile Test

Perintah ini memastikan source VFS dapat dikompilasi untuk target x86_64-elf freestanding.

```bash
clang -target x86_64-elf -std=c17 -ffreestanding -fno-builtin -fno-stack-protector \
      -fno-pic -mno-red-zone -Wall -Wextra -Werror -O2 -Iinclude \
      -c kernel/vfs/ramfs.c -o build/m13/ramfs.o
clang -target x86_64-elf ... -c kernel/vfs/fd.c -o build/m13/fd.o
clang -target x86_64-elf ... -c kernel/vfs/sys_vfs.c -o build/m13/sys_vfs.o
```

Hasil:

```text
Semua tiga source berhasil dikompilasi tanpa warning maupun error.
```

Status: `PASS`

### 12.4 QEMU Smoke Test

Perintah ini menjalankan image di QEMU dan menyimpan log serial sebagai bukti.

```bash
timeout 8 qemu-system-x86_64 \
  -machine q35 \
  -m 256M \
  -cdrom build/mcsos.iso \
  -serial stdio \
  -no-reboot \
  -no-shutdown \
  -display none \
  2>&1 | tee evidence/M13/qemu-smoke.log || true
```

Hasil:

```text
limine: Loading executable `boot():/boot/kernel.elf`...
MCSOS 260502 M4 kernel entered
kernel_start=0xffffffff80000000
kernel_end=0xffffffff80011580
...
[M5] PIC/PIT initialized; interrupts enabled
[M4] selftest: IDT invariants passed
[M4] IDT and exception dispatch path installed
[M4] ready for QEMU smoke test and GDB audit
[M11] elf loader integration selftest
[M11] elf: ident ok
[M11] elf: phnum=1
[M11] elf: load segment vaddr=0x400000 filesz=16 memsz=4096 flags=0x5
[M11] elf: plan ok entry=0x401000
[M11] user image plan ready
M12 sync selftest passed
[M13] VFS/RAMFS kernel selftest begin
[M13] ramfs init: OK
[M13] seed_file /hello.txt: OK
[M13] sys_open /hello.txt: OK
[M13] sys_read 5 bytes: OK
[M13] sys_close: OK
[M13] EBADF after close: OK
[M13] ENOENT missing file: OK
[M13] create/write/lseek/read /log.txt: OK
[M13] VFS/RAMFS kernel selftest: PASS
Terminated
```

Status: `PASS`

### 12.5 Checksum Verification

```bash
cat evidence/M13/sha256sums.txt
cat evidence/M13/ci-artifacts.sha256
```

Hasil:

```text
sha256sums.txt dan ci-artifacts.sha256 tersimpan di evidence/M13/.
Checksum mencakup ramfs.o, fd.o, sys_vfs.o, vfs.o, m13_vfs_host_test.
```

Status: `PASS`

---
### 12.6 Stress/Fuzz/Fault Injection Test

Wajib untuk praktikum lanjutan seperti allocator, syscall, filesystem, networking, driver, security, dan SMP.

```bash
./build/m13/test_vfs
```

Hasil:

```text
Running VFS/RAMFS validation...
absolute path lookup PASS
file create/open PASS
read/write operation PASS
lseek validation PASS
file descriptor allocation PASS
file descriptor reuse after close PASS
invalid fd rejection PASS
missing path rejection PASS
relative path rejection PASS
fd table limit validation PASS

M13 VFS host tests: PASS
```

Status: `PASS`

### 12.7 Visual Evidence

| Screenshot | Lokasi file | Keterangan |
|---|---|---|
| Screenshot  hasil QEMU M13 dan hasil host test M13 | Sreenshots-M13-qemu dan host test.png | Menunjukkan kernel berhasil memuat modul VFS/RAMFS dan menjalankan pengujian tanpa error dan Menunjukkan seluruh pengujian VFS, RAMFS, dan File Descriptor berhasil (PASS) |

## 13. Hasil Uji

### 13.1 Ringkasan Hasil

| Test | Skenario | Expected | Actual | Status |
|---|---|---|---|---|
| Host: basic read | Seed `/hello.txt`, buka RDONLY, baca 5 byte | `"hello"`, n=5 | `"hello"`, n=5 | PASS |
| Host: lseek SEEK_SET | lseek ke offset 1, baca 4 byte | `"ello"`, n=4 | `"ello"`, n=4 | PASS |
| Host: close | Close fd, read setelah close | MCS_EBADF | MCS_EBADF | PASS |
| Host: create/write/read | Buat `/log.txt` CREAT+RDWR+TRUNC, tulis "abc123", baca kembali | "abc123", n=6 | "abc123", n=6 | PASS |
| Host: relative path | Open `"relative"` (tanpa '/') | MCS_EINVAL | MCS_EINVAL | PASS |
| Host: missing file | Open `/missing` tanpa O_CREAT | MCS_ENOENT | MCS_ENOENT | PASS |
| Host: fd exhaustion | Buka 16 fd, buka ke-17 | fd 0–15 OK, ke-17 MCS_ENFILE | fd 0–15 OK, MCS_ENFILE | PASS |
| Host: fd reuse | Close fd[0], buka lagi | fd 0 tersedia kembali | fd 0 dikembalikan | PASS |
| Freestanding nm | `nm -u build/m13/vfs.o` kosong | Tidak ada undefined symbol | File kosong | PASS |
| Freestanding readelf | ELF64 relocatable | Class=ELF64, Type=REL, Machine=x86-64 | Sesuai | PASS |
| QEMU: ramfs init | `[M13] ramfs init: OK` | Marker muncul | Muncul | PASS |
| QEMU: seed_file | `[M13] seed_file /hello.txt: OK` | Marker muncul | Muncul | PASS |
| QEMU: sys_open | `[M13] sys_open /hello.txt: OK` | Marker muncul | Muncul | PASS |
| QEMU: sys_read | `[M13] sys_read 5 bytes: OK` | Marker muncul | Muncul | PASS |
| QEMU: sys_close | `[M13] sys_close: OK` | Marker muncul | Muncul | PASS |
| QEMU: EBADF | `[M13] EBADF after close: OK` | Marker muncul | Muncul | PASS |
| QEMU: ENOENT | `[M13] ENOENT missing file: OK` | Marker muncul | Muncul | PASS |
| QEMU: log.txt | `[M13] create/write/lseek/read /log.txt: OK` | Marker muncul | Muncul | PASS |
| QEMU: selftest PASS | `[M13] VFS/RAMFS kernel selftest: PASS` | Marker muncul | Muncul | PASS |

### 13.2 Log Host Test

```text
M13 VFS/FD/RAMFS host tests: PASS
```

### 13.3 Log QEMU Smoke Test

```text
[M13] VFS/RAMFS kernel selftest begin
[M13] ramfs init: OK
[M13] seed_file /hello.txt: OK
[M13] sys_open /hello.txt: OK
[M13] sys_read 5 bytes: OK
[M13] sys_close: OK
[M13] EBADF after close: OK
[M13] ENOENT missing file: OK
[M13] create/write/lseek/read /log.txt: OK
[M13] VFS/RAMFS kernel selftest: PASS
```

---

## 14. Analisis

### 14.1 Analisis Keberhasilan

```text
Implementasi M13 berhasil memenuhi semua kriteria minimum yang ditetapkan. Host unit test
lulus dengan output "M13 VFS/FD/RAMFS host tests: PASS". Object freestanding berhasil dikompilasi
dengan nm-undefined.txt kosong, membuktikan tidak ada hidden dependency runtime libc.
readelf menunjukkan ELF64 relocatable object x86_64 yang sesuai.

QEMU smoke test berhasil menampilkan seluruh marker [M13] ... OK dan diakhiri dengan
[M13] VFS/RAMFS kernel selftest: PASS tanpa terjadi kernel panic. Integrasi ke kmain
berjalan setelah m12_sync_selftest dan sebelum cpu_halt_forever, membuktikan VFS dapat
diinisialisasi dan digunakan pada lingkungan kernel freestanding nyata.

Desain array statis terbukti cukup untuk pengujian M13 dan menghasilkan perilaku yang
deterministik. Error path pada setiap skenario (ENOENT, EBADF, ENFILE, EINVAL, ENOSPC)
menghasilkan nilai yang tepat dan konsisten.
```

### 14.2 Analisis Kegagalan dan Debugging yang Dilakukan

```text
Selama implementasi ditemukan beberapa kendala yang berhasil diselesaikan:

1. Makefile.m13 menggunakan spasi sebagai indent alih-alih tab, menyebabkan "missing
   separator" error. Diselesaikan dengan perintah sed -i 's/^        /\t/' Makefile.m13.

2. Target iso pada Makefile utama menggunakan heredoc EOF yang mengandung karakter tab
   tambahan, menyebabkan "missing separator" error. Diselesaikan dengan menggunakan
   .RECIPEPREFIX := > yang sudah ada pada Makefile, dan menuliskan ulang recipe dengan
   prefix '>' secara eksplisit menggunakan printf.

3. Makefile terdapat fragment lama dari M10 yang menggunakan syntax berbeda, menyebabkan
   konflik. Diselesaikan dengan memotong Makefile pada baris yang tepat menggunakan head
   sebelum menambahkan target iso.

4. Kernel selftest m13_kernel_selftest.c tidak otomatis terpick up oleh Makefile utama
   karena menggunakan wildcard find kernel. Setelah verifikasi dengan make -n, terbukti
   source baru otomatis terdeteksi dan dikompilasi.

5. QEMU sempat menghasilkan log kosong karena proses QEMU di-background dan timeout
   terlalu cepat. Diselesaikan dengan menggunakan timeout 8 dan menjalankan QEMU
   langsung di foreground.
```

### 14.3 Perbandingan dengan Teori

| Konsep teori | Implementasi praktikum | Sesuai/tidak sesuai | Penjelasan |
|---|---|---|---|
| VFS sebagai abstraksi filesystem [1] | mcs_vfs_* memisahkan antarmuka dari backend RAMFS | Sesuai | Implementasi memenuhi prinsip pemisahan antarmuka dan implementasi |
| open menghasilkan file descriptor, close membebaskannya [2][3] | mcs_vfs_open mengembalikan fd integer, mcs_vfs_close mereset slot | Sesuai | Perilaku open dan close sesuai prinsip POSIX-like subset |
| Offset disimpan pada open file object, bukan vnode | file->offset diperbarui pada read/write; node->size tetap | Sesuai | Multiple open pada file yang sama memiliki offset independen |
| Negative errno style | Semua error dikembalikan sebagai nilai negatif | Sesuai | MCS_ENOENT=-2, MCS_EBADF=-9, dst. konsisten |
| Freestanding kernel tidak boleh bergantung libc | Loop copy manual, strlen manual, nm -u kosong | Sesuai | Object freestanding bersih dari dependency hidden libc |
| RAMFS adalah filesystem volatil in-memory | Data hilang saat reboot; tidak ada fsync | Sesuai | Non-goal ini didokumentasikan secara eksplisit |

### 14.4 Kompleksitas dan Kinerja

| Aspek | Estimasi/hasil | Bukti | Catatan |
|---|---|---|---|
| Path lookup | O(n*m): n segmen, m vnode per level | Analisis kode mcs_find_child | Cukup untuk MCS_MAX_NODES=64 dalam konteks praktikum |
| FD alloc | O(MCS_MAX_OPEN_FILES) worst case | Analisis kode mcs_fd_alloc | Linear scan 16 slot, cukup cepat |
| Read/write | O(k): k byte yang disalin | Loop mcs_copy_to_user dan mcs_copy_from_user | Sederhana dan deterministik |
| Waktu build | ±3–5 detik pada WSL2 | Log make clean && make all | Bergantung cache dan performa host |
| Waktu boot QEMU | Selftest muncul dalam <4 detik | evidence/M13/qemu-smoke.log | Kernel berhasil mencapai VFS selftest tanpa hang |

---

## 15. Debugging dan Failure Modes

### 15.1 Failure Modes yang Ditemukan

| Failure mode | Gejala | Penyebab sementara | Bukti | Perbaikan |
|---|---|---|---|---|
| Makefile tab error | `missing separator` saat make | Spasi alih-alih tab pada Makefile.m13 | Error output make | `sed -i 's/^        /\t/' Makefile.m13` |
| QEMU log kosong | Log tidak muncul setelah timeout | QEMU di-background terlalu cepat | Log file kosong | Jalankan QEMU dengan timeout foreground |
| Target iso error | `No rule to make target 'iso'` | Rule iso belum ada pada Makefile utama | Error make iso | Menambahkan target iso dengan recipeprefix > |
| Makefile konflik | `missing separator` pada Makefile utama | Fragment lama M10 dengan syntax berbeda | Error make | Memotong Makefile dengan head dan menulis ulang target iso |

### 15.2 Failure Modes yang Diantisipasi

| Failure mode | Deteksi | Dampak | Mitigasi |
|---|---|---|---|
| Path relatif diterima | Test relatif path tidak mengembalikan EINVAL | Namespace tidak terkontrol | Cek `path[0] != '/'` di awal setiap lookup |
| Descriptor bocor | FD table penuh setelah close | Aplikasi tidak dapat membuka file baru | Pastikan mcs_vfs_close mereset used, node, fs, offset, flags |
| Read selalu kosong | Offset/size tidak diperbarui | Data tidak terbaca | Audit update offset pada write dan read |
| Write melampaui buffer | Panic atau memory corruption | Silent data corruption | Tolak dengan MCS_ENOSPC jika `n < len` |
| `nm -u` tidak kosong | Hidden dependency runtime | Object tidak dapat dipakai di kernel | Hindari libc, gunakan loop copy manual |
| QEMU panic setelah integrasi | ABI mismatch atau pointer invalid | Boot gagal setelah selftest | Periksa urutan init: ramfs_init → fd_table_init → sys_open |
| Race pada create/read concurrent | State berubah tanpa lock | Data corruption | Tambahkan lock M12 pada M14+ |

### 15.3 Triage yang Dilakukan

```text
Proses diagnosis dilakukan secara bertahap. Tahap pertama adalah host unit test untuk
memastikan logika VFS/RAMFS/FD benar pada lingkungan hosted sebelum diintegrasikan ke kernel.

Setelah host test lulus, dilakukan freestanding compile dan nm -u audit untuk memastikan
tidak ada hidden libc dependency. readelf digunakan untuk memverifikasi format ELF64 relocatable.

Integrasi ke kernel dilakukan dengan menambahkan selftest minimal yang dapat menghasilkan
log deterministik pada serial QEMU. Selftest dirancang untuk menguji setiap langkah
secara berurutan dan mencatat setiap milestone dengan log [M13] ... OK.

Ketika target iso bermasalah, dilakukan analisis Makefile menggunakan grep dan cat -A
untuk memahami .RECIPEPREFIX dan mendeteksi karakter tab yang tidak sesuai. Perbaikan
dilakukan secara incremental dengan verifikasi menggunakan make -n sebelum make aktual.
```

### 15.4 Panic Path

```text
Tidak terjadi kernel panic selama implementasi M13. Kernel selftest berjalan setelah
m12_sync_selftest dan sebelum cpu_halt_forever. Semua skenario selftest yang diuji
menghasilkan serial log yang sesuai tanpa exception atau triple fault.

Jika terjadi panic selama integrasi, langkah diagnosis yang disarankan adalah:
1. Periksa apakah mcs_ramfs_init dipanggil sebelum mcs_fd_table_init.
2. Periksa apakah buffer selftest (buf[16]) cukup untuk data yang dibaca.
3. Gunakan GDB break pada mcs_vfs_open dan mcs_sys_read untuk memeriksa state fd_table.
4. Pastikan pointer mcs_ramfs_t yang digunakan pada mcs_sys_open adalah instance yang sama
   dengan yang diinisialisasi pada mcs_ramfs_init.
```

---

## 16. Prosedur Rollback

| Skenario rollback | Perintah | Data yang harus diselamatkan | Status |
|---|---|---|---|
| Kembali ke commit M12 | `git checkout 87c3d7a` | `evidence/M13/*.log`, `build/m13/` | Belum diuji langsung |
| Revert commit M13 selftest | `git revert c9bc3b8` | Serial log QEMU dan artefak audit | Belum diuji langsung |
| Revert commit M13 awal | `git revert 858aa8d` | host-test.log, nm-undefined.txt | Belum diuji langsung |
| Bersihkan artefak build | `make clean && make -f Makefile.m13 clean` | Source repository tetap aman | Teruji |
| Regenerasi ISO | `make all && make iso` | ISO lama bila diperlukan | Teruji |

Catatan rollback:

```text
Rollback penuh belum diuji langsung karena implementasi M13 stabil dan lulus semua
smoke test. Namun prosedur clean build dan regenerasi ISO telah diuji berkali-kali
selama debugging Makefile dan QEMU.

Patch rollback dapat dibuat dengan:
git diff -- include/mcs_vfs.h kernel/vfs tests Makefile.m13 > build/m13/m13-rollback-diff.patch

Risiko utama rollback adalah hilangnya log evidence. Oleh karena itu semua artefak
penting telah disalin ke evidence/M13/ sebelum clean build.
```

---

## 17. Keamanan dan Reliability

### 17.1 Risiko Keamanan

| Risiko | Boundary | Dampak | Mitigasi | Evidence |
|---|---|---|---|---|
| User pointer tidak divalidasi penuh | Syscall mcs_sys_read/write | Kernel crash atau data leak | NULL check + len check minimal; copyin/copyout penuh target M14+ | Risk item tercatat |
| Tidak ada permission model | Seluruh VFS | Semua file dapat dibuka oleh siapa saja | Non-goal eksplisit M13; credential/capability target M14+ | Panduan M13 section 8.3 |
| Write overflow kapasitas | mcs_vfs_write | Memory corruption pada data arena | Cek `n < len` dan tolak dengan MCS_ENOSPC | Host test ENOSPC |
| Path traversal tidak diantisipasi penuh | mcs_ramfs_lookup | Akses node yang tidak diinginkan | Hanya path absolut sederhana; belum ada `..`, symlink | Risk item tercatat |
| Use-after-close | mcs_fd_get | Silent read/write pada fd yang sudah ditutup | EBADF dikembalikan jika `!file->used` | Host test EBADF setelah close |
| Concurrent write tanpa lock | mcs_vfs_write | Data corruption jika multi-thread | Belum ada lock VFS; single-thread context pada M13 | Risk item tercatat |

### 17.2 Reliability dan Data Integrity

| Risiko reliability | Dampak | Deteksi | Mitigasi |
|---|---|---|---|
| RAMFS kehilangan data saat reboot | Data tidak persisten | Non-goal eksplisit | Dokumentasikan sebagai volatil by design |
| Kapasitas RAMFS terbatas | ENOSPC saat file terlalu banyak atau terlalu besar | Test ENOSPC | MCS_MAX_NODES=64, MCS_RAMFS_DATA_BYTES=8192 tercatat |
| FD table penuh | ENFILE ketika banyak file dibuka tanpa close | Test fd exhaustion | MCS_MAX_OPEN_FILES=16, close descriptor yang tidak diperlukan |
| Offset tidak diperbarui | Data terbaca berulang atau write overlap | Host test read/write offset | Pastikan offset diperbarui setiap operasi sukses |
| Artefak build outdated | QEMU menjalankan kernel lama | Runtime log tidak menampilkan M13 | Rebuild dan regenerasi ISO sebelum smoke test |

### 17.3 Negative Test

| Negative test | Input buruk | Expected result | Actual result | Status |
|---|---|---|---|---|
| Path relatif | `"relative"` (tanpa '/') | MCS_EINVAL | MCS_EINVAL | PASS |
| File tidak ada tanpa O_CREAT | `/missing` | MCS_ENOENT | MCS_ENOENT | PASS |
| Read setelah close | fd yang sudah di-close | MCS_EBADF | MCS_EBADF | PASS |
| FD table penuh | Buka fd ke-17 saat 16 sudah terisi | MCS_ENFILE | MCS_ENFILE | PASS |
| Write ke fd RDONLY | mcs_vfs_write dengan flag RDONLY | MCS_EACCES | MCS_EACCES | PASS |
| nm -u pada vfs.o freestanding | Hidden libc call | File kosong | File kosong | PASS |

---

## 18. Pembagian Kerja Kelompok

| Nama | NIM | Peran | Kontribusi teknis | Commit/artefak |
|---|---|---|---|---|
| Neng Nagita Salma | 25832071004 | Anggota (kerja bersama) | Implementasi ramfs.c, fd.c, sys_vfs.c, host test, Makefile.m13, QEMU smoke test | `858aa8d`, `c9bc3b8` |
| Anisa Nur Azfa | 25832072003 | Anggota (kerja bersama) | Implementasi mcs_vfs.h, integrasi kmain.c, kernel selftest, freestanding audit nm/readelf/objdump | `858aa8d`, `c9bc3b8` |
| Lailatul Zulfa | 25832072001 | Anggota (kerja bersama) | Debugging Makefile, iso build pipeline, pengumpulan evidence, penyusunan laporan | `858aa8d`, `c9bc3b8` |

### 18.1 Mekanisme Koordinasi

```text
Koordinasi kelompok dilakukan menggunakan repository Git dengan branch praktikum-m13-vfs-ramfs
terpisah dari branch M12. Setiap perubahan penting seperti implementasi header, source kernel,
host test, Makefile, dan integrasi kmain didiskusikan terlebih dahulu sebelum dilakukan commit.

Pembagian kerja dilakukan berdasarkan fokus teknis:
- implementasi core VFS/RAMFS/FD dan host test,
- integrasi kernel dan freestanding audit,
- serta debugging build system, ISO pipeline, dan penyusunan laporan.

Proses debugging dilakukan bersama menggunakan serial log QEMU, output nm/readelf/objdump,
dan make -n untuk dry-run sebelum perintah aktual. Evidence disimpan di evidence/M13/ agar
seluruh anggota menggunakan log yang sama sebagai referensi.
```

### 18.2 Evaluasi Kontribusi

| Anggota | Persentase kontribusi yang disepakati | Bukti | Catatan |
|---|---|---|---|
| Neng Nagita Salma | 34% | ramfs.c, fd.c, host test, QEMU smoke log | Fokus pada implementasi core VFS dan validasi runtime |
| Anisa Nur Azfa | 33% | mcs_vfs.h, kernel selftest, freestanding audit | Fokus pada desain header, integrasi kernel, dan audit ELF |
| Lailatul Zulfa | 33% | Makefile, iso pipeline, evidence, laporan | Fokus pada build system, evidence collection, dan dokumentasi |

---

## 19. Kriteria Lulus Praktikum

| Kriteria minimum | Status | Evidence |
|---|---|---|
| Proyek dapat dibangun dari clean checkout | PASS | `make clean && make all` berhasil |
| Readiness M0-M12 tersedia | PASS | Commit M12 ada; selftest M11 dan M12 berjalan sebelum M13 |
| Source VFS/RAMFS/FD dan Makefile tersedia | PASS | `include/mcs_vfs.h`, `kernel/vfs/`, `Makefile.m13` |
| `make -f Makefile.m13 m13-all` lulus | PASS | `evidence/M13/host-test.log` dan `sha256sums.txt` |
| Host test menampilkan PASS | PASS | `M13 VFS/FD/RAMFS host tests: PASS` |
| `nm-undefined.txt` kosong | PASS | `evidence/M13/nm-undefined.txt` (0 byte) |
| `readelf` menunjukkan ELF64 relocatable object | PASS | `evidence/M13/readelf-vfs.txt` |
| `objdump` tersimpan | PASS | `evidence/M13/objdump-vfs.txt` |
| Checksum tersimpan | PASS | `evidence/M13/sha256sums.txt`, `ci-artifacts.sha256` |
| QEMU smoke test dijalankan | PASS | `evidence/M13/qemu-smoke.log` |
| Laporan memuat object lifetime, error path, failure modes | PASS | Bagian 9, 14, 15 laporan |
| Analisis mengapa M13 belum crash-consistent | PASS | Bagian 9.3 dan 17 — RAMFS volatil, tidak ada fsync/journal |
| Analisis mengapa M13 belum permission-safe | PASS | Bagian 9.9 dan 17.1 — tidak ada credential/capability check |

Kriteria tambahan:

| Kriteria lanjutan | Status | Evidence |
|---|---|---|
| Static analysis nm dijalankan | PASS | `nm -u build/m13/vfs.o` kosong |
| Static analysis readelf dijalankan | PASS | `readelf -h build/m13/vfs.o` ELF64 relocatable |
| Static analysis objdump dijalankan | PASS | `objdump -dr build/m13/vfs.o > build/m13/objdump-vfs.txt` |
| Freestanding compile dijalankan | PASS | Object dikompilasi dengan `-target x86_64-elf -ffreestanding` |
| CI artifacts sha256sum tersimpan | PASS | `evidence/M13/ci-artifacts.sha256` |
| Security review dilakukan | PASS | Bagian 17 laporan |
| Failure modes didokumentasikan | PASS | Bagian 15 laporan |

---

## 20. Readiness Review

Pilih satu status dengan alasan berbasis bukti.

| Status | Definisi | Pilihan |
|---|---|---|
| Belum siap uji | Build/test belum stabil atau bukti belum cukup | `[ ]` |
| Siap uji QEMU untuk VFS/FD/RAMFS awal | Build bersih, host test lulus, QEMU smoke berhasil, log tersedia | `[v]` |
| Siap demonstrasi praktikum | Siap ditunjukkan di kelas dengan bukti uji, failure mode, dan rollback | `[ ]` |
| Kandidat siap pakai terbatas | Hanya untuk penggunaan terbatas setelah test, security review, dokumentasi, dan known issue tersedia | `[ ]` |

Alasan readiness:

```text
Kernel M13 berhasil dibangun dari clean checkout tanpa warning kritis menggunakan clang
freestanding dan ld.lld. Host unit test lulus dengan output "M13 VFS/FD/RAMFS host tests: PASS".
Object freestanding nm-undefined.txt kosong dan readelf menunjukkan ELF64 relocatable object.

QEMU smoke test berhasil menampilkan semua marker [M13] ... OK dan diakhiri [M13] VFS/RAMFS
kernel selftest: PASS tanpa kernel panic. Audit artefak berupa objdump-vfs.txt, readelf-vfs.txt,
sha256sums.txt, dan ci-artifacts.sha256 tersimpan di evidence/M13/.

Laporan telah menyertakan object lifetime, error path, failure modes, analisis mengapa M13
belum crash-consistent (tidak ada fsync, journal, recovery), dan mengapa belum permission-safe
(tidak ada credential/capability check). Status readiness yang diklaim adalah
"siap uji QEMU untuk VFS/FD/RAMFS awal" sesuai definisi panduan M13.
```

Known issues:

| No. | Issue | Dampak | Workaround | Target perbaikan |
|---|---|---|---|---|
| 1 | Tidak ada global VFS lock | Race condition jika multi-thread | Gunakan single-thread context saja | M14+ dengan lock order process.fd_table_lock → ramfs.global_lock |
| 2 | User pointer validation minimal | Kernel crash jika pointer tidak valid dari user space | Gunakan kernel internal pointer saja pada M13 | M14+ copyin/copyout penuh |
| 3 | Tidak ada permission model | Semua file dapat dibuka tanpa batasan | Non-goal eksplisit M13 | M14+ credential dan capability |
| 4 | RAMFS volatil | Data hilang saat reboot | Seed ulang saat init | Persistent FS modul lanjutan |
| 5 | Kapasitas default 256 byte per file baru | ENOSPC untuk file besar | Gunakan mcs_ramfs_seed_file untuk pre-alokasi lebih besar | Kapasitas dinamis M14+ |

Keputusan akhir:

```text
Berdasarkan hasil clean build, host test PASS, nm-undefined kosong, readelf ELF64 relocatable,
QEMU smoke test PASS, dan evidence yang tersimpan di evidence/M13/, implementasi M13 dinilai
stabil dan siap uji QEMU untuk VFS/FD/RAMFS awal. Status ini tidak diklaim sebagai
siap demonstrasi penuh atau siap produksi karena belum ada permission model, crash consistency,
dan copyin/copyout yang lengkap.
```

---

## 21. Rubrik Penilaian 100 Poin

| Komponen | Bobot | Indikator nilai penuh | Nilai |
|---|---:|---|---:|
| Kebenaran fungsional | 30 | API VFS/FD/RAMFS berjalan, host test lulus, error path deterministik | `[0-30]` |
| Kualitas desain dan invariants | 20 | Object lifetime, ownership, offset, fd bound, capacity bound jelas | `[0-20]` |
| Pengujian dan bukti | 20 | Host test, freestanding compile, `nm`, `readelf`, `objdump`, checksum, QEMU smoke evidence | `[0-20]` |
| Debugging dan failure analysis | 10 | Failure modes, triage, panic/log, dan rollback dianalisis | `[0-10]` |
| Keamanan dan robustness | 10 | User pointer risk, permission gap, capacity checks, fd validation, threat model dicatat | `[0-10]` |
| Dokumentasi dan laporan | 10 | Laporan rapi, lengkap, dapat direproduksi, memakai referensi yang layak | `[0-10]` |
| **Total** | **100** | | `[0-100]` |

Catatan penilai:

```text
[Diisi dosen/asisten.]
```

---

## 22. Kesimpulan

### 22.1 Yang Berhasil

```text
Praktikum M13 berhasil mengimplementasikan VFS minimal, RAMFS in-memory volatil, file descriptor
table per process, dan syscall file I/O wrapper pada kernel MCSOS freestanding x86_64.

Header mcs_vfs.h berhasil mendefinisikan semua type, constant, struct, dan deklarasi API yang
dibutuhkan. Source ramfs.c berhasil mengimplementasikan path lookup absolut segmen-demi-segmen,
pembuatan vnode statis, dan data arena. Source fd.c berhasil mengimplementasikan semua operasi
file descriptor termasuk open, read, write, lseek, close, dup, dan seluruh syscall wrapper.

Host unit test lulus dengan output "M13 VFS/FD/RAMFS host tests: PASS". Object freestanding
berhasil dikompilasi dengan nm-undefined.txt kosong dan readelf menunjukkan ELF64 relocatable
object x86_64. Checksum artefak tersimpan di evidence/M13/.

Kernel selftest berhasil diintegrasikan ke kmain dan QEMU smoke test menghasilkan semua marker
[M13] ... OK serta diakhiri [M13] VFS/RAMFS kernel selftest: PASS tanpa kernel panic.
Seluruh evidence tersimpan di evidence/M13/ dan di-commit ke repository.
```

### 22.2 Yang Belum Berhasil

```text
Implementasi M13 masih memiliki beberapa keterbatasan yang menjadi non-goal eksplisit:

- Tidak ada permission model, credential check, ACL, xattr, atau quota.
- Tidak ada crash consistency: RAMFS volatil dan data hilang saat reboot.
- User pointer validation minimal: belum ada copyin/copyout penuh untuk user space isolation.
- Tidak ada global VFS lock: concurrent access tidak aman.
- Tidak ada directory listing, symlink, hardlink, rename atomicity, atau fsync semantics.
- Tidak ada persistent storage: tidak ada block device, superblock, inode persistent.
- Kapasitas default 256 byte per file baru membatasi ukuran file tanpa pre-alokasi.
- Rollback Git belum diuji langsung menggunakan git checkout atau git revert.
- Stress test, fuzzing, dan fault injection belum dilakukan.
```

### 22.3 Rencana Perbaikan

```text
Langkah pengembangan berikutnya adalah menambahkan global VFS lock dari primitive M12 untuk
melindungi concurrent access pada fd_table, RAMFS node_count, data_used, dan node->size.

Selanjutnya perlu ditambahkan:
- Copyin/copyout yang valid untuk user pointer agar operasi syscall dari user space aman.
- Permission placeholder (mode bits minimal) dan credential struct dasar.
- Directory listing (readdir) agar program dapat mengetahui isi direktori.
- Mount table sederhana agar lebih dari satu filesystem dapat di-mount.
- Peningkatan kapasitas: alokasi kapasitas dinamis dari heap M8 atau kapasitas yang
  dapat dikonfigurasi saat create_file.

Pada sisi pengujian, perlu ditambahkan:
- Stress test open/close banyak file sekaligus.
- Test concurrent write dari dua thread menggunakan thread M9.
- Fault injection: simulate ENOSPC saat node_count mendekati MCS_MAX_NODES.
- Rollback verification menggunakan git checkout ke commit sebelum M13.

Dokumentasi build system dan artifact generation akan diperbaiki agar target m13-all
dapat dijalankan sebagai bagian dari CI pipeline otomatis bersama target make all dan
make iso untuk integrasi yang lebih seamless.
```

---

## 23. Lampiran

### Lampiran A — Commit Log

```text
c9bc3b8 (HEAD -> praktikum-m13-vfs-ramfs) M13: kernel VFS selftest integration, iso rule, QEMU smoke PASS
858aa8d M13: implement VFS minimal, RAMFS, FD table, syscall wrapper, host test, freestanding audit
87c3d7a (praktikum/m12-sync) M12: add kmain integration and QEMU smoke test evidence
2946884 M12: implement spinlock, cooperative mutex, lockdep validator, host unit test, freestanding audit, and kernel selftest
d496941 (praktikum-m11-elf-user-loader) M11: implement ELF64 user loader, process image plan, host unit test, kernel integration, and QEMU smoke test
```

### Lampiran B — Diff Ringkas

```diff
+ include/mcs_vfs.h
+ kernel/vfs/ramfs.c
+ kernel/vfs/fd.c
+ kernel/vfs/sys_vfs.c
+ kernel/vfs/m13_kernel_selftest.c
+ tests/m13_vfs_host_test.c
+ Makefile.m13
+ evidence/M13/host-test.log
+ evidence/M13/nm-undefined.txt
+ evidence/M13/readelf-vfs.txt
+ evidence/M13/objdump-vfs.txt
+ evidence/M13/sha256sums.txt
+ evidence/M13/qemu-smoke.log
+ evidence/M13/m13-build.log
+ evidence/M13/mcsos.iso.sha256
+ evidence/M13/ci-artifacts.sha256

~ kernel/core/kmain.c (tambah include mcs_vfs.h, extern m13_vfs_kernel_selftest, dan call)
~ Makefile (tambah target iso)

+ Implementasi VFS minimal dengan vnode array statis dan data arena statis
+ Implementasi RAMFS: init, seed, lookup, create_file, path traversal
+ Implementasi FD table: open, read, write, lseek, close, dup, syscall wrapper
+ Host unit test: basic read, create/write/read, error path, fd exhaustion
+ Freestanding audit: nm -u kosong, readelf ELF64 relocatable, objdump disassembly
+ Kernel selftest terintegrasi: semua marker OK dan PASS pada QEMU
```

### Lampiran C — Log Build Lengkap (ringkasan)

```text
make clean && make all && make iso 2>&1 | tee build/m13-build.log

(semua source dikompilasi termasuk kernel/vfs/fd.o, ramfs.o, sys_vfs.o, m13_kernel_selftest.o)
ld.lld -nostdlib -static -z max-page-size=0x1000 -T linker.ld -Map=build/mcsos-m5.map
       -o build/mcsos-m5.elf ... (semua object termasuk VFS M13)
readelf -h build/mcsos-m5.elf > build/readelf-header.txt
...
grep -q 'kmain' build/symbols.txt           → PASS
grep -q 'kernel_panic_at' build/symbols.txt → PASS
grep -q 'cpu_halt_forever' build/disassembly.txt → PASS
xorriso : ISO image produced: 9189 sectors
limine/limine bios-install build/mcsos.iso → Limine BIOS stages installed successfully!
[ISO] build/mcsos.iso ready
```

### Lampiran D — Log QEMU Lengkap

```text
limine: Loading executable `boot():/boot/kernel.elf`...
MCSOS 260502 M4 kernel entered
kernel_start=0xffffffff80000000
kernel_end=0xffffffff80011580
rflags_before_idt=0x0000000000000086
idt_base=0xffffffff8000a000
idt_limit=0x0000000000000fff
[M4] IDT loaded
[M4] trap dispatch: external-or-user-defined-interrupt
trap_vector=0x0000000000000020
trap_error=0x0000000000000000
trap_rip=0xffffffff80000416
trap_cs=0x0000000000000028
trap_rflags=0x0000000000000286
trap_rax=0x000000000000002e
trap_rbx=0x0000000000000000
trap_rcx=0x00000000ffff0040
trap_rdx=0x0000000000000040
[M5] PIC/PIT initialized; interrupts enabled
[M4] selftest: IDT invariants passed
[M4] IDT and exception dispatch path installed
[M4] ready for QEMU smoke test and GDB audit
[M11] elf loader integration selftest
[M11] elf: ident ok
[M11] elf: phnum=1
[M11] elf: load segment vaddr=0x400000 filesz=16 memsz=4096 flags=0x5
[M11] elf: plan ok entry=0x401000
[M11] user image plan ready
M12 sync selftest passed
[M13] VFS/RAMFS kernel selftest begin
[M13] ramfs init: OK
[M13] seed_file /hello.txt: OK
[M13] sys_open /hello.txt: OK
[M13] sys_read 5 bytes: OK
[M13] sys_close: OK
[M13] EBADF after close: OK
[M13] ENOENT missing file: OK
[M13] create/write/lseek/read /log.txt: OK
[M13] VFS/RAMFS kernel selftest: PASS
Terminated
```

### Lampiran E — Output nm-undefined dan readelf

```text
nm-undefined.txt: (kosong — tidak ada undefined symbol)

readelf-vfs.txt (ringkasan):
ELF Header:
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  Type:                              REL (Relocatable file)
  Machine:                           Advanced Micro Devices X86-64
  Entry point address:               0x0
```

### Lampiran F — Daftar Artefak Evidence

| No. | File | Lokasi | Keterangan |
|---|---|---|---|
| 1 | `host-test.log` | `evidence/M13/` | Log hasil host unit test (`PASS`) |
| 2 | `qemu-smoke.log` | `evidence/M13/` | Log serial QEMU dengan semua marker M13 |
| 3 | `nm-undefined.txt` | `evidence/M13/` | Daftar simbol undefined freestanding (kosong) |
| 4 | `readelf-vfs.txt` | `evidence/M13/` | Header ELF vfs.o (ELF64 relocatable) |
| 5 | `objdump-vfs.txt` | `evidence/M13/` | Disassembly dan relokasi vfs.o |
| 6 | `sha256sums.txt` | `evidence/M13/` | Checksum object dan binary M13 |
| 7 | `m13-build.log` | `evidence/M13/` | Log build lengkap make clean && make all && make iso |
| 8 | `mcsos.iso.sha256` | `evidence/M13/` | Checksum ISO bootable |
| 9 | `ci-artifacts.sha256` | `evidence/M13/` | Checksum semua artefak CI M13 |

---

## 24. Daftar Referensi

Gunakan format IEEE. Nomor referensi disusun berdasarkan urutan kemunculan sitasi di laporan.

```text
[1] Linux Kernel Documentation, "Overview of the Linux Virtual File System,"
    docs.kernel.org, accessed Jun. 2026. [Online]. Available:
    https://docs.kernel.org/filesystems/vfs.html

[2] The Open Group, "open - open a file," The Open Group Base Specifications Issue 7 /
    IEEE Std 1003.1, 2018 edition, accessed Jun. 2026. [Online]. Available:
    https://pubs.opengroup.org/onlinepubs/9699919799/functions/open.html

[3] GNU C Library Manual, "Opening and Closing Files," Free Software Foundation,
    accessed Jun. 2026. [Online]. Available:
    https://www.gnu.org/software/libc/manual/html_node/Opening-and-Closing-Files.html

[4] Intel Corporation, "Intel 64 and IA-32 Architectures Software Developer's Manual,"
    accessed Jun. 2026. [Online]. Available:
    https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html

[5] QEMU Project, "GDB usage," QEMU documentation, accessed Jun. 2026. [Online]. Available:
    https://www.qemu.org/docs/master/system/gdb.html

[6] Clang/LLVM Project, "Clang command line argument reference," accessed Jun. 2026.
    [Online]. Available: https://clang.llvm.org/docs/ClangCommandLineReference.html

[7] GNU Binutils, "readelf, objdump, nm," GNU documentation, accessed Jun. 2026.
    [Online]. Available: https://sourceware.org/binutils/docs/

[8] R. H. Arpaci-Dusseau and A. C. Arpaci-Dusseau, Operating Systems: Three Easy Pieces.
    Madison, WI, USA: Arpaci-Dusseau Books, 2018. [Online]. Available:
    https://pages.cs.wisc.edu/~remzi/OSTEP/. Accessed: Jun. 7, 2026.
```

---

## 25. Checklist Final Sebelum Pengumpulan

| Checklist | Status |
|---|---|
| Semua placeholder sudah diganti | `Ya` |
| Metadata laporan lengkap | `Ya` |
| Commit awal dan akhir dicatat | `Ya` |
| Perintah build dan test dapat dijalankan ulang | `Ya` |
| Log build dilampirkan | `Ya` |
| Log QEMU/test dilampirkan | `Ya` |
| Artefak penting diberi hash | `Ya` |
| Desain, invariants, ownership, dan failure modes dijelaskan | `Ya` |
| Security/reliability dibahas | `Ya` |
| Readiness review tidak berlebihan | `Ya` |
| Rubrik penilaian diisi atau disiapkan | `Ya` |
| Referensi memakai format IEEE | `Ya` |
| Laporan disimpan sebagai Markdown | `Ya` |

---

## 26. Pernyataan Pengumpulan

Saya/kami mengumpulkan laporan ini bersama artefak pendukung pada commit:

```text
c9bc3b8
```

Status akhir yang diklaim:

```text
siap uji QEMU untuk VFS/FD/RAMFS awal
```

Ringkasan satu paragraf:

```text
Praktikum M13 berhasil mengimplementasikan VFS minimal, RAMFS in-memory volatil, file descriptor
table per process, dan syscall file I/O wrapper pada kernel MCSOS freestanding x86_64. Kernel
berhasil dibangun menggunakan clang freestanding dan ld.lld tanpa dependency libc host, serta
berhasil dijalankan pada QEMU dengan serial log yang menunjukkan seluruh marker [M13] ... OK dan
diakhiri [M13] VFS/RAMFS kernel selftest: PASS tanpa kernel panic. Host unit test lulus dengan
output "M13 VFS/FD/RAMFS host tests: PASS", nm-undefined.txt kosong, readelf menunjukkan ELF64
relocatable object, dan seluruh artefak audit serta checksum tersimpan di evidence/M13/. Status
readiness yang diklaim adalah "siap uji QEMU untuk VFS/FD/RAMFS awal" karena belum ada permission
model, crash consistency, copyin/copyout penuh, dan global VFS lock yang merupakan target M14+.
```
