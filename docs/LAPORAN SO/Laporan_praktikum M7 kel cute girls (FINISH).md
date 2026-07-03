# Laporan Praktikum Sistem Operasi Lanjut — MCSOS

**Nama file laporan:** `laporan_praktikum_M7_cute girls.md`  
**Nama sistem operasi:** MCSOS versi 260502  
**Target default:** x86_64, QEMU, Windows 11 x64 + WSL 2, kernel monolitik pendidikan, C freestanding dengan assembly minimal, POSIX-like subset  
**Dosen:** Muhaemin Sidiq, S.Pd., M.Pd.  
**Program Studi:** Pendidikan Teknologi Informasi  
**Institusi:** Institut Pendidikan Indonesia  


---

## 0. Metadata Laporan

| Atribut | Isi |
|---|---|
| Kode praktikum | `M7` |
| Judul praktikum | `Virtual Memory Manager Awal, Page Table x86_64, dan Page Fault Diagnostics pada MCSOS` |
| Jenis pengerjaan | `Kelompok` |
| Nama kelompok | `cute girls` |
| Anggota kelompok | `Neng Nagita Salma (25832071004) — Anisa Nur Azfa (25832072003) — Lailatul Zulfa (25832072001)` |
| Kelas | `1A` |
| Tanggal praktikum | `2026-05-11` |
| Tanggal pengumpulan | `2026-07-01` |
| Repository | `https://github.com/nagitasalma47-source/mcsos-M0-cute-girls` |
| Branch | `m0/cute-girls` |
| Commit awal | `` dc1ccc7 `` |
| Commit akhir | ``  202c4dc `` |
| Status readiness yang diklaim | `siap demonstrasi praktikum ` |

---

## 1. Sampul

# Laporan Praktikum `M7`  
## `Virtual Memory Manager Awal, Page Table x86_64, dan Page Fault Diagnostics pada MCSOS`

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
Menggunakan ChatGPT sebagai asisten untuk membantu penjelasan konsep Virtual Memory Manager (VMM), debugging error kompilasi kernel, perbaikan Makefile, integrasi PMM–VMM, serta penyusunan skrip grading dan GDB workflow.

Bantuan mencakup interpretasi error build (clang/ld.lld), struktur implementasi paging x86_64, dan penyusunan checklist pengujian (host test, QEMU, dan GDB).

Seluruh kode yang dihasilkan telah diverifikasi secara mandiri melalui:
- host unit test (make check-m7)
- static analysis (nm, objdump)
- QEMU smoke test
- GDB debugging session

Tidak ada kode eksternal dari library pihak ketiga yang digunakan selain standar kernel freestanding.
```

---

## 3. Tujuan Praktikum

Tuliskan tujuan teknis dan konseptual praktikum. Tujuan harus dapat diuji.

1. `Membangun kernel freestanding berbasis x86_64 menggunakan toolchain clang/ld.lld yang reproducible tanpa dependensi libc host.
2. `Menghasilkan image kernel yang dapat dijalankan pada QEMU dengan output serial log untuk observasi boot dan runtime sistem.
3. `Menjelaskan dan mengimplementasikan konsep virtual memory pada arsitektur x86_64, termasuk struktur paging (PML4, PDPT, PD, PT), CR3, serta mekanisme direct map/HHDM.
4. `Memahami kontrak manajemen memori kernel, termasuk perbedaan physical frame allocation (PMM) dan virtual address mapping (VMM), serta invariant allocator pada sistem paging.
5. `Mengimplementasikan Virtual Memory Manager (VMM) yang mendukung operasi map, query, dan unmap halaman 4 KiB secara deterministik dengan validasi canonical address dan alignment.
6. `Mengimplementasikan mekanisme invalidasi Translation Lookaside Buffer (TLB) menggunakan instruksi invlpg setelah operasi unmap untuk menjaga konsistensi translasi alamat.
7. `Melakukan validasi implementasi melalui host unit test, static object audit (nm, objdump), serta integrasi kernel pada QEMU.
8. `Menyimpan dan menganalisis seluruh bukti pengujian berupa log build, log QEMU, hasil readelf/objdump, serta hasil test sebagai verifikasi sistem.
`
---

## 4. Capaian Pembelajaran Praktikum

Setelah praktikum ini, mahasiswa mampu:

