# Laporan Praktikum Sistem Operasi Lanjut — MCSOS

**Nama file laporan:** `laporan_praktikum_M12_cute girls.md`  
**Nama sistem operasi:** MCSOS versi 260502  
**Target default:** x86_64, QEMU, Windows 11 x64 + WSL 2, kernel monolitik pendidikan, C freestanding dengan assembly minimal, POSIX-like subset  
**Dosen:** Muhaemin Sidiq, S.Pd., M.Pd.  
**Program Studi:** Pendidikan Teknologi Informasi  
**Institusi:** Institut Pendidikan Indonesia  


---

## 0. Metadata Laporan

| Atribut | Isi |
|---|---|
| Kode praktikum | `M12` |
| Judul praktikum | `Sinkronisasi Kernel Awal: Spinlock, Mutex Kooperatif, Lock-Order Validator, dan Diagnosis Race/Deadlock pada MCSOS` |
| Jenis pengerjaan | `Kelompok` |
| Nama kelompok | `cute girls` |
| Anggota kelompok | `Neng Nagita Salma (25832071004) — Anisa Nur Azfa (25832072003) — Lailatul Zulfa (25832072001)` |
| Kelas | `1A` |
| Tanggal praktikum | `2026-06-06` |
| Tanggal pengumpulan | `2026-07-01` |
| Repository | `https://github.com/nagitasalma47-source/mcsos-M0-cute-girls` |
| Branch | `praktikum/m12-sync` |
| Commit awal | `` d496941 `` |
| Commit akhir | `` 87c3d7a `` |
| Status readiness yang diklaim | `siap uji QEMU` |

---

## 1. Sampul

# Laporan Praktikum `M12`  
## `Sinkronisasi Kernel Awal: Spinlock, Mutex Kooperatif, Lock-Order Validator, dan Diagnosis Race/Deadlock pada MCSOS`

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
Menggunakan bantuan AI assistant (Claude) untuk membantu proses implementasi, debugging, dan validasi praktikum M12, khususnya pada implementasi spinlock atomik, mutex kooperatif, lock-order validator, perbaikan linker error kernel selftest, integrasi kmain.c, pembuatan Makefile.m12, dan validasi QEMU smoke test.

Bagian yang dibantu:
- Pembuatan struktur header mcs_sync.h.
- Implementasi lockdep.c, spinlock.c, dan mutex.c.
- Pembuatan Makefile.m12 dengan target host-test, freestanding, dan audit.
- Debugging linker error undefined symbol kernel_panic dan klog_info.
- Penyesuaian m12_selftest.c agar menggunakan KERNEL_PANIC dan log_writeln sesuai API kernel MCSOS.
- Integrasi m12_sync_selftest() ke kernel/core/kmain.c.
- Uji panic path dengan skenario recursive acquire yang disengaja.
- Validasi QEMU smoke test dan penyimpanan serial log.

Verifikasi mandiri dilakukan dengan:
- make -f Makefile.m12 all CC=clang
- host unit test [PASS] M12 synchronization host tests passed
- audit nm -u, readelf -h, objdump -d
- sha256sum artefak
- QEMU smoke test serial log M12 sync selftest passed
- uji panic path recursive acquire
- commit final ke repository Git
```

---

## 3. Tujuan Praktikum

Tuliskan tujuan teknis dan konseptual praktikum. Tujuan harus dapat diuji.

1. Mengimplementasikan spinlock freestanding x86_64 berbasis operasi atomik acquire/release untuk melindungi critical section pendek pada kernel MCSOS tanpa dependensi libc hosted.

2. Mengimplementasikan mutex kooperatif awal dengan owner semantics yang menolak rekursi dan menolak unlock oleh non-owner sebagai fondasi sinkronisasi task context.

3. Mengimplementasikan lock-order validator sederhana bergaya lockdep untuk mendeteksi rekursi lock, pelepasan lock tidak sesuai urutan LIFO, dan akuisisi lock dengan urutan kelas yang menurun.

4. Melakukan validasi implementasi melalui host unit test multithreaded, freestanding object compile untuk target x86_64-elf, audit `nm -u`, `readelf`, `objdump`, checksum artefak, integrasi kernel selftest, dan QEMU smoke test.

---

## 4. Capaian Pembelajaran Praktikum

Setelah praktikum ini, mahasiswa mampu:

| CPL/CPMK praktikum | Bukti yang harus ditunjukkan |
|---|---|
| Membedakan spinlock, mutex, interrupt masking, dan preemption control | Penjelasan desain pada laporan dan implementasi mcs_spinlock_t dan mcs_mutex_t |
| Menjelaskan mengapa spinlock hanya cocok untuk critical section pendek | Bagian dasar teori dan analisis desain pada laporan |
| Mengimplementasikan spinlock dengan `__atomic_exchange_n` acquire dan `__atomic_store_n` release | Source `kernel/sync/spinlock.c` dan hasil `objdump` menunjukkan instruksi `xchg` dan `pause` |
| Mengimplementasikan mutex kooperatif awal dengan owner checking dan rekursi ditolak | Source `kernel/sync/mutex.c` dan host unit test `test_mutex_owner` PASS |
| Membuat lock-order validator sederhana dengan invariant monoton naik dan release LIFO | Source `kernel/sync/lockdep.c` dan host unit test `test_lockdep_order` dan `test_lockdep_negative` PASS |
| Menulis host unit test race menggunakan thread host | Source `tests/m12_sync_host_test.c` dan `[PASS] M12 synchronization host tests passed` |
| Mengompilasi source sinkronisasi sebagai object freestanding x86_64 | `lockdep.o`, `spinlock.o`, `mutex.o` berhasil dikompilasi dengan target `x86_64-elf` |
| Mengaudit `nm -u`, `readelf -h`, `objdump -d`, dan checksum artefak | `evidence/M12/nm-undefined.txt`, `readelf-lockdep.txt`, `objdump-spinlock.txt`, `sha256sums.txt` |
| Mengintegrasikan selftest sinkronisasi ke kernel MCSOS | `kernel/sync/m12_selftest.c` dipanggil dari `kmain.c` dan serial log `M12 sync selftest passed` muncul |
| Menjelaskan failure modes sinkronisasi: deadlock, recursive acquire, unlock non-owner, lock-order inversion | Bagian 15 laporan dan uji panic path |

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
| M5 | PMM, VMM, page table, kernel heap | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M6 | Thread, scheduler, synchronization | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M7 | Syscall ABI dan user program loader | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M8 | VFS, file descriptor, ramfs | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M9 | Block layer dan device model | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M10 | Persistent filesystem, mcsfs/ext2-like, recovery | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M11 | Networking stack, packet parsing, UDP/TCP subset | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M12 | Sinkronisasi kernel awal: spinlock, mutex, lockdep | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
| M13 | SMP, scalability, lock stress, NUMA-aware preparation | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M14 | Framebuffer, graphics console, visual regression | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M15 | Virtualization/container subset | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
| M16 | Observability, update/rollback, release image, readiness review | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |

Batas cakupan praktikum:

```text
Praktikum M12 berfokus pada implementasi fondasi sinkronisasi kernel awal pada MCSOS berbasis arsitektur x86_64. Cakupan praktikum meliputi implementasi spinlock freestanding berbasis operasi atomik GCC __atomic builtins, mutex kooperatif awal dengan owner semantics, lock-order validator sederhana bergaya lockdep dengan invariant monoton naik dan release LIFO, host unit test multithreaded menggunakan pthread, freestanding object compile untuk target x86_64-elf, audit object menggunakan nm/readelf/objdump, checksum artefak, integrasi kernel selftest ke kmain, dan QEMU smoke test.

Cakupan praktikum meliputi:
- Implementasi mcs_spinlock_t dengan atomic exchange acquire dan store release
- Implementasi mcs_mutex_t dengan owner semantics dan rekursi ditolak
- Implementasi mcs_lockdep_state_t dengan stack lock held dan ordering monoton naik
- Host unit test: test_lockdep_order, test_lockdep_negative, test_spinlock_threads, test_mutex_owner
- Freestanding object compile: lockdep.o, spinlock.o, mutex.o
- Audit: nm -u, readelf -h, objdump -d, sha256sum
- Kernel selftest m12_sync_selftest() terintegrasi ke kmain.c
- QEMU smoke test: serial log M12 sync selftest passed
- Uji panic path: recursive acquire terdeteksi dengan benar

Non-goals / tidak termasuk dalam praktikum:
- Futex, priority inheritance penuh, RCU, rwlock, seqlock
- Lock-free queue
- SMP AP bring-up penuh
- Preemptive scheduler final
- Pembuktian formal race freedom
- Wait queue penuh pada mutex
- irqsave/restore varian spinlock
```

---

## 6. Dasar Teori Ringkas

Praktikum M12 membahas fondasi sinkronisasi kernel pada arsitektur x86_64. Sinkronisasi diperlukan karena kernel memiliki lebih dari satu alur eksekusi konseptual yaitu interrupt, scheduler, syscall, loader, dan allocator yang dapat mengakses struktur data bersama secara bersamaan.

Spinlock adalah lock single-holder yang menunggu dengan busy-wait menggunakan operasi atomik. Spinlock hanya cocok untuk critical section pendek dan tidak boleh digunakan pada jalur yang dapat tidur. Mutex kooperatif adalah owner-aware primitive yang menolak rekursi dan menolak unlock oleh non-owner. Lock-order validator mendeteksi pelanggaran urutan akuisisi lock yang dapat menyebabkan deadlock.

### 6.1 Konsep Sistem Operasi yang Diuji

```text
Praktikum M12 menguji konsep sinkronisasi kernel pada arsitektur x86_64 melalui implementasi spinlock, mutex kooperatif, dan lock-order validator pada kernel MCSOS.