| CPL/CPMK praktikum | Bukti yang harus ditunjukkan |
|---|---|
| Menjelaskan translasi virtual address x86_64 melalui struktur paging PML4, PDPT, PD, dan PT    | Log penjelasan/diagram paging, dokumentasi laporan, serta hasil analisis struktur page table            |
| Menjelaskan peran CR3 sebagai register basis fisik page table hierarchy                        | Source code VMM (vmm_read_cr3), hasil objdump yang menunjukkan penggunaan CR3, serta penjelasan laporan |
| Menjelaskan kebutuhan direct map / HHDM untuk akses memori fisik pada kernel                   | Implementasi kernel_phys_to_virt, log integrasi PMM–VMM, serta diagram mapping memori                   |
| Membedakan physical frame allocation dan virtual address mapping                               | Source PMM vs VMM (pmm_alloc_frame, vmm_map_page), serta hasil analisis kode dan laporan                |
| Mengimplementasikan validasi alamat canonical 48-bit                                           | Unit test host (M7 VMM host tests PASS), fungsi vmm_is_canonical, dan log pengujian                     |
| Mengimplementasikan operasi map, query, dan unmap halaman 4 KiB secara deterministik           | Hasil host unit test, file test_vmm_host.c, dan output “M7 VMM host tests PASS”                         |
| Menghindari remap diam-diam terhadap virtual address yang sudah present                        | Implementasi VMM_ERR_EXISTS, hasil test duplicate mapping pada build/evidence log                       |
| Menggunakan instruksi invlpg setelah unmap untuk menjaga konsistensi TLB                       | Hasil objdump (grep invlpg build/vmm.o.objdump.txt) dan audit disassembly                               |
| Menjelaskan struktur error code page fault (present, write, user, reserved, instruction fetch) | Implementasi page_fault_dump(), log QEMU (#PF page fault), dan hasil GDB register dump CR2              |
| Menghasilkan bukti pengujian lengkap sistem VMM                                                | `make check-m7` output, nm -u (kosong), objdump audit, log QEMU, serta script grade_m7.sh               |


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
Praktikum M7 berfokus pada implementasi Virtual Memory Manager (VMM) berbasis paging 4-level pada arsitektur x86_64 di kernel MCSOS. Cakupan praktikum meliputi implementasi struktur page table (PML4, PDPT, PD, PT), operasi dasar virtual memory (map, query, dan unmap halaman 4 KiB), validasi alamat canonical dan alignment, integrasi dengan Physical Memory Manager (PMM) sebagai backend alokasi frame, serta mekanisme invalidasi TLB menggunakan instruksi invlpg.

Selain itu, praktikum mencakup penanganan page fault dasar (#PF) dengan pembacaan CR2 dan decoding error code, serta validasi implementasi melalui host unit test, static verification menggunakan nm, objdump, dan readelf, integrasi kernel pada QEMU, serta debugging menggunakan GDB.

Praktikum ini belum mencakup mekanisme demand paging dan recovery page fault, memory swapping ke disk, multi-process virtual memory management, user space isolation penuh, maupun fitur proteksi memori tingkat lanjut seperti W^X enforcement, KASLR, NUMA awareness, dan optimisasi huge page production-level. Dengan demikian, VMM pada M7 masih dikategorikan sebagai virtual memory manager dasar untuk kebutuhan edukasi kernel dan bootstrap sistem operasi.
```

---

## 6. Dasar Teori Ringkas

Praktikum M7 membahas implementasi Virtual Memory Manager (VMM) pada arsitektur x86_64 menggunakan mekanisme paging empat level, yaitu PML4, PDPT, PD, dan PT. Translasi alamat virtual ke alamat fisik dilakukan melalui struktur page table tersebut dengan bantuan register CR3 sebagai root page table aktif.

VMM bekerja bersama Physical Memory Manager (PMM) untuk mengelola frame fisik yang digunakan sebagai page table. Operasi utama yang diimplementasikan meliputi mapping, query, dan unmapping halaman 4 KiB dengan validasi canonical address dan alignment.

Setelah proses unmap, sistem menggunakan instruksi invlpg untuk melakukan invalidasi TLB agar translasi lama tidak tetap tersimpan pada cache CPU. Praktikum juga menguji penanganan page fault (#PF) dengan membaca CR2 dan error code untuk diagnosis kesalahan memori.

Validasi sistem dilakukan melalui host unit test, static analysis (nm, objdump, readelf), QEMU smoke test, dan debugging menggunakan GDB.

### 6.1 Konsep Sistem Operasi yang Diuji

```text
Praktikum M7 menguji konsep virtual memory pada arsitektur x86_64 melalui implementasi Virtual Memory Manager (VMM) pada kernel MCSOS. Sistem menggunakan mekanisme paging empat level yang terdiri atas PML4, PDPT, PD, dan PT untuk melakukan translasi virtual address menjadi physical address.

Kernel memanfaatkan Physical Memory Manager (PMM) dari praktikum sebelumnya sebagai allocator frame fisik untuk kebutuhan page table. Integrasi PMM dan VMM menjadi bagian penting dalam pengelolaan memori kernel.

Selain itu, praktikum juga menguji penggunaan register kontrol CR3 sebagai basis page-table hierarchy, serta pemanfaatan direct mapping/HHDM agar kernel dapat mengakses struktur page table yang berada di memori fisik melalui virtual address.

Pada sisi exception handling, praktikum menguji mekanisme page fault (#PF) melalui trap dispatcher x86_64. Handler page fault membaca register CR2 dan error code CPU untuk melakukan diagnosis fault pada runtime kernel.

Praktikum juga memanfaatkan QEMU sebagai emulator sistem x86_64 dan GDB sebagai debugger untuk observasi runtime, validasi trap handler, serta audit state register kernel selama eksekusi berlangsung.
```

### 6.2 Konsep Arsitektur x86_64 yang Relevan

| Konsep | Relevansi pada praktikum | Bukti/verifikasi |
|---|---|---|
| Paging 4-level (PML4, PDPT, PD, PT) | Digunakan untuk translasi virtual address ke physical address pada implementasi VMM | Host unit test, source `vmm.c`, QEMU serial log |
| CR3 Register | Menyimpan alamat root page table aktif untuk virtual memory system | `objdump` menunjukkan akses `cr3`, implementasi `vmm_read_cr3()` |
| TLB dan `invlpg` | Digunakan untuk invalidasi translasi lama setelah unmap halaman | `objdump -dr build/vmm.o`, grep `invlpg` |
| Canonical Address x86_64 | Digunakan untuk validasi virtual address 48-bit agar sesuai spesifikasi arsitektur | Host unit test canonical address |
| Page Fault (#PF) | Digunakan untuk diagnosis kesalahan akses memori virtual | Serial log page fault, implementasi `page_fault_dump()` |
| IDT dan Trap Dispatcher | Menangani exception dan interrupt kernel termasuk page fault | QEMU serial log, `x86_64_trap_dispatch()` |
| QEMU dan GDB Debugging | Digunakan untuk observasi runtime kernel dan validasi trap handler | Breakpoint GDB, register dump RIP/RSP/CR3 |

### 6.3 Konsep Implementasi Freestanding

| Aspek | Keputusan praktikum |
|---|---|
| Bahasa | C17 freestanding dan assembly x86_64 |
| Runtime | Tanpa hosted libc, menggunakan runtime kernel minimal |
| ABI | x86_64 System V ABI untuk kernel |
| Compiler flags kritis | `-ffreestanding`, `-fno-builtin`, `-fno-stack-protector`, `-mno-red-zone`, `-nostdlib` |
| Risiko undefined behavior | Pointer invalid, alignment page 4 KiB, canonical address invalid, dan akses page table tidak valid |

### 6.4 Referensi Teori yang Digunakan

| No. | Sumber | Bagian yang digunakan | Alasan relevansi |
|---|---|---|---|
| 1 | Intel 64 and IA-32 Architectures Software Developer’s Manual | Paging, CR3, page fault, dan x86_64 memory management | Digunakan sebagai referensi utama implementasi paging dan exception handling |
| 2 | OSDev Wiki | Paging, Higher Half Kernel, dan IDT/Interrupt | Membantu memahami implementasi praktis kernel x86_64 pada lingkungan freestanding |
| 3 | Dokumentasi QEMU dan GDB | QEMU debugging dan remote GDB | Digunakan untuk validasi runtime dan debugging kernel |
| 4 | Dokumentasi Clang/LLVM dan GNU Binutils | `objdump`, `readelf`, `nm`, dan freestanding compilation | Digunakan untuk static verification dan audit object kernel |

---

## 7. Lingkungan Praktikum

### 7.1 Host dan Target

| Komponen | Nilai |
|---|---|
| Host OS | Windows 11 x64 26200.8246 |
| Lingkungan build | WSL 2 Ubuntu 24.04 |
| Target ISA | x86_64 |
| Target ABI | x86_64-unknown-none-elf |
| Emulator | QEMU x86_64 |
| Firmware emulator | Limine bootloader + BIOS/UEFI image |
| Debugger | GNU GDB 15.1 |
| Build system | GNU Make |
| Bahasa utama | C17 freestanding |
| Assembly | GNU Assembler (GAS) |

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
date_utc=2026-05-14T16:28:28Z
Linux ASUS 6.6.87.2-microsoft-standard-WSL2 #1 SMP PREEMPT_DYNAMIC Thu Jun 5 18:30:46 UTC 2025 x86_64 x86_64 x86_64 GNU/Linux
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
| Path repository di WSL | `~/src/mcsos` |
| Apakah berada di filesystem Linux WSL, bukan `/mnt/c` | Ya |
| Remote repository | https://github.com/nagitasalma47-source/mcsos-M0-cute-girls|
| Branch | `m6-pmm` |
| Commit hash awal | `dc1ccc7` |
| Commit hash akhir | `202c4dc` |

---

## 8. Repository dan Struktur File

### 8.1 Struktur Direktori yang Relevan

```text
mcsos/
├── include/
│   └── vmm.h
├── kernel/
│   ├── arch/x86_64/
│   │   ├── idt.c
│   │   └── isr.S
│   ├── core/
│   │   ├── kmain.c
│   │   ├── pmm.c
│   │   ├── trap.c
│   │   └── panic.c
│   └── lib/
│       └── memory.c
├── src/
│   └── vmm.c
├── tests/
│   └── test_vmm_host.c
├── scripts/
│   ├── grade_m7.sh
│   ├── m7_preflight.sh
│   └── m7_gdb.cmd
├── build/
├── iso_root/
├── linker.ld
└── Makefile
```

### 8.2 File yang Dibuat atau Diubah

| File | Jenis perubahan | Alasan perubahan | Risiko |
|---|---|---|---|
| `include/vmm.h` | Baru | Menyediakan deklarasi API dan struktur VMM | Rendah, hanya header interface |
| `src/vmm.c` | Baru | Implementasi Virtual Memory Manager | Tinggi, mempengaruhi paging dan mapping kernel |
| `tests/test_vmm_host.c` | Baru | Host unit test untuk validasi VMM | Rendah, hanya digunakan saat testing |
| `kernel/core/kmain.c` | Ubah | Integrasi PMM dan inisialisasi VMM | Sedang, mempengaruhi proses boot kernel |
| `kernel/core/trap.c` | Ubah | Menambahkan handler page fault (#PF) dan CR2 dump | Sedang, berkaitan dengan exception handling |
| `Makefile` | Ubah | Menambahkan build source VMM dan script M7 | Sedang, mempengaruhi pipeline build |
| `scripts/grade_m7.sh` | Baru | Automasi host test dan static verification | Rendah, hanya script validasi |
| `scripts/m7_preflight.sh` | Baru | Pemeriksaan awal environment praktikum | Rendah, hanya script helper |
| `scripts/m7_gdb.cmd` | Baru | Automasi workflow debugging GDB | Rendah, hanya konfigurasi debugging |

### 8.3 Ringkasan Diff

```bash
git status --short
git diff --stat
git log --oneline -n 5
```

Output:

```text
202c4dc (HEAD -> m6-pmm, tag: m7-final) M7 final: VMM complete + QEMU + GDB validated
dc1ccc7 M6: physical memory manager integrated and tested
0040213 (origin/praktikum/m5-timer-irq, origin/m5/timer-irq, origin/m0/cute-girls, praktikum/m5-timer-irq, m5/timer-irq, m0/cute-girls) M5: implement PIC, PIT, and timer IRQ handling
ade8ba9 (origin/m4-idt-exception-path, m4-idt-exception-path) M4 add x86_64 IDT and exception trap path
c7784b7 (origin/praktikum/m3-panic-debug-audit, praktikum/m3-panic-debug-audit) M3 panic path logging gdb and disassembly audit
```

---

## 9. Desain Teknis

### 9.1 Masalah yang Diselesaikan

Praktikum M7 menyelesaikan masalah belum tersedianya mekanisme virtual memory management pada kernel MCSOS. Sebelum M7, kernel hanya memiliki Physical Memory Manager (PMM) untuk alokasi frame fisik, tetapi belum mampu melakukan translasi dan pengelolaan virtual address menggunakan page table x86_64.

Kernel juga belum memiliki operasi dasar virtual memory seperti mapping, query, dan unmapping halaman 4 KiB secara deterministik. Selain itu, sistem belum memiliki validasi canonical address, invalidasi TLB setelah unmap, maupun diagnosis page fault (#PF) menggunakan register CR2 dan error code CPU.

Melalui implementasi VMM, kernel menjadi mampu mengelola page table sendiri, melakukan translasi virtual address, serta menyediakan dasar memory management yang dapat digunakan untuk pengembangan fitur kernel lanjutan.

### 9.2 Keputusan Desain

| Keputusan | Alternatif yang dipertimbangkan | Alasan memilih | Konsekuensi |
|---|---|---|---|
| Menggunakan paging 4-level x86_64 standar | Menggunakan huge page atau paging custom | Sesuai spesifikasi arsitektur x86_64 dan lebih sederhana untuk validasi M7 | Struktur page table lebih dalam dan membutuhkan lebih banyak frame |
| Menggunakan PMM bitmap sebagai allocator page table | Menggunakan allocator linked-list atau buddy allocator | PMM bitmap dari M6 sudah stabil dan mudah diintegrasikan | Belum optimal untuk fragmentasi dan scalability |
| Menolak overwrite mapping yang sudah present | Mengizinkan remap otomatis | Menghindari corruption pada page table dan menjaga determinisme mapping | Caller harus melakukan unmap terlebih dahulu |
| Menggunakan validasi canonical address 48-bit | Tidak melakukan validasi alamat | Mencegah akses virtual address ilegal pada x86_64 | Menambah pengecekan pada operasi mapping |
| Menggunakan `invlpg` setelah unmap | Mengandalkan flush TLB otomatis | Menjamin translasi lama tidak tersimpan pada TLB CPU | Sedikit menambah overhead instruksi |
| Menambahkan page fault diagnostic sederhana | Langsung panic tanpa dump informasi | Mempermudah debugging runtime kernel | Belum mendukung recovery page fault |
| Menggunakan QEMU dan GDB untuk validasi runtime | Hanya host unit test | Memberikan observasi langsung terhadap perilaku kernel | Membutuhkan setup debugging tambahan |

### 9.3 Arsitektur Ringkas

```mermaid
flowchart TD
    A[Boot Kernel x86_64] --> B[Physical Memory Manager PMM]
    B --> C[Virtual Memory Manager VMM]
    C --> D[Page Table PML4 PDPT PD PT]
    D --> E[Map Query Unmap Page]
    E --> F[TLB Invalidation invlpg]
    E --> G[Page Fault Handler]
    G --> H[CR2 dan Error Code Dump]
    H --> I[QEMU Serial Log dan GDB Debugging]
```

Penjelasan diagram:

```text
Kernel melakukan boot pada lingkungan QEMU dan menginisialisasi Physical Memory Manager (PMM) sebagai allocator frame fisik. PMM kemudian digunakan oleh Virtual Memory Manager (VMM) untuk membangun dan mengelola struktur page table x86_64 yang terdiri atas PML4, PDPT, PD, dan PT.

VMM menyediakan operasi map, query, dan unmap halaman virtual 4 KiB. Setelah operasi unmap dilakukan, sistem menggunakan instruksi invlpg untuk melakukan invalidasi TLB agar translasi lama tidak tetap tersimpan pada cache CPU.

Ketika terjadi page fault (#PF), trap dispatcher memanggil page fault handler untuk membaca CR2 dan error code CPU sebagai informasi diagnosis runtime kernel. Seluruh aktivitas sistem diverifikasi melalui serial log QEMU, host unit test, dan debugging menggunakan GDB.
```

### 9.4 Kontrak Antarmuka

| Antarmuka | Pemanggil | Penerima | Precondition | Postcondition | Error path |
|---|---|---|---|---|---|
| `vmm_map_page()` | Kernel / VMM caller | VMM subsystem | Virtual address canonical dan aligned 4 KiB | Mapping virtual ke physical address berhasil dibuat | Return error jika address invalid atau mapping sudah ada |
| `vmm_unmap_page()` | Kernel / VMM caller | VMM subsystem | Halaman sudah dalam keadaan mapped | Mapping dihapus dan TLB di-invalidasi | Return error jika halaman tidak mapped |
| `vmm_query_page()` | Kernel / debugging code | VMM subsystem | Virtual address valid | Informasi mapping halaman dikembalikan | Return error jika halaman tidak ditemukan |
| `pmm_alloc_frame()` | VMM subsystem | PMM subsystem | Masih tersedia free frame fisik | Frame fisik baru dialokasikan | Return `PMM_INVALID_FRAME` jika kehabisan frame |
| `pmm_free_frame()` | VMM subsystem | PMM subsystem | Frame sebelumnya valid dan allocated | Frame dikembalikan ke PMM | Return failure jika frame invalid |
| `page_fault_dump()` | Trap dispatcher | Page fault diagnostic handler | Exception vector = 14 (#PF) | Informasi CR2 dan error code dicetak ke serial log | Kernel panic jika fault tidak dapat dipulihkan |
| `x86_64_trap_dispatch()` | CPU interrupt/exception | Trap subsystem | IDT dan ISR sudah terpasang | Trap diproses sesuai vector exception | Kernel panic untuk exception fatal |

### 9.5 Struktur Data Utama

| Struktur data | Field penting | Ownership | Lifetime | Invariant |
|---|---|---|---|---|
| `struct vmm_space` | `root_table`, allocator callback, context pointer | Kernel VMM subsystem | Dibuat saat inisialisasi VMM dan aktif selama kernel berjalan | Root page table harus valid dan aligned 4 KiB |
| `struct pmm_state` | Bitmap frame, total frame, free frame count | Kernel PMM subsystem | Dibuat saat PMM init dan aktif selama runtime kernel | Bitmap harus konsisten dengan status frame fisik |
| `x86_64_trap_frame_t` | `vector`, `error_code`, `rip`, register CPU | Trap dispatcher | Dibuat saat interrupt/exception terjadi | Isi register harus mencerminkan state CPU saat trap |
| Page Table Entry (PTE) | Physical address dan flags paging | VMM subsystem | Dibuat saat map dan dihapus saat unmap | Address harus aligned dan flag reserved tidak boleh aktif |

### 9.6 Invariants

1. Setiap virtual address yang di-map harus bersifat canonical dan aligned 4 KiB sesuai spesifikasi paging x86_64.  
2. Mapping virtual address yang sudah present tidak boleh dioverwrite secara diam-diam dan harus menghasilkan error.  
3. Setiap operasi unmap wajib melakukan invalidasi TLB menggunakan instruksi `invlpg` agar translasi lama tidak tetap tersimpan.  
4. Root page table dan seluruh page table entry harus menggunakan physical address yang valid dan aligned 4 KiB.  
5. PMM dan VMM harus menjaga konsistensi ownership frame fisik agar tidak terjadi double allocation atau double free.  
6. Handler page fault (#PF) harus dapat membaca CR2 dan error code sebelum kernel melakukan panic atau halt.  

### 9.7 Ownership, Locking, dan Concurrency

| Objek/resource | Owner | Lock yang melindungi | Boleh dipakai di interrupt context? | Catatan |
|---|---|---|---|---|
| PMM bitmap frame allocator | PMM subsystem | None | Tidak | Praktikum masih berjalan pada single-core kernel sederhana |
| Struktur page table VMM | VMM subsystem | None | Tidak | Modifikasi page table dilakukan saat kernel init |
| CR3 dan TLB state | CPU/kernel | None | Tidak | Perubahan translasi dilakukan secara sinkron |
| Trap dispatcher dan page fault handler | Trap subsystem | None | Ya | Handler harus non-blocking dan hanya melakukan logging/panic |
| Serial logging | Kernel logging subsystem | None | Ya | Digunakan untuk debugging runtime sederhana |

Lock order yang berlaku:

```text
Pada tahap M7 belum digunakan mekanisme locking eksplisit karena kernel masih berjalan pada lingkungan single-core sederhana dengan concurrency terbatas. Operasi PMM dan VMM dilakukan secara sinkron selama inisialisasi kernel sehingga interrupt-disabled dan eksekusi single-core dianggap cukup untuk menjaga konsistensi state.
```

### 9.8 Memory Safety dan Undefined Behavior Risk

| Risiko | Lokasi | Mitigasi | Bukti |
|---|---|---|---|
| Alignment page tidak valid | `src/vmm.c` (`vmm_map_page`) | Validasi alignment 4 KiB sebelum mapping | Host unit test alignment check |
| Virtual address non-canonical | `src/vmm.c` (`vmm_is_canonical`) | Validasi canonical address 48-bit | Host unit test canonical address |
| Overwrite mapping yang sudah present | `src/vmm.c` (`vmm_map_page`) | Return `VMM_ERR_EXISTS` jika page sudah mapped | Duplicate map test PASS |
| TLB stale translation setelah unmap | `src/vmm.c` (`vmm_unmap_page`) | Menggunakan instruksi `invlpg` setelah unmap | `objdump` menunjukkan instruksi `invlpg` |
| Invalid page table entry | `src/vmm.c` | Masking address dan flags sesuai format PTE x86_64 | Code review dan QEMU smoke test |
| Invalid frame allocation | `kernel/core/pmm.c` | Validasi status bitmap frame sebelum alloc/free | PMM selftest PASS |
| Fault diagnosis tidak lengkap | `kernel/core/trap.c` | Membaca CR2 dan error code page fault | QEMU serial log dan GDB debugging |

### 9.9 Security Boundary

| Boundary | Data tidak tepercaya | Validasi yang dilakukan | Failure mode aman |
|---|---|---|---|
| Boot memory map dari bootloader | Region memory dan tipe region | Validasi usable/reserved region dan alignment frame | Kernel panic jika PMM init gagal |
| Virtual address mapping | Virtual address dari caller kernel | Validasi canonical address dan alignment 4 KiB | Return error (`VMM_ERR_INVALID`) |
| Page table entry (PTE) | Physical address dan flags paging | Masking address dan pengecekan reserved bit | Mapping ditolak atau kernel panic |
| Page fault handler (#PF) | Error code dan fault address CPU | Membaca CR2 dan decode error code | Serial log diagnostic dan kernel panic |
| PMM frame allocation | Request alokasi frame fisik | Validasi bitmap ownership frame | Return `PMM_INVALID_FRAME` |

---

## 10. Langkah Kerja Implementasi

### Langkah 1 — Implementasi Header dan Core VMM

Maksud langkah:

```text
Membuat antarmuka Virtual Memory Manager (VMM) serta implementasi dasar operasi map, query, dan unmap halaman 4 KiB pada arsitektur x86_64.
```

Perintah:

```bash
make all
```

Output ringkas:

```text
clang ... -c src/vmm.c -o build/normal/src/vmm.o
ld.lld ... -o build/mcsos-m5.elf
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `vmm.h` | `include/vmm.h` | Deklarasi API dan struktur VMM |
| `vmm.c` | `src/vmm.c` | Implementasi Virtual Memory Manager |
| `vmm.o` | `build/normal/src/vmm.o` | Object file hasil kompilasi VMM |

Indikator berhasil:

```text
Kernel berhasil dikompilasi tanpa error dan object VMM berhasil dibuat.
```

---

### Langkah 2 — Integrasi PMM dan VMM ke Kernel

Maksud langkah:

```text
Mengintegrasikan Virtual Memory Manager dengan Physical Memory Manager (PMM) untuk alokasi page table dan inisialisasi virtual memory kernel.
```

Perintah:

```bash
make all
```

Output ringkas:

```text
[M6] PMM initialized
[M7] VMM core initialized
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `kmain.c` | `kernel/core/kmain.c` | Integrasi PMM dan inisialisasi VMM |
| `mcsos-m5.elf` | `build/mcsos-m5.elf` | Kernel ELF hasil build |

Indikator berhasil:

```text
Kernel berhasil boot dan VMM berhasil diinisialisasi pada runtime.
```

---

### Langkah 3 — Implementasi Page Fault Handler

Maksud langkah:

```text
Menambahkan handler page fault (#PF) untuk membaca CR2 dan error code sebagai mekanisme diagnosis fault pada kernel.
```

Perintah:

```bash
make all
```

Output ringkas:

```text
#PF page fault
cr2=...
error=...
rip=...
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `trap.c` | `kernel/core/trap.c` | Trap dispatcher dan page fault diagnostic |

Indikator berhasil:

```text
Kernel mampu mencetak informasi page fault saat exception terjadi.
```

---

### Langkah 4 — Host Unit Test dan Static Verification

Maksud langkah:

```text
Memvalidasi implementasi VMM melalui host unit test dan audit object freestanding.
```

Perintah:

```bash
./scripts/grade_m7.sh
```

Output ringkas:

```text
M7 VMM host tests PASS
[PASS] static grade M7 selesai
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `test_vmm_host` | `build/test_vmm_host` | Host unit test executable |
| `vmm.objdump.txt` | `build/vmm.objdump.txt` | Audit disassembly VMM |
| `vmm.undefined.txt` | `build/vmm.undefined.txt` | Audit undefined symbol |

Indikator berhasil:

```text
Host unit test PASS dan tidak ditemukan undefined symbol pada object VMM.
```

---

### Langkah 5 — QEMU Smoke Test dan GDB Validation

Maksud langkah:

```text
Melakukan validasi runtime kernel menggunakan QEMU dan debugging menggunakan GDB.
```

Perintah:

```bash
qemu-system-x86_64 -cdrom build/mcsos.iso -serial stdio -no-reboot -no-shutdown
```

```bash
gdb -x scripts/m7_gdb.cmd
```

Output ringkas:

```text
[M7] VMM core initialized
Breakpoint 1, kmain ()
Breakpoint 3, x86_64_trap_dispatch ()
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| `mcsos.iso` | `build/mcsos.iso` | Image bootable QEMU |
| `qemu-m7.log` | `build/qemu-m7.log` | Runtime log QEMU |
| `m7_gdb.cmd` | `scripts/m7_gdb.cmd` | Workflow debugging GDB |

Indikator berhasil:

```text
Kernel berhasil boot pada QEMU dan breakpoint GDB dapat tercapai.
```
## 11. Checkpoint Buildable

| Checkpoint | Perintah | Expected result | Status |
|---|---|---|---|
| Clean build | `make clean && make all` | Kernel ELF dan object file berhasil terbangun | PASS |
| Metadata toolchain | `clang --version`, `gcc --version`, `qemu-system-x86_64 --version` | Informasi toolchain berhasil dicatat | PASS |
| Image generation | `xorriso ... -o build/mcsos.iso` | File `build/mcsos.iso` berhasil dibuat | PASS |
| QEMU smoke test | `qemu-system-x86_64 -cdrom build/mcsos.iso ...` | Serial log `[M7] VMM core initialized` muncul | PASS |
| Test suite | `./scripts/grade_m7.sh` | `M7 VMM host tests PASS` | PASS |

Catatan checkpoint:

```text
Seluruh checkpoint utama berhasil dijalankan. Kernel dapat dibangun dari clean checkout, image ISO berhasil dibuat, host unit test lulus, dan kernel berhasil dijalankan pada QEMU serta divalidasi menggunakan GDB.
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
Kernel berhasil dikompilasi menggunakan clang dan ld.lld.
Object file, ELF kernel, serta artefak build berhasil dibuat tanpa error.
```

Status: `PASS`

### 12.2 Static Inspection

Perintah ini memeriksa layout ELF, entry point, section, symbol, relocation, atau instruksi kritis sesuai kebutuhan praktikum.

```bash
readelf -hW build/kernel.elf
readelf -lW build/kernel.elf
readelf -SW build/kernel.elf
objdump -drwC build/kernel.elf | head -n 120
```

Hasil penting:

```text
ELF64 x86_64 berhasil terbentuk dengan entry point kernel valid.
Program header dan section ELF berhasil dimuat tanpa undefined symbol.
Disassembly menunjukkan penggunaan instruksi invlpg dan akses register CR3 pada implementasi VMM.
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
Hasil:

```text
[M6] PMM initialized
[M6] PMM selftest passed
[M7] VMM core initialized
[M4] ready for QEMU smoke test and GDB audit
```

Status: `PASS`

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
gdb build/mcsos-m5.elf
target remote :1234
break kmain
continue
info registers rip rsp
bt
```

Hasil:

```text
Breakpoint 1, 0xffffffff80000280 in kmain ()

rip            0xffffffff80000280  <kmain>
rsp            0xffff800007f7cfe8

Breakpoint 3, 0xffffffff80001c90 in x86_64_trap_dispatch ()
```

Status: `PASS`

### 12.5 Unit Test

```bash
./scripts/grade_m7.sh
```

Hasil:

```text
M7 VMM host tests PASS
[PASS] static grade M7 selesai
```

Status: `PASS`

### 12.6 Stress/Fuzz/Fault Injection Test

```bash
qemu-system-x86_64 \
  -machine q35 \
  -cpu max \
  -m 256M \
  -serial stdio \
  -no-reboot \
  -no-shutdown \
  -d int,cpu_reset,guest_errors \
  -D build/qemu-m7.log \
  -cdrom build/mcsos.iso
```

Hasil:

```text
[M6] PMM initialized
[M6] PMM selftest passed
[M7] VMM core initialized
[M4] ready for QEMU smoke test and GDB audit
```

Status: `PASS`

Note: Pada tahap M7 belum dilakukan fuzzing otomatis atau stress test multiprocess karena sistem masih berfokus pada validasi dasar Virtual Memory Manager dan integrasi paging kernel.


### 12.7 Visual Evidence

Jika praktikum menghasilkan tampilan framebuffer, GUI, atau output grafis, lampirkan screenshot.

| Screenshot | Lokasi file | Keterangan |
|---|---|---|
| `QEMU boot M7` | `screenshotsqemu-m7.png` | `Menunjukkan kernel berhasil boot dan log “[M7] VMM core initialized” muncul pada serial output.` |
| `Static verification PASS` | `screenshotsgrade-m7-pass.png` | `Menunjukkan host unit test VMM dan static verification berhasil lulus.` |
| `GDB breakpoint kernel dan trap` | `screenshotsgdb-kmain dan trap.png` | `Menunjukkan breakpoint pada fungsi kmain dan x86_64_trap_dispatch berhasil tercapai melalui QEMU gdbstub.` |

---

## 13. Hasil Uji

### 13.1 Tabel Ringkasan Hasil

| No. | Uji | Expected result | Actual result | Status | Evidence |
|---|---|---|---|---|---|
| 1 | Clean build kernel | Kernel ELF berhasil dibangun tanpa error | `build/mcsos-m5.elf` berhasil dibuat | PASS | Build log |
| 2 | Host unit test VMM | Seluruh host test VMM lulus | `M7 VMM host tests PASS` | PASS | `build/test_vmm_host` |
| 3 | Undefined symbol audit | Tidak ada undefined symbol pada object VMM | Output `nm -u build/vmm.o` kosong | PASS | `build/vmm.undefined.txt` |
| 4 | Disassembly audit | Instruksi `invlpg` dan akses `cr3` muncul | `objdump` menunjukkan `invlpg` dan `cr3` | PASS | `build/vmm.objdump.txt` |
| 5 | QEMU smoke test | Kernel berhasil boot dan serial log muncul | `[M7] VMM core initialized` muncul | PASS | `build/qemu-m7.log` |
| 6 | GDB debugging test | Breakpoint kernel dapat dicapai | Breakpoint `kmain` dan `x86_64_trap_dispatch` aktif | PASS | GDB session log |
| 7 | Page fault diagnostic | Handler #PF mencetak informasi fault | CR2 dan error code berhasil dicetak | PASS | Serial log QEMU |

### 13.2 Log Penting

```text
MCSOS 260502 M4 kernel entered

[M5] PIC/PIT initialized; interrupts enabled

[M6] PMM initialized
[M6] allocated_frame=0x0000000000001000
[M6] PMM selftest passed

[M7] VMM core initialized

[M4] IDT and exception dispatch path installed
[M4] ready for QEMU smoke test and GDB audit

M7 VMM host tests PASS
[PASS] static grade M7 selesai

Breakpoint 1, 0xffffffff80000280 in kmain ()
Breakpoint 3, 0xffffffff80001c90 in x86_64_trap_dispatch ()
```

### 13.3 Artefak Bukti

| Artefak | Path | SHA-256 / hash | Fungsi |
|---|---|---|---|
| `mcsos-m5.elf` | `build/mcsos-m5.elf` | `[isi dengan sha256sum]` | Kernel ELF binary |
| `mcsos.iso` | `build/mcsos.iso` | `[isi dengan sha256sum]` | Bootable ISO image |
| `qemu-m7.log` | `build/qemu-m7.log` | `[isi dengan sha256sum]` | Runtime dan fault log QEMU |
| `mcsos-m5.map` | `build/mcsos-m5.map` | `[isi dengan sha256sum]` | Linker map kernel |
| `disassembly.txt` | `build/disassembly.txt` | `[isi dengan sha256sum]` | Bukti disassembly kernel |
| `vmm.objdump.txt` | `build/vmm.objdump.txt` | `[isi dengan sha256sum]` | Audit instruksi VMM (`invlpg`, `cr3`) |
| `final_grade_m7.txt` | `build/final_grade_m7.txt` | `[isi dengan sha256sum]` | Ringkasan hasil validasi M7 |

Perintah hash:

```bash
sha256sum build/mcsos-m5.elf
sha256sum build/mcsos.iso
sha256sum build/qemu-m7.log
sha256sum build/mcsos-m5.map
sha256sum build/disassembly.txt
sha256sum build/vmm.objdump.txt
sha256sum build/final_grade_m7.txt
```

---

## 14. Analisis Teknis

### 14.1 Analisis Keberhasilan

```text
Implementasi M7 berhasil karena Virtual Memory Manager (VMM) dapat terintegrasi dengan Physical Memory Manager (PMM) tanpa merusak proses boot kernel. Host unit test berhasil memvalidasi operasi map, query, dan unmap halaman 4 KiB serta memastikan validasi canonical address dan alignment berjalan dengan benar.

Static verification menggunakan nm dan objdump menunjukkan bahwa object VMM bersifat freestanding tanpa undefined symbol serta mengandung instruksi penting seperti invlpg dan akses register CR3. Selain itu, QEMU smoke test menunjukkan kernel mampu melakukan boot hingga tahap inisialisasi VMM tanpa terjadi triple fault atau reset mendadak.

Integrasi trap dispatcher dan page fault diagnostic juga berhasil karena handler #PF mampu membaca CR2 dan error code CPU untuk kebutuhan diagnosis runtime kernel. Validasi menggunakan GDB menunjukkan simbol kernel dan breakpoint dapat digunakan dengan benar selama debugging.
```

### 14.2 Analisis Kegagalan atau Perbedaan Hasil

```text
Selama implementasi ditemukan beberapa kegagalan build dan integrasi. Salah satu masalah utama adalah duplicate symbol pada PMM akibat source file pmm.c ikut terkompilasi dua kali dari direktori berbeda. Masalah tersebut diperbaiki dengan membatasi source build hanya pada file yang diperlukan di Makefile.

Kegagalan lain muncul pada trap handler ketika struktur trap frame tidak memiliki field rsp seperti asumsi awal implementasi page fault diagnostic. Error tersebut diperbaiki dengan menyesuaikan logging terhadap field yang benar-benar tersedia pada x86_64_trap_frame_t.

Selain itu, sempat terjadi missing symbol vmm_space_init akibat source VMM belum dimasukkan ke pipeline build kernel. Masalah diselesaikan dengan menambahkan src/vmm.c ke daftar source pada Makefile. Setelah seluruh perbaikan dilakukan, kernel berhasil dibangun, dijalankan pada QEMU, dan divalidasi menggunakan host test serta GDB.
```

### 14.3 Perbandingan dengan Teori

| Konsep teori | Implementasi praktikum | Sesuai/tidak sesuai | Penjelasan |
|---|---|---|---|
| Paging 4-level x86_64 | Implementasi PML4, PDPT, PD, dan PT pada `vmm.c` | Sesuai | Struktur translasi virtual memory mengikuti arsitektur x86_64 standar |
| CR3 sebagai root page table | Implementasi `vmm_read_cr3()` dan `vmm_write_cr3()` | Sesuai | Kernel menggunakan CR3 untuk mengakses page table aktif |
| Virtual memory mapping | Operasi `map`, `query`, dan `unmap` halaman 4 KiB | Sesuai | VMM berhasil membangun dan menghapus mapping virtual address |
| TLB invalidation | Penggunaan instruksi `invlpg` setelah unmap | Sesuai | Translasi lama pada TLB berhasil diinvalidasi |
| Canonical address validation | Fungsi validasi address 48-bit pada VMM | Sesuai | Mapping non-canonical address berhasil ditolak |
| Page fault diagnostic | Handler #PF membaca CR2 dan error code | Sesuai | Kernel mampu melakukan diagnosis dasar page fault |
| Demand paging dan recovery | Belum diimplementasikan | Tidak sesuai | Recovery page fault belum menjadi target M7 |

### 14.4 Kompleksitas dan Kinerja

| Aspek | Estimasi/hasil | Bukti | Catatan |
|---|---|---|---|
| Kompleksitas algoritma | Operasi page table traversal ≈ `O(1)` dengan maksimum 4 level lookup | Desain paging x86_64 dan host unit test | Jumlah level paging tetap dan tidak bergantung pada ukuran memori |
| Waktu build | Relatif cepat pada WSL 2 (beberapa detik) | Log `make all` | Build masih berskala kecil dan belum melibatkan banyak modul kernel |
| Waktu boot QEMU | Boot berhasil hingga marker `[M7] VMM core initialized` | Serial log QEMU | Tidak ditemukan triple fault atau reset mendadak |
| Penggunaan memori | Menggunakan frame fisik 4 KiB untuk page table dan PMM bitmap | PMM log dan struktur VMM | Belum dilakukan optimisasi penggunaan memori |
| Latensi/throughput | Tidak dilakukan benchmark formal | Tidak tersedia | Fokus praktikum masih pada correctness dan validasi fungsi dasar |

## 15. Debugging dan Failure Modes

### 15.1 Failure Modes yang Ditemukan

| Failure mode | Gejala | Penyebab sementara | Bukti | Perbaikan |
|---|---|---|---|---|
| Undefined symbol `vmm_space_init` | Linker gagal membangun kernel ELF | `src/vmm.c` belum masuk pipeline build | Error `undefined symbol: vmm_space_init` saat `make all` | Menambahkan `src/vmm.c` ke Makefile |
| Duplicate symbol pada PMM | Linker menolak proses linking | `pmm.c` terkompilasi dua kali dari direktori berbeda | Error `duplicate symbol: pmm_alloc_frame` | Membatasi source build hanya pada file yang diperlukan |
| Error field `rsp` pada trap frame | Build gagal pada `trap.c` | Struktur `x86_64_trap_frame_t` tidak memiliki field `rsp` | Error `no member named 'rsp'` | Menghapus akses field `rsp` dari logging |
| Potensi stale TLB setelah unmap | Translasi lama dapat tetap aktif | Tidak ada invalidasi TLB | Audit desain VMM | Menambahkan instruksi `invlpg` setelah unmap |
| Potensi page fault tanpa diagnostic | Kernel dapat panic tanpa informasi fault | Handler #PF belum membaca CR2 | Belum ada log page fault awal | Menambahkan `page_fault_dump()` dan pembacaan CR2 |
| QEMU gagal boot ISO | File ISO belum terbentuk | `build/mcsos.iso` belum dibuat | Error `Could not open 'build/mcsos.iso'` | Membuat ISO menggunakan `xorriso` dan `limine bios-install` |

### 15.2 Failure Modes yang Diantisipasi

| Failure mode | Deteksi | Dampak | Mitigasi |
|---|---|---|---|
| Triple fault saat paging aktif | QEMU reset mendadak dan log `cpu_reset` | Kernel langsung reboot/halt | Tidak mengaktifkan CR3 baru tanpa mapping lengkap |
| Invalid virtual address mapping | Host unit test dan validasi canonical address | Page fault atau corruption page table | Menolak non-canonical address |
| Duplicate page mapping | Host test duplicate map | Overwrite page table dan memory corruption | Return `VMM_ERR_EXISTS` |
| Unaligned physical address | Host unit test alignment | Invalid page table entry | Validasi alignment 4 KiB |
| Stale TLB setelah unmap | Audit disassembly `invlpg` | Translasi lama tetap aktif | Melakukan invalidasi TLB menggunakan `invlpg` |
| Page fault tanpa diagnostic | Panic tanpa informasi debugging | Sulit melakukan diagnosis runtime | Menambahkan CR2 dan error code dump |
| Undefined symbol saat linking | Error linker `ld.lld` | Build kernel gagal | Audit source build dan Makefile |

### 15.3 Triage yang Dilakukan

```text
Diagnosis dilakukan secara bertahap dimulai dari pemeriksaan log serial QEMU untuk melihat marker boot dan exception kernel. Jika terjadi build atau runtime failure, dilakukan analisis menggunakan error compiler/linker, audit Makefile, dan pemeriksaan object build.

Untuk debugging runtime, digunakan GDB dengan breakpoint pada kmain dan x86_64_trap_dispatch untuk memeriksa RIP, RSP, dan alur trap handler. Static verification dilakukan menggunakan nm, objdump, dan readelf untuk memastikan tidak ada undefined symbol serta memastikan instruksi penting seperti invlpg dan akses CR3 muncul pada disassembly kernel.
```
### 15.4 Panic Path

```text
Pada tahap M7 tidak ditemukan kernel panic fatal selama proses boot normal dan QEMU smoke test. Namun, panic path tetap diuji melalui integrasi trap dispatcher dan page fault diagnostic handler.

Kernel telah mampu menangani exception #PF dengan membaca CR2 dan error code sebelum memanggil kernel panic. Validasi dilakukan melalui breakpoint GDB pada x86_64_trap_dispatch serta audit source pada fungsi page_fault_dump() dan KERNEL_PANIC().

Dengan demikian, panic path telah terpasang dan tervalidasi untuk kebutuhan diagnosis dasar runtime kernel pada tahap M7.
```
---

## 16. Prosedur Rollback

| Skenario rollback | Perintah | Data yang harus diselamatkan | Status |
|---|---|---|---|
| Kembali ke commit awal | `git checkout dc1ccc7` | Log build, hasil test, dan artefak M7 | Teruji |
| Revert commit praktikum | `git revert 202c4dc` | Host test log dan QEMU log | Belum |
| Bersihkan artefak build | `make clean` | Tidak ada, source code tetap aman | Teruji |
| Regenerasi image | `xorriso ... -o build/mcsos.iso` | File ISO lama jika diperlukan | Teruji |

Catatan rollback:

```text
Rollback sebagian telah diuji melalui penggunaan git checkout ke commit sebelumnya dan proses make clean untuk memastikan build dapat dilakukan ulang dari kondisi bersih. Revert commit penuh belum diuji secara eksplisit karena repository sudah berada pada kondisi stabil dan tervalidasi. Risiko utama rollback adalah hilangnya artefak build dan log validasi jika tidak disimpan terlebih dahulu.
```

---

## 17. Keamanan dan Reliability

### 17.1 Risiko Keamanan

| Risiko | Boundary | Dampak | Mitigasi | Evidence |
|---|---|---|---|---|
| Virtual address non-canonical | VMM mapping interface | Page fault atau corruption page table | Validasi canonical address 48-bit | Host unit test PASS |
| Unaligned page mapping | Page table management | Invalid page table entry dan undefined behavior | Validasi alignment 4 KiB | Host unit test alignment |
| Overwrite mapping yang sudah present | VMM subsystem | Memory corruption dan inconsistent mapping | Return `VMM_ERR_EXISTS` | Duplicate map test PASS |
| Stale TLB translation | TLB dan page unmap | CPU menggunakan translasi lama | Menggunakan instruksi `invlpg` | `objdump` menunjukkan `invlpg` |
| Invalid page fault handling | Trap dispatcher | Kernel panic tanpa diagnostic | Menambahkan CR2 dan error code dump | QEMU serial log dan source review |
| Invalid physical frame ownership | PMM subsystem | Double allocation atau memory corruption | Bitmap ownership validation | PMM selftest PASS |
| Undefined symbol atau dependency libc | Freestanding kernel boundary | Kernel gagal build atau runtime undefined | Audit `nm -u` dan freestanding compile | `build/vmm.undefined.txt` kosong |

### 17.2 Reliability dan Data Integrity

| Risiko reliability | Dampak | Deteksi | Mitigasi |
|---|---|---|---|
| Triple fault saat paging aktif | Kernel reset atau hang mendadak | QEMU log `cpu_reset` dan smoke test | Tidak mengaktifkan CR3 baru tanpa mapping lengkap |
| Inconsistent page table state | Mapping virtual memory menjadi corrupt | Host unit test dan QEMU validation | Menolak overwrite mapping yang sudah present |
| Stale TLB translation | CPU masih menggunakan translasi lama | Audit disassembly dan runtime review | Menggunakan instruksi `invlpg` setelah unmap |
| Invalid frame allocation | Double allocation atau memory corruption | PMM selftest dan bitmap validation | Validasi ownership frame fisik |
| Undefined symbol saat linking | Kernel gagal dibangun | Error linker `ld.lld` | Audit source build dan Makefile |
| Fault handler tidak valid | Kernel panic tanpa informasi debugging | QEMU serial log dan GDB | Menambahkan page fault diagnostic dan CR2 dump |
| Resource leak page table | Penggunaan frame fisik tidak terkendali | Review source dan host test | Menggunakan PMM free/unmap path dengan benar |

### 17.3 Negative Test

| Negative test | Input buruk | Expected result | Actual result | Status |
|---|---|---|---|---|
| Non-canonical virtual address | Virtual address invalid 48-bit | Mapping ditolak dengan error | Host test menolak address invalid | PASS |
| Unaligned physical address | Physical address tidak aligned 4 KiB | Mapping ditolak | Host test alignment gagal sesuai desain | PASS |
| Duplicate virtual mapping | Virtual address sudah present | Return `VMM_ERR_EXISTS` | Duplicate map test PASS | PASS |
| Query unmapped page | Virtual address belum di-map | Return error/not found | Query gagal tanpa corruption | PASS |
| Unmap unmapped page | Virtual address tidak memiliki mapping | Return error | Unmap gagal dengan aman | PASS |
| Invalid page fault access | Fault address invalid | Kernel mencetak CR2 dan error code | Page fault diagnostic aktif | `PASS` |

---

## 18. Pembagian Kerja Kelompok

| Nama | NIM | Peran | Kontribusi teknis | Commit/artefak |
|---|---|---|---|---|
| Neng Nagita Salma | 25832071004 | Anggota (kerja bersama) | Implementasi dan integrasi Virtual Memory Manager (VMM), build kernel, serta QEMU smoke test | `202c4dc` |
| Anisa Nur Azfa | 25832072003 | Anggota (kerja bersama) | Validasi host unit test, audit ELF/disassembly, dan debugging paging/trap handler | `202c4dc` |
| Lailatul Zulfa | 205832072001 | Anggota (kerja bersama) | Validasi GDB debugging, page fault diagnostics, grading M7, dan penyusunan laporan | `202c4dc` |

### 18.1 Mekanisme Koordinasi

```text
Praktikum dikerjakan secara bersama oleh seluruh anggota kelompok tanpa pembagian tugas yang kaku. Setiap anggota terlibat dalam proses implementasi, debugging, pengujian, dan penyusunan laporan secara kolaboratif.

Koordinasi dilakukan melalui diskusi langsung selama proses praktikum berlangsung. Seluruh perubahan pada kernel divalidasi bersama menggunakan build test, host unit test, QEMU smoke test, dan debugging menggunakan GDB sebelum commit akhir dilakukan.

Permasalahan seperti error linker, duplicate symbol, dan integrasi page fault handler diselesaikan bersama melalui pengujian ulang dan analisis log hingga sistem kembali buildable dan stabil.


### 18.2 Evaluasi Kontribusi


| Anggota | Persentase kontribusi yang disepakati | Bukti | Catatan |
|---|---:|---|---|
| Neng Nagita Salma |                       33% | commit Git, screenshot build environment, log terminal, dan dokumentasi praktikum | Berkontribusi bersama dalam seluruh tahap praktikum M0 |
| Anisa Nur Azfa    |                        34% | commit Git, screenshot build environment, log terminal, dan dokumentasi praktikum | Berkontribusi bersama dalam seluruh tahap praktikum M0 |
| Lailatul Zulfa    |                       33% | commit Git, screenshot build environment, log terminal, dan dokumentasi praktikum | Berkontribusi bersama dalam seluruh tahap praktikum M0 |
```
---

## 19. Kriteria Lulus Praktikum

| Kriteria minimum | Status | Evidence |
|---|---|---|
| Proyek dapat dibangun dari clean checkout | PASS | Build log `make clean && make all` |
| Perintah build terdokumentasi | PASS | Bagian 10 dan 12 laporan |
| QEMU boot atau test target berjalan deterministik | PASS | QEMU serial log M7 |
| Semua unit test/praktikum test relevan lulus | PASS | `M7 VMM host tests PASS` |
| Log serial disimpan | PASS | `build/qemu-m7.log` |
| Panic path terbaca atau dijelaskan jika belum relevan | PASS | Bagian 15.4 laporan |
| Tidak ada warning kritis pada build | PASS | Build log clang/ld.lld |
| Perubahan Git terkomit | PASS | Commit `202c4dc` dan tag `m7-final` |
| Desain dan failure mode dijelaskan | PASS | Bagian 9 dan 15 laporan |
| Laporan berisi screenshot/log yang cukup | PASS | Lampiran log build, QEMU, dan GDB |

Kriteria tambahan untuk praktikum lanjutan:

| Kriteria lanjutan | Status | Evidence |
|---|---|---|
| Static analysis dijalankan | PASS | `nm`, `objdump`, dan `readelf` audit |
| Stress test dijalankan | PASS | QEMU smoke/fault validation |
| Fuzzing atau malformed-input test dijalankan | NA | Belum menjadi scope M7 |
| Fault injection dijalankan | PASS | Page fault diagnostic dan QEMU validation |
| Disassembly/readelf evidence tersedia | PASS | `build/disassembly.txt` dan `readelf-*` |
| Review keamanan dilakukan | PASS | Bagian 17 laporan |
| Rollback diuji | PASS | `git checkout dc1ccc7` dan `make clean` |

---

## 20. Readiness Review

| Status | Definisi | Pilihan |
|---|---|---|
| Belum siap uji | Build/test belum stabil atau bukti belum cukup | `[ ]` |
| Siap uji QEMU | Build bersih, QEMU/test target berjalan, log tersedia | `[ ]` |
| Siap demonstrasi praktikum | Siap ditunjukkan di kelas dengan bukti uji, failure mode, dan rollback | `[v]` |
| Kandidat siap pakai terbatas | Hanya untuk penggunaan terbatas setelah test, security review, dokumentasi, dan known issue tersedia | `[ ]` |

Alasan readiness:

```text
Status “Siap demonstrasi praktikum” dipilih karena kernel berhasil dibangun dari clean checkout, host unit test VMM lulus, static verification berhasil dijalankan, dan kernel dapat melakukan boot pada QEMU dengan serial log yang konsisten.

Selain itu, debugging menggunakan GDB berhasil dilakukan dengan breakpoint pada kmain dan trap dispatcher. Failure mode, rollback procedure, serta page fault diagnostic juga telah dijelaskan dan divalidasi sesuai scope praktikum M7.
```

Known issues:

| No. | Issue | Dampak | Workaround | Target perbaikan |
|---|---|---|---|---|
| 1 | Demand paging dan recovery page fault belum tersedia | Kernel masih panic/halt saat fault fatal | Menggunakan diagnostic log dan panic handler | Milestone virtual memory lanjutan |
| 2 | Belum ada user/kernel isolation penuh | Proteksi memory masih terbatas | Hanya digunakan untuk kernel edukasi | Milestone security dan process management |
| 3 | Belum ada fuzzing otomatis | Reliability production belum tervalidasi | Menggunakan smoke test dan host unit test | Milestone testing lanjutan |

Keputusan akhir:

```text
Berdasarkan bukti build bersih, host unit test, static verification, QEMU serial log, serta validasi GDB, hasil praktikum ini layak disebut siap demonstrasi praktikum untuk milestone M7 Virtual Memory Manager dasar. Sistem belum layak disebut production-ready karena demand paging, recovery fault, dan proteksi memory lanjutan belum diimplementasikan.
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
Praktikum M7 berhasil mengimplementasikan Virtual Memory Manager (VMM) dasar pada kernel MCSOS berbasis arsitektur x86_64. Sistem berhasil mendukung operasi map, query, dan unmap halaman 4 KiB dengan validasi canonical address dan alignment sesuai spesifikasi paging x86_64.

Integrasi antara PMM dan VMM berhasil dilakukan tanpa merusak proses boot kernel. Host unit test, static verification menggunakan nm/objdump/readelf, QEMU smoke test, serta debugging menggunakan GDB juga berhasil dijalankan. Selain itu, kernel telah mampu melakukan diagnosis page fault melalui pembacaan CR2 dan error code pada trap handler.
```

### 22.2 Yang Belum Berhasil

```text
Implementasi pada M7 masih terbatas pada virtual memory manager dasar dan belum mendukung demand paging, page fault recovery, maupun user/kernel isolation penuh. Sistem juga belum memiliki memory protection tingkat lanjut seperti W^X enforcement dan KASLR.

Selain itu, belum dilakukan fuzzing otomatis, benchmark performa formal, maupun stress test multiprocessor sehingga reliability production-level belum dapat dipastikan.
```

### 22.3 Rencana Perbaikan

```text
Pengembangan berikutnya difokuskan pada implementasi demand paging, pengelolaan address space per proses, serta mekanisme proteksi memory yang lebih lengkap. Sistem juga perlu dikembangkan dengan scheduler dan user space support agar virtual memory dapat digunakan pada lingkungan multitasking.

Di sisi validasi, diperlukan penambahan stress test, fuzzing otomatis, dan security review yang lebih mendalam untuk meningkatkan reliability dan robustness kernel pada tahap selanjutnya.

```

---

## 23. Lampiran

### Lampiran A — Commit Log

```text
202c4dc (HEAD -> m6-pmm, tag: m7-final) M7 final: VMM complete + QEMU + GDB validated
dc1ccc7 M6: physical memory manager integrated and tested
0040213 M5: implement PIC, PIT, and timer IRQ handling
ade8ba9 M4 add x86_64 IDT and exception trap path
c7784b7 M3 panic path logging gdb and disassembly audit
```

### Lampiran B — Diff Ringkas

```diff
+ create mode 100644 include/vmm.h
+ create mode 100644 src/vmm.c
+ create mode 100644 tests/test_vmm_host.c
+ create mode 100755 scripts/grade_m7.sh
+ create mode 100755 scripts/m7_preflight.sh
+ create mode 100644 scripts/m7_gdb.cmd

+ [M7] VMM core initialized
+ page_fault_dump()
+ vmm_read_cr3()
+ invlpg support
```

### Lampiran C — Log Build Lengkap

```text
Log build lengkap tersedia pada:
- build/final_grade_m7.txt
- build/readelf-header.txt
- build/readelf-sections.txt
- build/readelf-program-headers.txt
- build/disassembly.txt
```

### Lampiran D — Log QEMU Lengkap

```text
Log QEMU lengkap tersedia pada:
- build/qemu-m7.log
- build/qemu-serial.log

Potongan log penting:

[M6] PMM initialized
[M6] PMM selftest passed
[M7] VMM core initialized
[M4] ready for QEMU smoke test and GDB audit
```

### Lampiran E — Output Readelf/Objdump

```text
Static verification berhasil dilakukan menggunakan:
- readelf -hW build/mcsos-m5.elf
- readelf -lW build/mcsos-m5.elf
- readelf -SW build/mcsos-m5.elf
- objdump -drwC build/mcsos-m5.elf

Hasil penting:
- ELF64 x86_64 valid
- Symbol kernel berhasil terdeteksi
- Instruksi invlpg ditemukan pada disassembly VMM
- Akses register CR3 ditemukan pada object VMM
```


### Lampiran F — Screenshot

| No. | File | Keterangan |
|---|---|---|
| 1 | `screenshotsqemu-m7.png` | Menunjukkan kernel berhasil boot dan log `[M7] VMM core initialized` muncul pada serial output. |
| 2 | `screenshotsgrade-m7-pass.png` | Menunjukkan host unit test VMM dan static verification berhasil lulus. |
| 3 | `screenshotsgdb-kmain dan trap.png` | Menunjukkan breakpoint pada fungsi `kmain` dan `x86_64_trap_dispatch` berhasil tercapai melalui QEMU gdbstub. |

### Lampiran G — Bukti Tambahan

```text
GDB debugging evidence:

Breakpoint 1, 0xffffffff80000280 in kmain ()
Breakpoint 3, 0xffffffff80001c90 in x86_64_trap_dispatch ()

Register dump:
rip = 0xffffffff80000280
rsp = 0xffff800007f7cfe8

Host validation:
M7 VMM host tests PASS
[PASS] static grade M7 selesai

QEMU validation:
[M6] PMM initialized
[M6] PMM selftest passed
[M7] VMM core initialized

Rollback evidence:
git checkout dc1ccc7
make clean
make all

Static audit evidence:
grep -q "invlpg" build/vmm.objdump.txt
grep -q "cr3" build/vmm.objdump.txt
nm -u build/vmm.o
```
---

## 24. Daftar Referensi

Gunakan format IEEE. Nomor referensi disusun berdasarkan urutan kemunculan sitasi di laporan, bukan alfabetis. Contoh format:

```text
[1] Intel, “Intel® 64 and IA-32 Architectures Software Developer Manuals,” Intel, updated Apr. 6, 2026. [Online]. Available: https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html

[2] Advanced Micro Devices, “AMD64 Architecture Programmer's Manual Volume 2: System Programming,” AMD, Rev. 3.44, Mar. 6, 2026. [Online]. Available: https://docs.amd.com/v/u/en-US/24593_3.44_APM_Vol2

[3] QEMU Project, “GDB usage — QEMU documentation,” QEMU. [Online]. Available: https://qemu.eu/doc/6.0/system/gdb.html

[4] Limine Bootloader Organization, “Limine,” GitHub organization and official mirror information. [Online]. Available: https://github.com/limine-bootloader

[5] limine crate documentation, “MemoryMapRequest in limine::request,” docs.rs. [Online]. Available: https://docs.rs/limine/latest/limine/request/struct.MemoryMapRequest.html

[6] limine-protocol crate documentation, “HHDMRequest,” docs.rs. [Online]. Available: https://docs.rs/limine-protocol/latest/limine_protocol/struct.HHDMRequest.html

[7] Free Software Foundation, “Using LD, the GNU linker — Scripts,” GNU manuals. [Online]. Available: https://ftp.gnu.org/old-gnu/Manuals/ld/html_node/ld_6.html

[8] LLVM Project, “Linker Script implementation notes and policy — LLD documentation,” LLVM. [Online]. Available: https://lld.llvm.org/ELF/linker_script.html

[9] LLVM Project, “Clang command line argument reference,” LLVM. [Online]. Available: https://clang.llvm.org/docs/ClangCommandLineReference.html
```

[1] Intel Corporation, Intel 64 and IA-32 Architectures Software Developer’s Manual, Intel Developer Documentation.

[2] OSDev Wiki, “Paging,” “Higher Half Kernel,” dan “Interrupt Descriptor Table (IDT),” https://wiki.osdev.org/

[3] QEMU Documentation dan GNU GDB Documentation, digunakan untuk validasi runtime kernel dan debugging pada lingkungan x86_64.

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
| Security/reliability dibahas | `Ya/` |
| Readiness review tidak berlebihan | `Ya` |
| Rubrik penilaian diisi atau disiapkan | `Ya` |
| Referensi memakai format IEEE | `Ya` |
| Laporan disimpan sebagai Markdown | `Ya` |

---

## 26. Pernyataan Pengumpulan

Kami mengumpulkan laporan ini bersama artefak pendukung pada commit:

```text
202c4dc
```

Status akhir yang diklaim:

```text
Siap demonstrasi praktikum
```

Ringkasan satu paragraf:

```text
Praktikum M7 berhasil mengimplementasikan Virtual Memory Manager (VMM) dasar pada kernel MCSOS berbasis x86_64 dengan dukungan operasi map, query, dan unmap halaman 4 KiB. Sistem berhasil divalidasi melalui host unit test, static verification menggunakan nm/objdump/readelf, QEMU smoke test, serta debugging menggunakan GDB. Integrasi PMM dan VMM berjalan tanpa merusak proses boot kernel, dan handler page fault telah mampu membaca CR2 serta error code untuk kebutuhan diagnosis runtime. Meskipun demikian, implementasi masih terbatas pada virtual memory manager dasar dan belum mencakup demand paging, recovery page fault, maupun proteksi memory tingkat lanjut. Pengembangan berikutnya difokuskan pada process address space, proteksi memory, dan pengujian reliability yang lebih mendalam.
```