Spinlock menggunakan operasi atomik __atomic_exchange_n dengan memory order acquire pada lock dan __atomic_store_n dengan memory order release pada unlock. Pada x86_64, busy-wait loop menggunakan instruksi pause untuk mengurangi tekanan pada pipeline CPU.

Mutex kooperatif menggunakan compare-and-exchange atomik untuk akuisisi dan menyimpan owner ID untuk validasi. Rekursi lock oleh owner yang sama ditolak dengan MCS_SYNC_EDEADLK. Unlock oleh non-owner ditolak dengan MCS_SYNC_EPERM.

Lock-order validator menggunakan stack held_class[] untuk merekam urutan lock yang sedang dipegang. Akuisisi lock dengan kelas lebih rendah dari lock paling atas ditolak sebagai lock-order inversion. Rekursi lock yang sama ditolak sebagai recursive acquire. Release harus mengikuti urutan LIFO.

Praktikum menggunakan QEMU sebagai emulator sistem x86_64 dan pthread sebagai framework multithreading untuk host unit test.
```

### 6.2 Konsep Arsitektur x86_64 yang Relevan

| Konsep | Relevansi pada praktikum | Bukti/verifikasi |
|---|---|---|
| Operasi atomik x86_64 | Digunakan untuk implementasi spinlock acquire/release dan mutex CAS | `objdump` menunjukkan instruksi `xchg` pada `mcs_spin_try_lock` |
| Instruksi `pause` | Digunakan pada busy-wait loop spinlock agar tidak terlalu agresif terhadap pipeline | `objdump` menunjukkan instruksi `pause` pada `mcs_spin_lock` |
| Memory ordering acquire/release | Digunakan untuk memastikan akses data dalam critical section terlihat dengan urutan yang benar | `__atomic_exchange_n` acquire dan `__atomic_store_n` release pada source |
| GCC `__atomic` builtins | Digunakan sebagai interface atomik freestanding tanpa dependensi libc | Source `spinlock.c` dan `mutex.c`, audit `nm -u` tidak ada unresolved symbol |
| ELF64 relocatable object | Format object freestanding x86_64 yang dihasilkan oleh clang | `readelf -h` menunjukkan `ELF64` dan `Advanced Micro Devices X86-64` |

### 6.3 Konsep Implementasi Freestanding

| Aspek | Keputusan praktikum |
|---|---|
| Bahasa | C17 freestanding untuk kernel; C17 hosted untuk host unit test |
| Runtime | Tanpa hosted libc pada object kernel, menggunakan GCC atomic builtins |
| ABI | x86_64 System V ABI untuk kernel |
| Compiler flags kritis | `-ffreestanding`, `-fno-builtin`, `-fno-stack-protector`, `-mno-red-zone`, `-target x86_64-elf` |
| Risiko undefined behavior | Null pointer dereference pada lock, recursive acquire, unlock non-owner |

### 6.4 Referensi Teori yang Digunakan

| No. | Sumber | Bagian yang digunakan | Alasan relevansi |
|---|---|---|---|
| 1 | Intel SDM | Memory ordering, instruksi atomik x86_64, instruksi pause | Referensi utama untuk implementasi spinlock dan mutex atomik |
| 2 | Linux Kernel Documentation — Lock types and their rules | Kategori lock, aturan konteks spinlock/mutex, owner semantics | Referensi desain spinlock dan mutex |
| 3 | Linux Kernel Documentation — lockdep design | Runtime locking correctness validator, lock class, ordering | Referensi desain lock-order validator |
| 4 | GCC `__atomic` builtins documentation | acquire/release/relaxed, exchange, compare-exchange | Referensi implementasi atomik freestanding |
| 5 | Clang command line reference | Freestanding compile, target triple, flags kernel | Referensi kompilasi object freestanding |
| 6 | GNU Binutils | nm, readelf, objdump | Referensi audit object dan symbol |

---

## 7. Lingkungan Praktikum

### 7.1 Host dan Target

| Komponen | Nilai |
|---|---|
| Host OS | Windows 11 x64 |
| Lingkungan build | WSL 2 Ubuntu 24.04.4 LTS (Noble Numbat) |
| Target ISA | x86_64 |
| Target ABI kernel | x86_64-unknown-none-elf |
| Target ABI host test | x86_64-pc-linux-gnu (hosted) |
| Emulator | QEMU x86_64 |
| Firmware emulator | Limine bootloader + BIOS/UEFI image |
| Debugger | GNU GDB 15.1 |
| Build system | GNU Make 4.3 |
| Bahasa utama | C17 freestanding (kernel) dan C17 hosted (host test) |

### 7.2 Versi Toolchain

```text
2026-06-06T22:11:32+07:00
Linux ASUS 6.6.87.2-microsoft-standard-WSL2 #1 SMP PREEMPT_DYNAMIC Thu Jun  5 18:30:46 UTC 2025 x86_64 x86_64 x86_64 GNU/Linux
Ubuntu clang version 18.1.3 (1ubuntu1)
cc (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
GNU Make 4.3
QEMU emulator version 8.2.2 (Debian 1:8.2.2+ds-0ubuntu1.16)
GNU gdb (Ubuntu 15.1-1ubuntu1~24.04.1) 15.1
GNU nm (GNU Binutils for Ubuntu) 2.42
GNU readelf (GNU Binutils for Ubuntu) 2.42
GNU objdump (GNU Binutils for Ubuntu) 2.42
git version 2.43.0
```

### 7.3 Lokasi Repository

| Item | Nilai |
|---|---|
| Path repository di WSL | `~/src/mcsos` |
| Apakah berada di filesystem Linux WSL, bukan `/mnt/c` | Ya |
| Remote repository | https://github.com/nagitasalma47-source/mcsos-M0-cute-girls |
| Branch | `praktikum/m12-sync` |
| Commit hash awal | `d496941` |
| Commit hash akhir | `87c3d7a` |

---

## 8. Repository dan Struktur File

### 8.1 Struktur Direktori yang Relevan

```text
mcsos/
├── include/
│   └── mcs_sync.h
├── kernel/
│   ├── core/
│   │   └── kmain.c
│   └── sync/
│       ├── lockdep.c
│       ├── spinlock.c
│       ├── mutex.c
│       └── m12_selftest.c
├── tests/
│   └── m12_sync_host_test.c
├── evidence/
│   └── M12/
│       ├── preflight.log
│       ├── m12-build.log
│       ├── host-test.log
│       ├── nm-undefined.txt
│       ├── readelf-lockdep.txt
│       ├── objdump-spinlock.txt
│       ├── sha256sums.txt
│       └── qemu/
│           ├── kernel-build.log
│           ├── serial.log
│           ├── serial-panic-test.log
│           └── serial-final.log
├── build/
│   └── m12/
│       ├── lockdep.o
│       ├── spinlock.o
│       ├── mutex.o
│       └── m12_sync_host_test
├── Makefile.m12
└── Makefile
```

### 8.2 File yang Dibuat atau Diubah

| File | Jenis perubahan | Alasan perubahan | Risiko |
|---|---|---|---|
| `include/mcs_sync.h` | Baru | Mendefinisikan kontrak data structure, error code, dan deklarasi API sinkronisasi | Rendah, hanya header interface |
| `kernel/sync/lockdep.c` | Baru | Implementasi lock-order validator dengan stack held_class dan ordering monoton naik | Sedang, memengaruhi validasi urutan lock |
| `kernel/sync/spinlock.c` | Baru | Implementasi spinlock atomik dengan acquire/release dan loop pause | Tinggi, digunakan di critical section kernel |
| `kernel/sync/mutex.c` | Baru | Implementasi mutex kooperatif dengan owner semantics dan rekursi ditolak | Tinggi, memengaruhi owner-aware locking |
| `kernel/sync/m12_selftest.c` | Baru | Self-test kernel untuk validasi sinkronisasi saat boot | Sedang, dipanggil dari kmain setelah console aktif |
| `tests/m12_sync_host_test.c` | Baru | Host unit test: lockdep order, lockdep negative, spinlock threads, mutex owner | Rendah, hanya digunakan saat testing host |
| `Makefile.m12` | Baru | Menyediakan target host-test, freestanding, dan audit untuk M12 | Rendah, tidak memengaruhi build kernel utama |
| `kernel/core/kmain.c` | Ubah | Menambahkan deklarasi dan pemanggilan m12_sync_selftest() | Sedang, memengaruhi urutan boot kernel |

### 8.3 Ringkasan Diff

```bash
git log --oneline -3
```

Output:

```text
87c3d7a (HEAD -> praktikum/m12-sync) M12: add kmain integration and QEMU smoke test evidence
2946884 M12: implement spinlock, cooperative mutex, lockdep validator, host unit test, freestanding audit, and kernel selftest
d496941 (praktikum-m11-elf-user-loader) M11: implement ELF64 user loader, process image plan, host unit test, kernel integration, and QEMU smoke test
```

---

## 9. Desain Teknis

### 9.1 Masalah yang Diselesaikan

Praktikum M12 menyelesaikan masalah belum tersedianya fondasi sinkronisasi eksplisit pada kernel MCSOS. Setelah kernel memiliki lebih dari satu alur eksekusi konseptual yaitu interrupt, scheduler, syscall, loader, dan allocator, diperlukan primitive sinkronisasi yang dapat melindungi struktur data bersama dari race condition dan deadlock.

Sebelum M12, kernel tidak memiliki spinlock atomik, mutex owner-aware, maupun mekanisme deteksi pelanggaran urutan lock. Kondisi ini berpotensi menyebabkan data race pada PMM, VMM, heap, dan runqueue saat subsystem tersebut diakses dari jalur eksekusi yang berbeda.

Melalui implementasi M12, kernel menjadi memiliki fondasi sinkronisasi yang dapat diaudit, diuji, dan diperluas untuk modul berikutnya.

### 9.2 Keputusan Desain

| Keputusan | Alternatif yang dipertimbangkan | Alasan memilih | Konsekuensi |
|---|---|---|---|
| Menggunakan GCC `__atomic` builtins | Inline assembly LOCK XCHG langsung | Lebih portabel dan dapat diaudit tanpa undefined behavior | Bergantung pada compiler untuk menghasilkan instruksi atomik yang benar |
| Spinlock dengan dua-fase: try_lock + inner loop relaxed | Langsung spin pada exchange | Mengurangi cache-line bouncing saat menunggu | Sedikit lebih kompleks pada loop |
| Mutex menyimpan owner sebagai uint64_t | Pointer thread atau integer kecil | Fleksibel untuk diisi thread ID, TCB pointer, atau ID numerik | Caller bertanggung jawab memilih owner ID yang unik dan bukan nol |
| Lock-order validator dengan stack array statis | Hash table atau linked list dinamis | Tidak memerlukan alokasi heap, aman digunakan sebelum heap siap | Maksimum 16 lock nested |
| Release LIFO pada lockdep | Release sembarang urutan | Mendeteksi out-of-order release yang dapat mengindikasikan bug | Pembatasan ini sengaja lebih ketat dari lockdep Linux |
| Objek lock tidak dialokasikan dinamis | Alokasi dari heap kernel | Mencegah allocator recursion dan aman sebelum heap penuh siap | Pemilik memori lock adalah subsystem pemanggil |

### 9.3 Arsitektur Ringkas

```text
MCSOS M12 synchronization layer

thread/trap context
   |
   |-- lockdep_state per-thread/per-context
   |       |-- held_class[]
   |       |-- depth
   |       '-- violation_count
   |
   |-- spinlock: atomic exchange acquire / store release
   |       '-- critical section pendek, non-blocking
   |
   '-- mutex: try-lock owner-aware
           '-- kandidat wait queue pada modul berikutnya

M12 evidence path:
source -> host unit test -> freestanding object -> nm/readelf/objdump -> checksum -> QEMU smoke integration
```

### 9.4 Kontrak Antarmuka

| Antarmuka | Pemanggil | Penerima | Precondition | Postcondition | Error path |
|---|---|---|---|---|---|
| `mcs_spin_init()` | Subsystem kernel | Spinlock subsystem | Pointer lock tidak null | Lock dalam keadaan bebas (locked=0) | Jika null, return tanpa aksi |
| `mcs_spin_lock()` | Subsystem kernel | Spinlock subsystem | Lock sudah diinisialisasi | Lock dimiliki caller, critical section aktif | Busy-wait hingga berhasil |
| `mcs_spin_unlock()` | Subsystem kernel | Spinlock subsystem | Caller adalah pemegang lock | Lock bebas (locked=0) | Jika null, return tanpa aksi |
| `mcs_mutex_try_lock()` | Subsystem kernel | Mutex subsystem | owner_id != 0 | Lock dimiliki owner_id | MCS_SYNC_EBUSY, MCS_SYNC_EDEADLK, MCS_SYNC_EINVAL |
| `mcs_mutex_unlock()` | Subsystem kernel | Mutex subsystem | Caller adalah owner | Lock bebas, owner dihapus | MCS_SYNC_EPERM jika bukan owner, MCS_SYNC_EINVAL jika null |
| `mcs_lockdep_before_acquire()` | Subsystem kernel | Lockdep subsystem | state tidak null, class_id != 0 | class_id ditambahkan ke held_class[] | MCS_SYNC_EDEADLK, MCS_SYNC_EOVERFLOW |
| `mcs_lockdep_after_release()` | Subsystem kernel | Lockdep subsystem | class_id paling atas sesuai | class_id dihapus dari held_class[] | MCS_SYNC_EDEADLK, MCS_SYNC_EPERM |

### 9.5 Struktur Data

| Struktur | Field penting | Owner | Invariant |
|---|---|---|---|
| `mcs_spinlock_t` | `locked` (volatile uint32_t), `class_id`, `name` | Subsystem pemanggil | `locked==0` berarti bebas; `locked==1` berarti dimiliki |
| `mcs_mutex_t` | `locked` (volatile uint32_t), `owner` (uint64_t), `class_id`, `name` | Subsystem pemanggil | `locked==1` harus diikuti `owner!=0`; hanya owner yang boleh unlock |
| `mcs_lockdep_state_t` | `held_class[]`, `held_name[]`, `depth`, `violation_count` | Per-thread atau per-context | Stack LIFO; kelas monoton naik; setiap pelanggaran menaikkan violation_count |

### 9.6 Invariants

1. Nilai `locked == 0` pada spinlock berarti lock bebas; `locked == 1` berarti lock sedang dimiliki tepat satu eksekusi logis.
2. Acquire spinlock harus memberikan ordering acquire; release harus memberikan ordering release.
3. Null pointer pada semua fungsi lock tidak boleh menyebabkan dereference; fungsi return tanpa aksi atau return error.
4. Mutex hanya dapat dilepas oleh owner yang sama; non-owner yang mencoba unlock mendapat MCS_SYNC_EPERM.
5. Owner yang sama tidak boleh mengambil mutex yang sama secara rekursif; recursive acquire menghasilkan MCS_SYNC_EDEADLK.
6. Stack `held_class[]` pada lockdep mengikuti urutan monoton naik; akuisisi kelas lebih rendah dari kelas paling atas ditolak.
7. Release lockdep harus mengikuti urutan LIFO; release di luar urutan menghasilkan MCS_SYNC_EDEADLK.
8. Setiap pelanggaran lockdep menaikkan `violation_count` sebagai bukti observability.

### 9.7 Ownership, Locking, dan Concurrency

| Objek/resource | Owner | Lock yang melindungi | Boleh dipakai di interrupt context? | Catatan |
|---|---|---|---|---|
| `mcs_spinlock_t` | Subsystem pemanggil | Dirinya sendiri (atomic) | Ya, dengan catatan | Critical section harus pendek; belum ada irqsave/restore |
| `mcs_mutex_t` | Owner yang memegang lock | Dirinya sendiri (atomic CAS) | Tidak | Mutex tidak boleh diambil dari interrupt context |
| `mcs_lockdep_state_t` | Per-thread atau per-context | Tidak ada locking internal | Tidak | Harus digunakan dari konteks yang sama secara konsisten |
| `boot_stats_lock` pada selftest | Kernel boot context | `boot_stats_lock` itu sendiri | Tidak | Hanya digunakan pada fase boot sebelum scheduler aktif |

Lock order yang berlaku:

```text
Lock hierarchy M12 (class ID monoton naik):
10  - boot_stats (hanya selftest/early log)
20  - PMM
30  - VMM/page table
40  - Heap
50  - Thread/TCB
60  - Runqueue
70  - Syscall table/policy
80  - Loader/process image
90  - VFS awal
100 - Device model

Aturan: lock hanya boleh diambil dari class lebih rendah ke class lebih tinggi.
Perubahan hierarchy wajib dicatat dan diuji dengan negative test.
```

### 9.8 Memory Safety dan Undefined Behavior Risk

| Risiko | Lokasi | Mitigasi | Bukti |
|---|---|---|---|
| Null pointer dereference pada lock | Semua fungsi lock | Pengecekan pointer != 0 di awal setiap fungsi | Code review dan host unit test |
| Recursive acquire spinlock | `mcs_spin_lock` | Lockdep validator mendeteksi dan menolak | Host test `test_lockdep_negative` PASS |
| Unlock non-owner pada mutex | `mcs_mutex_unlock` | Pengecekan owner_id sebelum release | Host test `test_mutex_owner` PASS |
| Stack overflow lockdep | `mcs_lockdep_before_acquire` | Pengecekan `depth >= MCS_LOCKDEP_MAX_HELD` menghasilkan MCS_SYNC_EOVERFLOW | Code review |
| Unresolved runtime helper `__atomic_*` | Object freestanding | Penggunaan tipe 32-bit/64-bit natural yang lock-free | `nm -u` tidak ada unresolved symbol |
| Race pada counter host test | `g_counter` | Dilindungi `g_counter_lock` spinlock | Host test `test_spinlock_threads` PASS: counter exact |

### 9.9 Security Boundary

| Boundary | Data tidak tepercaya | Validasi yang dilakukan | Failure mode aman |
|---|---|---|---|
| API lock dengan pointer null | Pointer lock dari caller kernel | Pengecekan null di awal setiap fungsi | Return tanpa aksi atau return error |
| owner_id pada mutex | ID dari caller | Pengecekan owner_id != 0 pada try_lock dan unlock | MCS_SYNC_EINVAL |
| class_id pada lockdep | Class ID dari caller | Pengecekan class_id != 0 dan urutan monoton naik | MCS_SYNC_EDEADLK atau MCS_SYNC_EINVAL |
| Interrupt context mengambil mutex | Interrupt handler kernel | Desain: mutex tidak boleh diambil dari interrupt context | Tidak ada enforcement otomatis pada M12; documented constraint |

---

## 10. Langkah Kerja Implementasi

### Langkah 1 — Preflight dan Branch M12

Maksud langkah:

```text
Memastikan toolchain, Git, dan artefak dasar tersedia sebelum implementasi dimulai.
Membuat branch terpisah untuk M12 agar rollback mudah dilakukan.
```

Perintah:

```bash
mkdir -p evidence/M12
{
  date -Is
  uname -a
  clang --version | head -n 1 || true
  cc --version | head -n 1 || true
  make --version | head -n 1
  git rev-parse --short HEAD
  git status --short
} | tee evidence/M12/preflight.log

git checkout -b praktikum/m12-sync
mkdir -p include kernel/sync tests scripts evidence/M12
```

Output ringkas:

```text
2026-06-06T22:11:32+07:00
Linux ASUS 6.6.87.2-microsoft-standard-WSL2 ...
Ubuntu clang version 18.1.3 (1ubuntu1)
cc (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
GNU Make 4.3
d496941
Switched to a new branch 'praktikum/m12-sync'
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `preflight.log` | `evidence/M12/preflight.log` | Rekaman versi toolchain dan status awal |

Indikator berhasil:

```text
Branch praktikum/m12-sync aktif dan preflight.log tersimpan dengan commit hash tercatat.
```

---

### Langkah 2 — Header `include/mcs_sync.h`

Maksud langkah:

```text
Mendefinisikan kontrak data structure, error code, dan deklarasi API sinkronisasi.
Header tidak boleh bergantung pada libc hosted.
```

Perintah:

```bash
cat > include/mcs_sync.h <<'EOF'
... (isi header)
EOF
wc -l include/mcs_sync.h
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `mcs_sync.h` | `include/mcs_sync.h` | Deklarasi API spinlock, mutex, lockdep, dan error code |

Indikator berhasil:

```text
Header 87 baris berhasil dibuat dengan semua deklarasi fungsi dan tipe data lengkap.
```

---

### Langkah 3 — Implementasi `kernel/sync/lockdep.c`

Maksud langkah:

```text
Mengimplementasikan lock-order validator dengan stack held_class, ordering monoton naik,
release LIFO, dan pencatatan violation_count.
```

Perintah:

```bash
cat > kernel/sync/lockdep.c <<'EOF'
... (implementasi)
EOF
wc -l kernel/sync/lockdep.c
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `lockdep.c` | `kernel/sync/lockdep.c` | Implementasi lock-order validator |

Indikator berhasil:

```text
lockdep.c 102 baris berhasil dibuat dengan fungsi mcs_lockdep_init, mcs_lockdep_is_held,
mcs_lockdep_before_acquire, dan mcs_lockdep_after_release.
```

---

### Langkah 4 — Implementasi `kernel/sync/spinlock.c`

Maksud langkah:

```text
Mengimplementasikan spinlock atomik menggunakan __atomic_exchange_n acquire pada lock
dan __atomic_store_n release pada unlock. Loop spin menggunakan instruksi pause.
```

Perintah:

```bash
cat > kernel/sync/spinlock.c <<'EOF'
... (implementasi)
EOF
wc -l kernel/sync/spinlock.c
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `spinlock.c` | `kernel/sync/spinlock.c` | Implementasi spinlock atomik |

Indikator berhasil:

```text
spinlock.c 74 baris berhasil dibuat dengan fungsi mcs_spin_init, mcs_spin_try_lock,
mcs_spin_lock, mcs_spin_unlock, dan mcs_spin_is_locked.
```

---

### Langkah 5 — Implementasi `kernel/sync/mutex.c`

Maksud langkah:

```text
Mengimplementasikan mutex kooperatif awal dengan compare-and-exchange atomik,
owner ID validation, rekursi ditolak, dan unlock hanya oleh owner.
```

Perintah:

```bash
cat > kernel/sync/mutex.c <<'EOF'
... (implementasi)
EOF
wc -l kernel/sync/mutex.c
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `mutex.c` | `kernel/sync/mutex.c` | Implementasi mutex kooperatif |

Indikator berhasil:

```text
mutex.c 120 baris berhasil dibuat dengan fungsi mcs_mutex_init, mcs_mutex_try_lock,
mcs_mutex_unlock, mcs_mutex_is_locked, dan mcs_mutex_owner.
```

---

### Langkah 6 — Host Unit Test dan Build M12

Maksud langkah:

```text
Membuat host unit test lengkap dengan empat fungsi test dan menjalankan
full build menggunakan Makefile.m12.
```

Perintah:

```bash
make -f Makefile.m12 clean
make -f Makefile.m12 all CC=clang 2>&1 | tee evidence/M12/m12-build.log
```

Output ringkas:

```text
[PASS] M12 synchronization host tests passed
nm -u build/m12/lockdep.o build/m12/spinlock.o build/m12/mutex.o | tee build/m12/nm-undefined.txt
(kosong - tidak ada unresolved symbol)
ELF Header:
  Class: ELF64
  Machine: Advanced Micro Devices X86-64
objdump -d build/m12/spinlock.o
  xchg %eax,(%rdi)
  pause
sha256sum tersimpan
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `m12_sync_host_test` | `build/m12/m12_sync_host_test` | Host unit test executable |
| `lockdep.o` | `build/m12/lockdep.o` | Freestanding object lockdep |
| `spinlock.o` | `build/m12/spinlock.o` | Freestanding object spinlock |
| `mutex.o` | `build/m12/mutex.o` | Freestanding object mutex |
| `nm-undefined.txt` | `build/m12/nm-undefined.txt` | Audit undefined symbol |
| `readelf-lockdep.txt` | `build/m12/readelf-lockdep.txt` | Audit ELF header lockdep |
| `objdump-spinlock.txt` | `build/m12/objdump-spinlock.txt` | Audit disassembly spinlock |
| `sha256sums.txt` | `build/m12/sha256sums.txt` | Checksum artefak |

Indikator berhasil:

```text
[PASS] M12 synchronization host tests passed
nm -u menunjukkan tidak ada unresolved symbol
readelf menunjukkan ELF64 dan Advanced Micro Devices X86-64
objdump menunjukkan instruksi xchg dan pause
```

---

### Langkah 7 — Kernel Integration dan QEMU Smoke Test

Maksud langkah:

```text
Mengintegrasikan m12_sync_selftest() ke kmain.c dan memvalidasi melalui QEMU smoke test.
Uji panic path juga dilakukan dengan skenario recursive acquire yang disengaja.
```

Perintah:

```bash
# Integrasi ke kmain
sed -i 's/extern void m11_kernel_selftest(void);/extern void m11_kernel_selftest(void);\nextern void m12_sync_selftest(void);/' kernel/core/kmain.c
sed -i 's/m11_kernel_selftest();/m11_kernel_selftest();\n    m12_sync_selftest();/' kernel/core/kmain.c

# Build dan ISO
make all 2>&1 | tee evidence/M12/qemu/kernel-build.log
ln -sf mcsos-m5.elf build/kernel.elf
bash tools/scripts/make_iso.sh 2>&1 | tail -3

# QEMU smoke test
qemu-system-x86_64 \
  -cdrom build/mcsos.iso \
  -serial file:evidence/M12/qemu/serial-final.log \
  -no-reboot -no-shutdown \
  -display none &
sleep 6
kill %1 2>/dev/null
cat evidence/M12/qemu/serial-final.log
```

Output ringkas serial log:

```text
limine: Loading executable `boot():/boot/kernel.elf`...
MCSOS 260502 M4 kernel entered
[M5] PIC/PIT initialized; interrupts enabled
[M4] selftest: IDT invariants passed
[M4] IDT and exception dispatch path installed
[M4] ready for QEMU smoke test and GDB audit
[M11] user image plan ready
M12 sync selftest passed
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `m12_selftest.c` | `kernel/sync/m12_selftest.c` | Kernel selftest sinkronisasi |
| `serial-final.log` | `evidence/M12/qemu/serial-final.log` | Serial log QEMU normal |
| `serial-panic-test.log` | `evidence/M12/qemu/serial-panic-test.log` | Serial log uji panic path |
| `kernel-build.log` | `evidence/M12/qemu/kernel-build.log` | Log build kernel utama |

Indikator berhasil:

```text
M12 sync selftest passed muncul pada serial log QEMU.
Uji panic path: M12 panic path verified: recursive acquire detected correctly.
```

---

## 11. Checkpoint Buildable

| Checkpoint | Perintah | Expected result | Status |
|---|---|---|---|
| Header sync | `test -f include/mcs_sync.h` | File header tersedia | PASS |
| Host unit test | `make -f Makefile.m12 host-test` | `[PASS] M12 synchronization host tests passed` | PASS |
| Freestanding object | `make -f Makefile.m12 freestanding CC=clang` | `lockdep.o`, `spinlock.o`, `mutex.o` dibuat | PASS |
| Object audit | `make -f Makefile.m12 audit CC=clang` | nm, readelf, objdump, checksum tersimpan | PASS |
| Kernel build | `make all` | Kernel ELF berhasil dibangun dengan sync objects | PASS |
| QEMU smoke | Serial log QEMU | `M12 sync selftest passed` muncul | PASS |
| Panic path | Uji recursive acquire | `M12 panic path verified` muncul | PASS |
| Git commit | `git log --oneline -3` | Commit `87c3d7a` tercatat | PASS |

Catatan checkpoint:

```text
Seluruh checkpoint M12 berhasil dijalankan. Kernel dapat dibangun dari clean checkout,
host unit test lulus dengan empat fungsi test, freestanding object berhasil dibuat dan
diaudit, dan kernel berhasil boot pada QEMU dengan M12 sync selftest passed tercatat
pada serial log. Panic path juga tervalidasi dengan benar.
```

---

## 12. Perintah Uji dan Validasi

### 12.1 Build Test

```bash
make -f Makefile.m12 clean
make -f Makefile.m12 all CC=clang 2>&1 | tee evidence/M12/m12-build.log
```

Hasil:

```text
[PASS] M12 synchronization host tests passed
Object freestanding lockdep.o, spinlock.o, mutex.o berhasil dibuat.
Audit nm, readelf, objdump, dan sha256sum tersimpan.
```

Status: `PASS`

### 12.2 Static Inspection

```bash
nm -u build/m12/lockdep.o build/m12/spinlock.o build/m12/mutex.o
readelf -h build/m12/lockdep.o
objdump -d build/m12/spinlock.o
```

Hasil penting:

```text
nm -u: tidak ada unresolved symbol pada ketiga object
readelf: Class ELF64, Machine Advanced Micro Devices X86-64, Type REL (Relocatable file)
objdump: instruksi xchg pada mcs_spin_try_lock, instruksi pause pada mcs_spin_lock
```

Status: `PASS`

### 12.3 QEMU Smoke Test

```bash
qemu-system-x86_64 \
  -cdrom build/mcsos.iso \
  -serial file:evidence/M12/qemu/serial-final.log \
  -no-reboot -no-shutdown \
  -display none
```

Hasil:

```text
M12 sync selftest passed
```

Status: `PASS`

### 12.4 Uji Panic Path

```bash
# Modifikasi selftest untuk trigger recursive acquire
# Build, ISO, QEMU
cat evidence/M12/qemu/serial-panic-test.log
```

Hasil:

```text
M12 panic path verified: recursive acquire detected correctly
M12 sync selftest passed
```

Status: `PASS`

### 12.5 Unit Test

```bash
make -f Makefile.m12 host-test
```

Hasil:

```text
[PASS] M12 synchronization host tests passed
```

Status: `PASS`

### 12.6 Stress atau Fuzzing

```text
Host unit test test_spinlock_threads menjalankan 4 thread dengan masing-masing 25000 iterasi
increment counter yang dilindungi spinlock. Total increment yang diharapkan adalah 100000.
Hasil counter exact = THREADS * ITERS = 4 * 25000 = 100000 terpenuhi.
```

Status: `PASS`

### 12.7 Visual Evidence

| Screenshot | Lokasi file | Keterangan |
|---|---|---|
| QEMU serial log M12 | `Screenshots-M12-qemuserial-final.log` | Menunjukkan `M12 sync selftest passed` pada serial output |
| Panic path test | `Screenshots-M12-qemuserial-panic-test.log` | Menunjukkan `M12 panic path verified: recursive acquire detected correctly` |


---

## 13. Hasil Uji

### 13.1 Tabel Ringkasan Hasil

| No. | Uji | Expected result | Actual result | Status | Evidence |
|---|---|---|---|---|---|
| 1 | Host unit test lockdep order | Acquire rank 10 dan 20 berhasil; release berurutan LIFO | test_lockdep_order PASS | PASS | `evidence/M12/host-test.log` |
| 2 | Host unit test lockdep negative | Reject descending rank dan recursion; violation_count == 2 | test_lockdep_negative PASS | PASS | `evidence/M12/host-test.log` |
| 3 | Host unit test spinlock threads | Counter = 4 * 25000 = 100000 exact | test_spinlock_threads PASS | PASS | `evidence/M12/host-test.log` |
| 4 | Host unit test mutex owner | Owner semantics, recursive reject, non-owner unlock reject | test_mutex_owner PASS | PASS | `evidence/M12/host-test.log` |
| 5 | Freestanding compile | lockdep.o, spinlock.o, mutex.o berhasil dibuat | Object berhasil dibuat | PASS | `build/m12/` |
| 6 | nm -u audit | Tidak ada unresolved symbol | Output kosong untuk ketiga object | PASS | `evidence/M12/nm-undefined.txt` |
| 7 | readelf audit | ELF64, x86-64, REL | Class ELF64, Machine AMD X86-64, Type REL | PASS | `evidence/M12/readelf-lockdep.txt` |
| 8 | objdump audit | Instruksi xchg dan pause muncul | xchg pada try_lock, pause pada loop spin | PASS | `evidence/M12/objdump-spinlock.txt` |
| 9 | Kernel build dengan sync | Kernel ELF berhasil dibangun | mcsos-m5.elf berhasil dibuat | PASS | `evidence/M12/qemu/kernel-build.log` |
| 10 | QEMU smoke test | M12 sync selftest passed muncul | Serial log menunjukkan M12 sync selftest passed | PASS | `evidence/M12/qemu/serial-final.log` |
| 11 | Uji panic path | Recursive acquire terdeteksi | M12 panic path verified muncul | PASS | `evidence/M12/qemu/serial-panic-test.log` |

### 13.2 Log Penting

```text
=== Host Unit Test ===
[PASS] M12 synchronization host tests passed

=== QEMU Serial Log (Normal) ===
limine: Loading executable `boot():/boot/kernel.elf`...
MCSOS 260502 M4 kernel entered
kernel_start=0xffffffff80000000
kernel_end=0xffffffff8000c160
[M4] IDT loaded
[M5] PIC/PIT initialized; interrupts enabled
[M4] selftest: IDT invariants passed
[M4] IDT and exception dispatch path installed
[M4] ready for QEMU smoke test and GDB audit
[M11] elf loader integration selftest
[M11] user image plan ready
M12 sync selftest passed

=== QEMU Serial Log (Panic Path Test) ===
M12 panic path verified: recursive acquire detected correctly
M12 sync selftest passed

=== objdump spinlock (ringkasan) ===
0000000000000020 <mcs_spin_try_lock>:
  2e: 87 07   xchg %eax,(%rdi)
0000000000000040 <mcs_spin_lock>:
  70: f3 90   pause
```

### 13.3 Artefak Bukti

| Artefak | Path | Fungsi |
|---|---|---|
| `lockdep.o` | `build/m12/lockdep.o` | Freestanding object lockdep |
| `spinlock.o` | `build/m12/spinlock.o` | Freestanding object spinlock |
| `mutex.o` | `build/m12/mutex.o` | Freestanding object mutex |
| `m12_sync_host_test` | `build/m12/m12_sync_host_test` | Host unit test executable |
| `sha256sums.txt` | `evidence/M12/sha256sums.txt` | Checksum keempat artefak |
| `nm-undefined.txt` | `evidence/M12/nm-undefined.txt` | Bukti tidak ada unresolved symbol |
| `readelf-lockdep.txt` | `evidence/M12/readelf-lockdep.txt` | Bukti ELF64 x86-64 |
| `objdump-spinlock.txt` | `evidence/M12/objdump-spinlock.txt` | Bukti instruksi xchg dan pause |
| `serial-final.log` | `evidence/M12/qemu/serial-final.log` | Serial log QEMU normal |
| `serial-panic-test.log` | `evidence/M12/qemu/serial-panic-test.log` | Serial log uji panic path |

---

## 14. Analisis Teknis

### 14.1 Analisis Keberhasilan

```text
Implementasi M12 berhasil karena ketiga komponen sinkronisasi yaitu spinlock, mutex, dan
lockdep dapat dikompilasi sebagai object freestanding x86_64 tanpa dependensi runtime
hosted dan lulus host unit test multithreaded.

Spinlock berhasil melindungi counter bersama pada test 4 thread dengan 25000 iterasi masing-masing
dan menghasilkan counter exact 100000 tanpa race condition. Mutex berhasil menolak recursive acquire
dan unlock oleh non-owner sesuai semantics yang direncanakan. Lock-order validator berhasil mendeteksi
descending rank acquire dan recursive acquire serta mencatat violation_count dengan benar.

Pada level kernel, integrasi m12_sync_selftest() ke kmain.c berhasil setelah dilakukan penyesuaian
nama fungsi panic dan log dari API kernel MCSOS yang sebenarnya yaitu KERNEL_PANIC macro dan
log_writeln. QEMU smoke test menunjukkan M12 sync selftest passed muncul pada serial log
setelah seluruh subsystem M4 sampai M11 selesai diinisialisasi.
```

### 14.2 Analisis Kegagalan atau Perbedaan Hasil

```text
Ditemukan dua kegagalan linker saat pertama kali mengintegrasikan m12_selftest.c ke kernel:

1. Undefined symbol kernel_panic — selftest awalnya menggunakan nama fungsi kernel_panic yang tidak
   ada di MCSOS. Setelah memeriksa kernel/include/mcsos/kernel/panic.h, nama yang benar adalah
   macro KERNEL_PANIC yang memanggil kernel_panic_at. Selftest diperbaiki menggunakan KERNEL_PANIC.

2. Undefined symbol klog_info — selftest awalnya menggunakan nama klog_info yang tidak ada.
   Setelah memeriksa kernel/include/mcsos/kernel/log.h, fungsi yang tersedia adalah log_writeln.
   Selftest diperbaiki menggunakan log_writeln.

Setelah kedua perbaikan dilakukan, kernel berhasil dibangun tanpa error linker dan selftest
berhasil berjalan pada QEMU.
```

### 14.3 Perbandingan dengan Teori

| Konsep teori | Implementasi praktikum | Sesuai/tidak sesuai | Penjelasan |
|---|---|---|---|
| Acquire/release ordering pada spinlock | `__atomic_exchange_n` acquire dan `__atomic_store_n` release | Sesuai | Memory ordering sesuai model acquire/release |
| Busy-wait dengan pause hint | Instruksi `pause` pada inner loop spin | Sesuai | Mengurangi tekanan pipeline sesuai rekomendasi Intel SDM |
| Owner semantics pada mutex | owner_id disimpan dan divalidasi sebelum unlock | Sesuai | Hanya owner yang boleh unlock |
| Lock order monoton naik | Stack held_class[] dengan pengecekan class_id < top | Sesuai | Akuisisi descending rank ditolak |
| Release LIFO pada lockdep | Pengecekan class_id == held_class[depth-1] | Sesuai | Release di luar urutan ditolak |
| Wait queue pada mutex | Belum diimplementasikan | Tidak sesuai | M12 hanya try_lock; blocking wait untuk modul berikutnya |
| irqsave/restore pada spinlock | Belum diimplementasikan | Tidak sesuai | M12 belum menangani interrupt reentry; documented constraint |

### 14.4 Kompleksitas dan Kinerja

| Aspek | Estimasi/hasil | Bukti | Catatan |
|---|---|---|---|
| Kompleksitas spinlock lock/unlock | O(1) amortized | Desain atomik dan QEMU smoke test | Worst case bergantung pada lama menunggu |
| Kompleksitas lockdep before_acquire | O(depth) untuk pengecekan recursive | Desain loop linear | Depth maksimum 16 |
| Kompleksitas lockdep after_release | O(1) karena hanya memeriksa top | Desain LIFO | Release selalu dari paling atas |
| Waktu build M12 | Sangat cepat (beberapa detik) | Log build | Source kecil, hanya 3 file sync |
| Stress test counter | 4 thread * 25000 iterasi = 100000 exact | Host test PASS | Tidak ada counter mismatch |

---

## 15. Debugging dan Failure Modes

### 15.1 Failure Modes yang Ditemukan

| Failure mode | Gejala | Penyebab | Bukti | Perbaikan |
|---|---|---|---|---|
| Undefined symbol `kernel_panic` | Linker error saat `make all` | Nama fungsi tidak sesuai API kernel MCSOS | Error `undefined symbol: kernel_panic` dari ld.lld | Ganti dengan macro `KERNEL_PANIC` dari `mcsos/kernel/panic.h` |
| Undefined symbol `klog_info` | Linker error saat `make all` | Nama fungsi tidak sesuai API kernel MCSOS | Error `undefined symbol: klog_info` dari ld.lld | Ganti dengan `log_writeln` dari `mcsos/kernel/log.h` |
| Warning `unused variable boot_counter` | Build error karena `-Werror` | Variable deklarasi pada versi panic test tidak digunakan | Error `unused variable 'boot_counter'` | Hapus deklarasi variable yang tidak digunakan |

### 15.2 Failure Modes yang Diantisipasi

| Failure mode | Deteksi | Dampak | Mitigasi |
|---|---|---|---|
| Deadlock spinlock: lock diambil dua kali | Boot hang, QEMU tidak mencetak log setelah selftest | Kernel hang selamanya | Gunakan lockdep sebelum spinlock; audit semua return path |
| Mutex tidak dapat dibuka | Lock tetap locked, subsystem berikutnya tidak dapat masuk | Subsystem downstream hang | Pastikan owner ID berasal dari thread/context yang sama |
| Lock-order inversion | Lockdep menolak acquire dengan violation_count naik | Potensi deadlock pada SMP | Tetapkan global lock hierarchy dan dokumentasikan |
| Interrupt reentry pada spinlock | Stack overflow atau data corruption | Kernel panic atau undefined behavior | Tambahkan varian irqsave/restore pada modul lanjutan |
| Unresolved atomic symbol pada freestanding | Linker error `__atomic_*` | Build gagal | Gunakan tipe 32-bit/64-bit natural yang lock-free |
| Starvation pada spinlock | Thread tidak pernah mendapat lock | Livelock atau priority inversion | Pecah critical section; gunakan proper scheduling |

### 15.3 Triage yang Dilakukan

```text
Diagnosis dilakukan secara bertahap. Pertama, error linker dianalisis dari output ld.lld untuk
menemukan simbol yang tidak ditemukan. Kemudian dilakukan pencarian nama fungsi yang tersedia
pada header kernel menggunakan grep terhadap direktori kernel/include.

Setelah menemukan nama yang benar (KERNEL_PANIC dan log_writeln), selftest diperbaiki dan
build dijalankan ulang. Verifikasi dilakukan dengan memeriksa output make all hingga tidak ada
error. QEMU smoke test kemudian dijalankan untuk memastikan selftest berhasil pada runtime.
```

### 15.4 Panic Path

```text
Panic path diuji secara eksplisit dengan memodifikasi m12_selftest.c untuk melakukan
recursive acquire yang disengaja. Skenario pengujian:

1. mcs_lockdep_before_acquire(&boot_lockdep, 10u, "boot_stats") — akuisisi pertama berhasil (MCS_SYNC_OK)
2. mcs_lockdep_before_acquire(&boot_lockdep, 10u, "boot_stats") — akuisisi kedua mengembalikan MCS_SYNC_EDEADLK
3. Jika akuisisi kedua tidak mengembalikan MCS_SYNC_EDEADLK, KERNEL_PANIC dipanggil

Hasil: akuisisi kedua berhasil terdeteksi sebagai EDEADLK dan kernel mencetak
"M12 panic path verified: recursive acquire detected correctly" tanpa panic.
Setelah pengujian, selftest dikembalikan ke versi normal dan kernel berhasil boot dengan
"M12 sync selftest passed".
```

---

## 16. Prosedur Rollback

| Skenario rollback | Perintah | Data yang harus diselamatkan | Status |
|---|---|---|---|
| Kembali ke commit M11 | `git checkout d496941` | Log build, evidence, dan artefak M12 | Teruji |
| Revert commit M12 | `git revert 87c3d7a` | Host test log dan QEMU log | Belum diuji eksplisit |
| Restore file yang dimodifikasi | `git restore kernel/core/kmain.c kernel/sync/` | Backup evidence jika diperlukan | Teruji via git restore |
| Bersihkan artefak build | `make -f Makefile.m12 clean && make clean` | Source code tetap aman | Teruji |

Catatan rollback:

```text
Rollback dapat dilakukan dengan aman menggunakan git checkout atau git revert karena
seluruh perubahan M12 berada pada branch praktikum/m12-sync yang terpisah dari branch
M11. Evidence dan artefak tersimpan di evidence/M12/ dan tidak ikut terhapus oleh
make clean. Risiko rollback utama adalah hilangnya artefak build sementara yang tidak
disimpan di evidence.
```

---

## 17. Keamanan dan Reliability

### 17.1 Risiko Keamanan

| Risiko | Boundary | Dampak | Mitigasi | Evidence |
|---|---|---|---|---|
| Race condition pada critical section | Spinlock kernel | Data corruption pada PMM, VMM, heap, atau runqueue | Menggunakan spinlock atomik dengan acquire/release ordering | Host test spinlock threads PASS: counter exact |
| Deadlock karena lock-order inversion | Lock hierarchy kernel | Kernel hang | Lock-order validator dengan invariant monoton naik | Host test lockdep negative PASS |
| Recursive acquire deadlock | Mutex dan spinlock | Kernel hang | Mutex menolak recursive acquire; lockdep mendeteksi recursive | Host test mutex owner dan lockdep negative PASS |
| Unlock oleh non-owner | Mutex boundary | Privilege boundary bocor, critical section tidak terlindungi | Mutex memvalidasi owner_id sebelum release | Host test mutex owner: non-owner unlock rejected PASS |
| Interrupt reentry pada spinlock | Interrupt handler dan task context | Stack overflow atau data corruption | Documented constraint; irqsave/restore untuk modul lanjutan | Belum ada enforcement; known limitation M12 |
| Null pointer pada lock | API boundary | Kernel panic atau undefined behavior | Pengecekan null pointer di awal setiap fungsi | Code review dan freestanding compile |

### 17.2 Reliability dan Data Integrity

| Risiko reliability | Dampak | Deteksi | Mitigasi |
|---|---|---|---|
| Stale memory order tanpa acquire/release | CPU atau compiler dapat memindahkan akses data keluar critical section | Audit disassembly dan code review | Menggunakan `__ATOMIC_ACQUIRE` dan `__ATOMIC_RELEASE` secara konsisten |
| Unresolved atomic helper | Build kernel gagal atau runtime undefined | `nm -u` audit | Menggunakan tipe 32-bit/64-bit natural yang lock-free |
| violation_count overflow | Pelanggaran tidak tercatat setelah overflow | Monitoring violation_count pada runtime | Gunakan tipe uint32_t yang memiliki range cukup untuk praktikum |
| Boot hang setelah selftest | Kernel tidak melanjutkan eksekusi | QEMU serial log berhenti sebelum marker berikutnya | Selftest harus tidak blocking dan hanya menggunakan lock yang sudah dilepas |

### 17.3 Negative Test

| Negative test | Input buruk | Expected result | Actual result | Status |
|---|---|---|---|---|
| Lock-order descending | Acquire class 20 lalu class 10 | MCS_SYNC_EDEADLK | EDEADLK | PASS |
| Recursive acquire lockdep | Acquire class 20 dua kali | MCS_SYNC_EDEADLK | EDEADLK | PASS |
| Recursive mutex acquire | try_lock dengan owner yang sama | MCS_SYNC_EDEADLK | EDEADLK | PASS |
| Mutex lock oleh thread lain | try_lock dengan owner berbeda saat terkunci | MCS_SYNC_EBUSY | EBUSY | PASS |
| Mutex unlock oleh non-owner | unlock dengan owner ID berbeda | MCS_SYNC_EPERM | EPERM | PASS |
| Panic path recursive acquire | Recursive acquire disengaja di selftest | KERNEL_PANIC tidak dipanggil; pesan verified muncul | panic path verified | PASS |

---

## 18. Pembagian Kerja Kelompok

| Nama | NIM | Peran | Kontribusi teknis | Commit/artefak |
|---|---|---|---|---|
| Neng Nagita Salma | 25832071004 | Anggota (kerja bersama) | Implementasi spinlock, mutex, lockdep, host unit test, Makefile.m12, dan QEMU smoke test | `87c3d7a` |
| Anisa Nur Azfa | 25832072003 | Anggota (kerja bersama) | Audit nm/readelf/objdump, integrasi kmain, debugging linker error, dan penyimpanan evidence | `87c3d7a` |
| Lailatul Zulfa | 25832072001 | Anggota (kerja bersama) | Uji panic path, validasi serial log QEMU, dan penyusunan laporan | `87c3d7a` |

### 18.1 Mekanisme Koordinasi

```text
Praktikum dikerjakan secara bersama oleh seluruh anggota kelompok tanpa pembagian tugas yang kaku.
Setiap anggota terlibat dalam proses implementasi, debugging, pengujian, dan penyusunan laporan
secara kolaboratif.

Permasalahan seperti linker error undefined symbol diselesaikan bersama melalui pemeriksaan
header kernel dan pengujian ulang build hingga berhasil. Validasi QEMU dilakukan bersama
untuk memastikan serial log menunjukkan M12 sync selftest passed.
```

### 18.2 Evaluasi Kontribusi

| Anggota | Persentase kontribusi yang disepakati | Bukti | Catatan |
|---|---:|---|---|
| Neng Nagita Salma | `34%` | `commit 87c3d7a` | Dikerjakan bersama |
| Anisa Nur Azfa | `33%` | `commit 87c3d7a` | Dikerjakan bersama |
| Lailatul Zulfa | `33%` | `commit 87c3d7a` | Dikerjakan bersama |

---

## 19. Kriteria Lulus Praktikum

| Kriteria minimum | Status | Evidence |
|---|---|---|
| Repository dapat dibangun dari clean checkout | PASS | `make -f Makefile.m12 all CC=clang` berhasil |
| `Makefile.m12` tersedia dan target `all` berhasil | PASS | `Makefile.m12` dengan target host-test, freestanding, audit |
| Host unit test M12 lulus | PASS | `[PASS] M12 synchronization host tests passed` |
| Object freestanding x86_64 berhasil dibuat | PASS | `lockdep.o`, `spinlock.o`, `mutex.o` |
| Audit `nm -u`, `readelf`, `objdump`, checksum tersedia | PASS | `evidence/M12/` |
| Integrasi kernel tidak merusak boot path M2–M11 | PASS | Serial log QEMU menunjukkan M4–M11 tetap berjalan |
| Serial log QEMU disimpan | PASS | `evidence/M12/qemu/serial-final.log` |
| Panic path terbaca jika self-test digagalkan | PASS | `evidence/M12/qemu/serial-panic-test.log` |
| Tidak ada warning kritis pada build M12 | PASS | Build log clang/ld.lld |
| Perubahan Git dikomit | PASS | Commit `87c3d7a` pada branch `praktikum/m12-sync` |
| Laporan menjelaskan desain, invariant, failure mode, dan readiness review | PASS | Bagian 9, 14, 15, dan 20 laporan |

---

## 20. Readiness Review

| Status | Definisi | Pilihan |
|---|---|---|
| Belum siap uji | Build/test belum stabil atau bukti belum cukup | `[ ]` |
| Siap uji QEMU | Build bersih, QEMU/test target berjalan, log tersedia | `[ ]` |
| Siap uji QEMU untuk sinkronisasi kernel awal single-core menuju SMP | Host test, freestanding, audit, QEMU, panic path lulus | `[v]` |
| Siap demonstrasi praktikum | Siap ditunjukkan di kelas dengan bukti uji, failure mode, dan rollback | `[ ]` |
| Kandidat siap pakai terbatas | Hanya untuk penggunaan terbatas setelah test, security review, dan dokumentasi | `[ ]` |

Alasan readiness:

```text
Status "siap uji QEMU untuk sinkronisasi kernel awal single-core menuju SMP" dipilih karena
seluruh kriteria minimum M12 terpenuhi: host unit test lulus, freestanding object berhasil
dikompilasi dan diaudit, kernel berhasil boot pada QEMU dengan M12 sync selftest passed,
panic path tervalidasi, dan tidak ada warning kritis pada build.

Hasil M12 belum boleh disebut siap produksi, bebas deadlock, bebas race, atau enterprise-ready
karena belum ada wait queue penuh pada mutex, belum ada irqsave/restore pada spinlock,
belum ada SMP stress test, dan belum ada pembuktian formal race freedom.
```

Known issues:

| No. | Issue | Dampak | Workaround | Target perbaikan |
|---|---|---|---|---|
| 1 | Mutex tidak memiliki wait queue; try_lock langsung return EBUSY | Thread yang gagal lock tidak dapat sleep dan menunggu | Caller melakukan polling try_lock | Tambahkan wait queue dan wakeup pada modul berikutnya |
| 2 | Spinlock tidak memiliki irqsave/restore | Interrupt handler yang mengambil lock yang sama dengan task context dapat deadlock | Hindari sharing lock antara task dan IRQ context | Tambahkan varian irqsave/restore pada modul lanjutan |
| 3 | Lock-order validator hanya LIFO; tidak mendukung trylock-based nesting | Pola nesting non-LIFO ditolak meskipun aman | Gunakan desain lock yang mengikuti urutan LIFO | Perluasan lockdep graph untuk modul lanjutan |
| 4 | Belum ada SMP stress test | Race pada multi-core belum terdeteksi | Hanya digunakan pada single-core QEMU | SMP bring-up pada milestone berikutnya |

Keputusan akhir:

```text
Berdasarkan bukti build bersih, host unit test empat fungsi PASS, freestanding object audit
(nm/readelf/objdump/sha256), QEMU serial log, dan uji panic path, hasil praktikum M12 layak
disebut siap uji QEMU untuk sinkronisasi kernel awal single-core menuju SMP. Sistem belum
layak disebut production-ready karena keterbatasan yang tercatat pada known issues.
```

---

## 21. Rubrik Penilaian 100 Poin

| Komponen | Bobot | Indikator nilai penuh | Nilai |
|---|---:|---|---:|
| Kebenaran fungsional | 30 | Spinlock, mutex, lockdep, host test, dan freestanding compile berjalan sesuai kontrak | `[0-30]` |
| Kualitas desain dan invariants | 20 | Invariant lock, owner, memory ordering, dan lock hierarchy dijelaskan benar | `[0-20]` |
| Pengujian dan bukti | 20 | Log host test, QEMU, `nm`, `readelf`, `objdump`, checksum, dan commit hash lengkap | `[0-20]` |
| Debugging dan failure analysis | 10 | Failure modes, diagnosis, dan solusi kendala realistis | `[0-10]` |
| Keamanan dan robustness | 10 | Risiko race, deadlock, interrupt reentry, privilege effect, dan mitigasi dijelaskan | `[0-10]` |
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
Praktikum M12 berhasil mengimplementasikan fondasi sinkronisasi kernel awal pada MCSOS berbasis
arsitektur x86_64. Spinlock atomik berhasil melindungi counter bersama pada test multithreaded
dengan hasil exact tanpa race condition. Mutex kooperatif berhasil menolak recursive acquire dan
unlock oleh non-owner sesuai owner semantics yang direncanakan. Lock-order validator berhasil
mendeteksi lock-order inversion dan recursive acquire serta mencatat violation_count dengan benar.

Seluruh komponen berhasil dikompilasi sebagai object freestanding x86_64 tanpa unresolved symbol
runtime dan lulus audit nm/readelf/objdump. Kernel selftest berhasil diintegrasikan ke kmain
setelah penyesuaian nama fungsi panic dan log sesuai API kernel MCSOS. QEMU smoke test menunjukkan
M12 sync selftest passed dan panic path tervalidasi dengan benar.
```

### 22.2 Yang Belum Berhasil

```text
Implementasi M12 masih terbatas pada fondasi sinkronisasi awal dan belum mencakup wait queue
penuh pada mutex, irqsave/restore varian spinlock, SMP stress test, lock-free queue,
priority inheritance, RCU, rwlock, seqlock, dan pembuktian formal race freedom.

Mutex pada M12 hanya menyediakan try_lock yang langsung return EBUSY jika lock tidak tersedia.
Thread yang gagal mendapat lock harus melakukan polling atau busy-wait sendiri karena belum
ada mekanisme sleep dan wakeup yang terhubung dengan scheduler.
```

### 22.3 Pekerjaan Berikutnya

```text
Pengembangan berikutnya difokuskan pada:
- Menghubungkan mutex dengan wait queue dan scheduler untuk mendukung sleep dan wakeup
- Menambahkan varian irqsave/restore pada spinlock untuk keamanan interrupt context
- Memperluas lockdep dengan graph dependency untuk mendeteksi circular dependency
- SMP bring-up dan stress test multi-core pada milestone berikutnya
- Mengintegrasikan lock hierarchy ke semua subsystem kernel: PMM, VMM, heap, runqueue, syscall, loader
```

---

## 23. Lampiran

### Lampiran A — Commit Log

```text
87c3d7a (HEAD -> praktikum/m12-sync) M12: add kmain integration and QEMU smoke test evidence
2946884 M12: implement spinlock, cooperative mutex, lockdep validator, host unit test, freestanding audit, and kernel selftest
d496941 (praktikum-m11-elf-user-loader) M11: implement ELF64 user loader, process image plan, host unit test, kernel integration, and QEMU smoke test
f3a325f (praktikum/m10-syscall-abi) M10: implement syscall ABI, IDT vector 0x80, and smoke test
a585135 (m9-kernel-thread-scheduler) M9: implement cooperative kernel thread scheduler
```

### Lampiran B — Diff Ringkas

```diff
+ include/mcs_sync.h
+ kernel/sync/lockdep.c
+ kernel/sync/spinlock.c
+ kernel/sync/mutex.c
+ kernel/sync/m12_selftest.c
+ tests/m12_sync_host_test.c
+ Makefile.m12
+ evidence/M12/

M kernel/core/kmain.c (tambah deklarasi dan pemanggilan m12_sync_selftest)

+ Implementasi spinlock atomik acquire/release
+ Implementasi mutex kooperatif owner-aware
+ Implementasi lock-order validator LIFO monoton naik
+ Host unit test: lockdep order, lockdep negative, spinlock threads, mutex owner
+ Freestanding compile dan audit nm/readelf/objdump/sha256
+ Kernel selftest terintegrasi ke kmain
```

### Lampiran C — Log Build Lengkap

```text
Log build lengkap tersedia pada:
- evidence/M12/m12-build.log
- evidence/M12/host-test.log
- evidence/M12/qemu/kernel-build.log
```

### Lampiran D — Log QEMU Lengkap

```text
Log QEMU normal tersedia pada: evidence/M12/qemu/serial-final.log

Potongan log penting:
limine: Loading executable `boot():/boot/kernel.elf`...
MCSOS 260502 M4 kernel entered
[M5] PIC/PIT initialized; interrupts enabled
[M4] selftest: IDT invariants passed
[M4] IDT and exception dispatch path installed
[M4] ready for QEMU smoke test and GDB audit
[M11] user image plan ready
M12 sync selftest passed

Log uji panic path tersedia pada: evidence/M12/qemu/serial-panic-test.log

Potongan log penting:
M12 panic path verified: recursive acquire detected correctly
M12 sync selftest passed
```

### Lampiran E — Output nm/readelf/objdump

```text
=== nm -u (tidak ada unresolved symbol) ===
build/m12/lockdep.o:
build/m12/spinlock.o:
build/m12/mutex.o:

=== readelf -h lockdep.o (ringkasan) ===
ELF Header:
  Class:   ELF64
  Data:    2's complement, little endian
  Type:    REL (Relocatable file)
  Machine: Advanced Micro Devices X86-64

=== objdump -d spinlock.o (ringkasan) ===
0000000000000020 <mcs_spin_try_lock>:
  2e: 87 07   xchg %eax,(%rdi)
0000000000000040 <mcs_spin_lock>:
  70: f3 90   pause
0000000000000080 <mcs_spin_unlock>:
  89: c7 07 00 00 00 00   movl $0x0,(%rdi)
```

### Lampiran F — Screenshot

| No. | File | Keterangan |
|---|---|---|
| 1 | `Sreenshots-M12-host-test.log` | Menunjukkan `[PASS] M12 synchronization host tests passed` |
| 2 | `Screenshots-M12-qemuserial-final.log` | Menunjukkan `M12 sync selftest passed` pada serial output QEMU |

### Lampiran G — Bukti Tambahan

```text
Preflight log: evidence/M12/preflight.log
Build log M12: evidence/M12/m12-build.log
nm audit: evidence/M12/nm-undefined.txt
readelf audit: evidence/M12/readelf-lockdep.txt
objdump audit: evidence/M12/objdump-spinlock.txt
sha256 checksum: evidence/M12/sha256sums.txt
Kernel build log: evidence/M12/qemu/kernel-build.log
ISO make log: evidence/M12/qemu/make-iso.log
QEMU serial normal: evidence/M12/qemu/serial-final.log
QEMU serial panic test: evidence/M12/qemu/serial-panic-test.log
```

---

## 24. Daftar Referensi

Gunakan format IEEE. Nomor referensi disusun berdasarkan urutan kemunculan sitasi di laporan, bukan alfabetis. Contoh format:

```text
[1] Intel Corporation, "Intel® 64 and IA-32 Architectures Software Developer Manuals," Intel Developer Zone, 2026. [Online]. Available: https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html. Accessed: May 3, 2026.

[2] The Linux Kernel Documentation, "Lock types and their rules," kernel.org, 2026. [Online]. Available: https://www.kernel.org/doc/html/latest/locking/locktypes.html. Accessed: May 3, 2026.

[3] The Linux Kernel Documentation, "Runtime locking correctness validator," kernel.org, 2026. [Online]. Available: https://www.kernel.org/doc/html/latest/locking/lockdep-design.html. Accessed: May 3, 2026.

[4] The Linux Kernel Documentation, "Generic Mutex Subsystem," kernel.org, 2026. [Online]. Available: https://docs.kernel.org/locking/mutex-design.html. Accessed: May 3, 2026.

[5] Free Software Foundation, "Built-in Functions for Memory Model Aware Atomic Operations," GCC Online Documentation, 2026. [Online]. Available: https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html. Accessed: May 3, 2026.

[6] LLVM Project, "Clang command line argument reference," Clang Documentation, 2026. [Online]. Available: https://clang.llvm.org/docs/ClangCommandLineReference.html. Accessed: May 3, 2026.

[7] QEMU Project, "GDB usage," QEMU Documentation, 2026. [Online]. Available: https://www.qemu.org/docs/master/system/gdb.html. Accessed: May 3, 2026.

[8] GNU Binutils, "GNU Binary Utilities," Sourceware, 2025. [Online]. Available: https://www.sourceware.org/binutils/docs/binutils.html. Accessed: May 3, 2026.
```

---

## 25. Checklist Final Sebelum Pengumpulan

| Checklist | Status |
|---|---|
| Semua placeholder `[isi ...]` sudah diganti | `Ya` |
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

Kami mengumpulkan laporan ini bersama artefak pendukung pada commit:

```text
87c3d7a
```

Status akhir yang diklaim:

```text
siap uji QEMU untuk sinkronisasi kernel awal single-core menuju SMP
```

Ringkasan satu paragraf:

```text
Praktikum M12 berhasil mengimplementasikan fondasi sinkronisasi kernel awal pada MCSOS berbasis
arsitektur x86_64 yang mencakup spinlock atomik dengan acquire/release ordering, mutex kooperatif
dengan owner semantics, dan lock-order validator dengan invariant monoton naik dan release LIFO.
Seluruh komponen berhasil dikompilasi sebagai object freestanding x86_64 tanpa unresolved symbol
runtime, lulus host unit test multithreaded dengan empat fungsi test, dan diaudit menggunakan
nm/readelf/objdump. Kernel selftest berhasil diintegrasikan ke kmain dan QEMU smoke test
menunjukkan M12 sync selftest passed pada serial log. Panic path juga tervalidasi melalui
skenario recursive acquire yang disengaja. Meskipun demikian, implementasi masih terbatas pada
fondasi sinkronisasi awal single-core dan belum mencakup wait queue penuh pada mutex,
irqsave/restore, dan SMP stress test. Pengembangan berikutnya difokuskan pada integrasi
mutex dengan scheduler, lock hierarchy seluruh subsystem, dan SMP bring-up.
```
