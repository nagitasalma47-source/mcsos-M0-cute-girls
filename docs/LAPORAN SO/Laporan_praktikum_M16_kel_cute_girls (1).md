# Laporan Praktikum Sistem Operasi Lanjut — MCSOS

**Nama file laporan:** `laporan_praktikum_M16_cute girls.md`  
**Nama sistem operasi:** MCSOS 260502  
**Target default:** x86_64, QEMU, Windows 11 x64 + WSL 2, kernel monolitik pendidikan, C freestanding dengan assembly minimal, POSIX-like subset  
**Dosen:** Muhaemin Sidiq, S.Pd., M.Pd.  
**Program Studi:** Pendidikan Teknologi Informasi  
**Institusi:** Institut Pendidikan Indonesia  


---

## 0. Metadata Laporan

| Atribut | Isi |
|---|---|
| Kode praktikum | `M16` |
| Judul praktikum | `MCSFS1J — Crash-Consistency Teaching Journal dan Journal Recovery` |
| Jenis pengerjaan | `Kelompok` |
| Nama kelompok | `cute girls` |
| Anggota kelompok | `Neng Nagita Salma (25832071004) — Anisa Nur Azfa (25832072003) — Lailatul Zulfa (25832072001)` |
| Kelas | `1A` |
| Tanggal praktikum | `2026-06-13` |
| Tanggal pengumpulan | `2026-07-01` |
| Repository | `https://github.com/nagitasalma47-source/mcsos-M0-cute-girls.git` |
| Branch | `praktikum-m16-journal-recovery` |
| Commit awal | `` 0d87222 `` |
| Commit akhir | `` 274e01f `` |
| Status readiness yang diklaim | `belum siap uji ` |

---

## 1. Sampul

# Laporan Praktikum `M16`  
## `MCSFS1J — Crash-Consistency Teaching Journal dan Journal Recovery`

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
- Menanyakan cara menyusun struktur journal write-ahead (descriptor block, commit record, recovery)
- Meminta penjelasan error saat kompilasi freestanding dan host test
- Meminta bantuan penyusunan laporan M16

Sumber:
- Dokumentasi praktikum MCSOS
- ChatGPT

Bagian yang dibantu:
- Penjelasan konsep (write-ahead journal, commit record, crash-consistency)
- Penyelesaian error (Makefile tab/heredoc, target freestanding)
- Penyusunan laporan dan format tabel

Verifikasi mandiri:
- Menjalankan semua command sendiri di terminal (make -C tests/m16, qemu, readelf, objdump)
- Memastikan output sesuai (ELF64 x86-64, host test PASS, journal recovery PASS)
- Memastikan repository dan GitHub sesuai hasil praktikum.
```

---

## 3. Tujuan Praktikum

Tuliskan tujuan teknis dan konseptual praktikum. Tujuan harus dapat diuji.

1. `Tujuan teknis 1: mengimplementasikan write-ahead journal (MCSFS1J) di atas filesystem MCSFS1 untuk menjaga crash-consistency`
2. `Tujuan teknis 2: menghasilkan object freestanding dan host test yang dapat diverifikasi dengan readelf/objdump/nm`
3. `Tujuan konseptual 1: menjelaskan mekanisme commit record, replay, dan rejection terhadap journal descriptor yang corrupt`
4. `Tujuan validasi: menyimpan log build, log QEMU serial, evidence readelf/objdump/nm, dan hasil host test`

---

## 4. Capaian Pembelajaran Praktikum

Setelah praktikum ini, mahasiswa mampu:

| CPL/CPMK praktikum | Bukti yang harus ditunjukkan |
|---|---|
| Menjelaskan konsep write-ahead journal dan crash-consistency pada filesystem sederhana | Implementasi `m16_journal_commit`, `m16_journal_recover` pada `kernel/fs/mcsfs1j/m16_mcsfs_journal.c` |
| Membuat struktur journal (header, descriptor, payload, commit record) beserta checksum-nya | Struct `m16_journal_header`, `m16_journal_desc`, fungsi `m16_header_checksum`, `m16_checksum` |
| Menguji skenario crash mid-transaction dan memastikan recovery dapat me-replay transaksi yang sudah committed | Host test `M16 host tests PASS`, skenario `write_file_ex(..., stop_after_commit_record=1)` lalu `m16_journal_recover` |
| Menguji penanganan kegagalan tertutup (fail closed) ketika descriptor journal corrupt | Test `corrupt descriptor rejected` menghasilkan `M16_E_CORRUPT` |
| Mengompilasi modul filesystem secara host (untuk testing) dan freestanding (untuk kernel) | `tests/m16/Makefile` target `host` dan `m16_mcsfs_journal.o` |
| Mengintegrasikan modul journal ke dalam kernel MCSOS dan memverifikasi simbolnya pada ELF kernel | `nm build/mcsos-m5.elf | grep m16_` |
| Membuktikan modul berjalan pada QEMU melalui log serial | `evidence/m16/qemu_serial.log` berisi `[M16] mcsfs1j linked into kernel` |
| Memahami pentingnya evidence teknis seperti log build, commit hash, checksum, dan pemeriksaan object file | Output `git log`, SHA-256 `m16_mcsfs_journal.o`/`test_m16`, `readelf`, `objdump`, `nm` |




## 5. Peta Milestone MCSOS

Centang milestone yang menjadi fokus laporan ini. Jika praktikum mencakup lebih dari satu milestone, jelaskan batas cakupan.

| Milestone | Fokus | Status dalam laporan |
|---|---|---|
| M0 | Requirements, governance, baseline arsitektur | `[v] tidak dibahas / [ ] dibahas / [ ] selesai praktikum` |
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
| M16 | Observability, update/rollback, release image, readiness review | `[ ] tidak dibahas / [ ] dibahas / [v] selesai praktikum` |
Batas cakupan praktikum:

```text
Praktikum M16 berfokus pada implementasi MCSFS1J, yaitu lapisan write-ahead journal (crash-consistency teaching journal) di atas filesystem MCSFS1 yang sudah dibuat pada milestone sebelumnya (M15). Praktikum ini mencakup pembuatan struktur journal (header, descriptor, payload block, commit record), fungsi commit dan recovery (replay), fungsi fsck, host unit test untuk menguji skenario normal, skenario crash mid-transaction, dan skenario descriptor journal yang corrupt, kompilasi freestanding modul ke dalam object kernel, integrasi simbol m16_* ke dalam kernel MCSOS, build ISO, serta verifikasi melalui QEMU serial log.

Praktikum M16 tidak mencakup implementasi journal multi-device, journal yang persisten lintas reboot pada storage fisik sungguhan, recovery terhadap korupsi pada banyak record sekaligus, maupun integrasi penuh MCSFS1J sebagai filesystem default VFS. M16 juga tidak mengklaim sistem operasi siap produksi atau journal yang teruji menyeluruh terhadap seluruh kelas fault. Fokus utama M16 adalah membuktikan mekanisme dasar write-ahead journal: commit, replay, dan fail-closed terhadap data yang corrupt, beserta evidence engineering pendukungnya.
```

---

## 6. Dasar Teori Ringkas

Praktikum M16 melanjutkan filesystem MCSFS1 (M15) dengan menambahkan lapisan journal bernama MCSFS1J untuk menjaga crash-consistency. Konsep utamanya adalah write-ahead logging (WAL): sebelum data ditulis langsung ke lokasi aslinya (in-place update), perubahan dicatat terlebih dahulu ke area journal pada block device, lalu ditandai "committed" melalui sebuah header journal yang dilengkapi checksum. Jika sistem crash sebelum commit header ditulis, journal dianggap kosong dan tidak ada perubahan yang direplay. Jika crash terjadi setelah commit header ditulis namun sebelum data asli ter-update, proses recovery akan membaca kembali journal dan me-replay (menulis ulang) setiap record menuju lokasi aslinya (target_lba).

Setiap entri journal terdiri dari descriptor block (berisi magic number, target LBA, dan checksum payload) serta payload block (isi data yang akan ditulis). Proses recovery memvalidasi checksum header journal dan checksum tiap payload sebelum melakukan replay; jika checksum atau magic number tidak valid, recovery akan menolak (fail closed) dengan kode error `M16_E_CORRUPT`, alih-alih menerapkan data yang tidak dapat dipercaya ke lokasi target yang tidak diketahui.

Modul ini dikompilasi dalam dua mode: mode host (dengan flag `-DMCSOS_M16_HOST_TEST`) untuk menjalankan unit test menggunakan `printf` di lingkungan Linux WSL, dan mode freestanding (`-ffreestanding -fno-builtin`, tanpa libc) untuk dilink langsung ke dalam image kernel MCSOS x86_64. Pemeriksaan hasil kompilasi freestanding dilakukan menggunakan `nm -u` (memastikan tidak ada symbol undefined yang berasal dari libc), `readelf -h` (memastikan target ELF64 x86-64), dan `objdump -d` (memeriksa hasil disassembly).

### 6.1 Konsep Sistem Operasi yang Diuji

```text
Pada praktikum M16, konsep sistem operasi yang diuji adalah mekanisme crash-consistency pada filesystem melalui write-ahead journal, yaitu komponen lanjutan dari filesystem layer (MCSFS1) yang dibangun pada M15.

Konsep utama yang digunakan meliputi:

1. Write-Ahead Journal (WAL)
Sebelum data ditulis ke lokasi asli (in-place), perubahan dicatat dulu ke area journal pada block device, kemudian ditandai committed melalui header journal.

2. Journal Descriptor dan Payload
Setiap transaksi terdiri atas descriptor block (target LBA + checksum) dan payload block (data sebenarnya), disusun dalam struct m16_journal_desc dan m16_jrec.

3. Commit Record dan Checksum
Header journal (m16_journal_header) memuat magic number, jumlah record, sequence number, dan checksum header (m16_header_checksum) yang dihitung dari seluruh isi header sebelum disimpan.

4. Crash Recovery (Replay)
Fungsi m16_journal_recover membaca header journal; jika checksum valid dan status committed, setiap descriptor divalidasi lalu payload-nya ditulis ulang (replay) ke target_lba masing-masing.

5. Fail-Closed terhadap Korupsi
Jika descriptor journal di-corrupt (magic number atau checksum tidak sesuai), recovery menghentikan proses dan mengembalikan M16_E_CORRUPT, bukan menerapkan data yang tidak valid.

6. fsck (filesystem check)
Fungsi m16_fsck memverifikasi konsistensi inode bitmap, block bitmap, inode root, dan direktori root setelah format maupun setelah recovery.

7. Dual-target compilation
Modul dikompilasi dalam mode host (untuk unit test menggunakan printf) dan mode freestanding (untuk dilink ke dalam kernel), masing-masing diverifikasi dengan tools binutils.

Dengan demikian, M16 berfokus pada pembuktian mekanisme dasar crash-consistency (commit, replay, dan rejection) di atas filesystem MCSFS1 yang telah dibangun sebelumnya.
```

### 6.2 Konsep Arsitektur x86_64 yang Relevan

| Konsep | Relevansi pada praktikum | Bukti/verifikasi |
|---|---|---|
| Long mode | Mode 64-bit tempat kernel beserta modul m16_mcsfs_journal dieksekusi | QEMU, readelf |
| Paging | Alamat kernel virtual tempat fungsi m16_* dimuat (mis. `ffffffff80002fd0`) | nm build/mcsos-m5.elf |
| Linking | Modul m16_mcsfs_journal.o dilink statis bersama objek kernel lain via ld.lld | build/mcsos-m5.map |
| ELF Relocatable | Output objek freestanding sebelum linking, diperiksa berdiri sendiri | readelf -h m16_mcsfs_journal.o |
| Serial/UART | Saluran log evidence runtime modul M16 di QEMU | evidence/m16/qemu_serial.log |

### 6.3 Konsep Implementasi Freestanding

| Aspek | Keputusan praktikum |
|---|---|
| Bahasa | `C11/C17 freestanding` |
| Runtime | `tanpa hosted libc (mode kernel); printf hanya dipakai di mode host test)` |
| ABI | `x86_64 System V` |
| Compiler flags kritis | `-ffreestanding, -fno-builtin, -fno-stack-protector, -mno-red-zone, -Wall -Wextra -Werror` |
| Risiko undefined behavior | `checksum salah jika struct tidak aligned, off-by-one pada index record journal, pointer ke block array di luar batas` |

### 6.4 Referensi Teori yang Digunakan

| No. | Sumber | Bagian yang digunakan | Alasan relevansi |
|---|---|---|---|
| 1 | OSDev Wiki (https://wiki.osdev.org) | Journaling Filesystem, ext3/ext4 journal concept | Referensi dasar konsep write-ahead journal pada filesystem |
| 2 | LLVM/Clang Documentation (https://clang.llvm.org/docs/) | Command-line options, target triple, -ffreestanding | Menjelaskan penggunaan flag freestanding dan kompilasi dual-target |
| 3 | GNU Binutils Documentation (https://sourceware.org/binutils/docs/) | nm, readelf, objdump | Digunakan untuk memverifikasi object freestanding dan symbol kernel |
| 4 | R. H. Arpaci-Dusseau & A. C. Arpaci-Dusseau, *Operating Systems: Three Easy Pieces* | Bab Crash Consistency: FSCK and Journaling | Dasar teori write-ahead logging dan recovery |
| 5 | Git Documentation (https://git-scm.com/docs) | Basic Git commands | Digunakan untuk manajemen branch dan commit praktikum M16 |

## 7. Lingkungan Praktikum

### 7.1 Host dan Target

| Komponen | Nilai |
|---|---|
| Host OS | `Windows 11 x64 (WSL 2)` |
| Lingkungan build | `WSL 2 Ubuntu 24.04.4 LTS (noble)` |
| Target ISA | `x86_64` |
| Target ABI | `x86_64-unknown-none-elf` |
| Emulator | `QEMU 8.2.2 (Debian 1:8.2.2+ds-0ubuntu1.16)` |
| Firmware emulator | `QEMU q35, BIOS Limine (mode -display none, -no-reboot, -no-shutdown)` |
| Debugger | `GDB (tidak digunakan langsung pada M16, hanya tersedia)` |
| Build system | `Make (GNU Make 4.3)` |
| Bahasa utama | `C17 freestanding (kernel), C11 (host test)` |
| Assembly | `tidak ditambahkan pada M16 (hanya isr.S/syscall_entry.S existing)` |

### 7.2 Versi Toolchain

Tempel output versi toolchain berikut. Dijalankan via `scripts/m16_preflight.sh`.

```bash
date -Iseconds
uname -a
lsb_release -a
clang --version | head -n 1
make --version | head -n 1
nm --version | head -n 1
readelf --version | head -n 1
objdump --version | head -n 1
sha256sum --version | head -n 1
qemu-system-x86_64 --version | head -n 1
git status --short
git rev-parse --short HEAD
```

Output:

```text
2026-06-13T13:08:40+07:00
Linux ASUS 6.6.87.2-microsoft-standard-WSL2 #1 SMP PREEMPT_DYNAMIC Thu Jun  5 18:30:46 UTC 2025 x86_64 x86_64 x86_64 GNU/Linux
Distributor ID: Ubuntu
Description:    Ubuntu 24.04.4 LTS
Release:        24.04
Codename:       noble
Ubuntu clang version 18.1.3 (1ubuntu1)
GNU Make 4.3
GNU nm (GNU Binutils for Ubuntu) 2.42
GNU readelf (GNU Binutils for Ubuntu) 2.42
GNU objdump (GNU Binutils for Ubuntu) 2.42
sha256sum (GNU coreutils) 9.4
QEMU emulator version 8.2.2 (Debian 1:8.2.2+ds-0ubuntu1.16)
0d87222
```

### 7.3 Lokasi Repository

| Item | Nilai |
|---|---|
| Path repository di WSL | `` ~/src/mcsos `` |
| Apakah berada di filesystem Linux WSL, bukan `/mnt/c` | `Ya` |
| Remote repository | `https://github.com/nagitasalma47-source/mcsos-M0-cute-girls.git` |
| Branch | `praktikum-m16-journal-recovery` |
| Commit hash awal | `` 0d87222 `` |
| Commit hash akhir | `` 274e01f `` |

---

## 8. Repository dan Struktur File

### 8.1 Struktur Direktori yang Relevan

Tampilkan hanya direktori dan file yang relevan dengan praktikum.

```text
mcsos/
  kernel/
    fs/
      mcsfs1j/
        m16_mcsfs_journal.c
        mcsfs1j_adapter.h
    block/
      block_demo.c
  fs/
    mcsfs1/
      mcsfs1.c
      mcsfs1.h
  tests/
    m16/
      Makefile
  scripts/
    m16_preflight.sh
  evidence/
    m16/
  logs/
    m16/
  build/
    mcsos-m5.elf
    mcsos.iso
  Makefile
```

### 8.2 File yang Dibuat atau Diubah

| File | Jenis perubahan | Alasan perubahan | Risiko |
|---|---|---|---|
| kernel/fs/mcsfs1j/m16_mcsfs_journal.c | baru | Implementasi inti write-ahead journal MCSFS1J (format, commit, recovery, fsck, host test) | Sedang (logika crash-consistency kritis) |
| kernel/fs/mcsfs1j/mcsfs1j_adapter.h | baru | Header adapter publik (format/mount/fsck/read/write) untuk integrasi VFS di masa depan | Rendah (deklarasi fungsi) |
| kernel/block/block_demo.c | ubah | Menambahkan log `[M16] mcsfs1j linked into kernel` dan `[M16] journal recovery support available` | Rendah (hanya logging) |
| scripts/m16_preflight.sh | baru | Script pencatat metadata host, toolchain, dan probe subsistem sebelum praktikum M16 dimulai | Rendah (hanya pencatatan) |
| tests/m16/Makefile | baru | Build host test (`test_m16`) dan build object freestanding (`m16_mcsfs_journal.o`) beserta audit nm/readelf/objdump/sha256sum | Sedang (jika salah, build/verifikasi gagal) |
| evidence/m16/*.log, *.txt, *.sha256 | baru | Menyimpan bukti hasil host test, build, dan audit binary sebagai evidence praktikum | Rendah (dokumentasi/bukti) |

### 8.3 Ringkasan Diff

```bash
git status --short
git diff --stat
git log --oneline -n 5
```

Output:

```text
git status --short
A  evidence/m16/nm_undefined.txt
A  evidence/m16/objdump_disasm.txt
A  evidence/m16/readelf_header.txt
A  evidence/m16/sha256sum.txt
A  evidence/m16/test_m16.sha256
M  kernel/block/block_demo.c
A  kernel/fs/mcsfs1j/m16_mcsfs_journal.c
A  kernel/fs/mcsfs1j/mcsfs1j_adapter.h
A  scripts/m16_preflight.sh
A  tests/m16/Makefile
A  tests/m16/nm_undefined.txt
A  tests/m16/objdump_disasm.txt
A  tests/m16/readelf_header.txt
A  tests/m16/sha256sum.txt

git diff --stat
 kernel/block/block_demo.c | 2 ++
 1 file changed, 2 insertions(+)

git log --oneline -n 5
274e01f (HEAD -> praktikum-m16-journal-recovery, origin/praktikum-m16-journal-recovery) M16 journal recovery implementation and verification
0d87222 (sebelum M16 dimulai)
```

---

## 9. Desain Teknis

### 9.1 Masalah yang Diselesaikan

```text
Pada praktikum M16, masalah utama yang diselesaikan adalah bagaimana menjaga konsistensi filesystem MCSFS1 (dari M15) ketika terjadi crash di tengah-tengah operasi tulis (write_file). Tanpa journal, sebuah crash mid-write dapat membuat bitmap, inode table, dan direktori menjadi tidak konsisten satu sama lain.

Beberapa masalah teknis yang dihadapi antara lain:
- Kesalahan saat membuat file C lewat heredoc (cat > ... <<'EOF') yang sempat tertimpa perintah lain sehingga isi file rusak dan harus dihapus ulang (rm kernel/fs/mcsfs1j/m16_mcsfs_journal.c)
- Baris-baris awal hasil heredoc yang tidak sengaja tercampur dengan perintah shell sebelumnya, sehingga harus dipotong ulang dengan `tail -n +3`
- Penulisan Makefile tests/m16/Makefile yang sempat tertimpa kosong (0 byte) karena heredoc dijalankan tanpa delimiter EOF yang benar, sehingga harus dikembalikan dari backup (`Makefile.sebelum_panduan`)
- File adapter header yang sempat dibuat pada direktori salah (di dalam tests/m16/kernel/fs/mcsfs1j) akibat working directory yang tidak sesuai, sehingga harus dipindahkan (mv) ke lokasi yang benar
- Memastikan modul journal dapat dikompilasi baik secara host (dengan printf untuk testing) maupun freestanding (tanpa libc, untuk dilink ke kernel) tanpa undefined symbol

Masalah-masalah tersebut diselesaikan dengan memeriksa ulang isi file menggunakan `head`/`tail`/`wc -l`, memindahkan file ke lokasi yang benar dengan `mv`, mengembalikan Makefile dari backup, serta memvalidasi hasil akhir menggunakan `clang -fsyntax-only`, `make -C tests/m16 clean all`, dan menjalankan `./tests/m16/test_m16`.
```

### 9.2 Keputusan Desain

| Keputusan | Alternatif yang dipertimbangkan | Alasan memilih | Konsekuensi |
|---|---|---|---|
| Journal terpisah dari MCSFS1 (modul baru MCSFS1J) | Menambahkan journaling langsung ke mcsfs1.c lama | Memisahkan modul memudahkan pengujian terisolasi tanpa mengubah filesystem M15 yang sudah ada | Perlu adapter terpisah (`mcsfs1j_adapter.h`) untuk integrasi VFS di masa depan |
| Layout journal statis: 1 block header + N descriptor + N data block | Journal dengan ukuran dinamis/circular buffer | Lebih sederhana untuk tujuan pendidikan dan mudah diverifikasi dengan unit test | Kapasitas journal terbatas (`M16_JOURNAL_MAX_RECORDS = 8`) |
| Checksum sederhana (custom `m16_checksum`) untuk header dan payload | Menggunakan CRC32 standar | Implementasi checksum custom cukup untuk mendeteksi korupsi pada skala device test (128 block) dan tidak memerlukan tabel lookup tambahan | Tidak sekuat CRC32 standar terhadap kolisi |
| Dua mode kompilasi (host test dan freestanding) dalam satu source file (`#ifdef MCSOS_M16_HOST_TEST`) | Memisahkan source host dan kernel menjadi dua file berbeda | Menjaga logika inti tetap satu sumber kebenaran (single source of truth) sehingga tidak ada drift antara versi test dan versi kernel | Source sedikit lebih kompleks karena ada blok `#ifdef` |
| Fail-closed pada descriptor journal corrupt (`M16_E_CORRUPT`) | Mengabaikan record yang corrupt dan melanjutkan replay record lain | Menerapkan payload ke target_lba yang tidak terverifikasi sangat berisiko merusak data lain; fail-closed lebih aman untuk filesystem pendidikan | Recovery berhenti total jika satu descriptor corrupt, butuh fsck manual |

### 9.3 Arsitektur Ringkas

Tambahkan diagram ASCII atau Mermaid. Jika Mermaid tidak didukung oleh evaluator, tetap sertakan penjelasan tekstual.

```mermaid
flowchart TD
m16_write_file_ex
     ↓
m16_tx_add (kumpulkan record ke transaksi)
     ↓
m16_journal_commit (tulis descriptor+payload, lalu commit header+checksum)
     ↓
[opsional: crash disimulasikan setelah commit record]
     ↓
m16_journal_recover (validasi header & descriptor, replay payload ke target_lba)
     ↓
m16_fsck (verifikasi bitmap, inode, direktori konsisten)
```

Penjelasan diagram:

```text
Arsitektur MCSFS1J dimulai dari pemanggilan m16_write_file_ex, yang mengumpulkan perubahan (perubahan inode bitmap, block bitmap, inode table, direktori, dan data block baru) ke dalam sebuah transaksi (struct m16_tx) menggunakan m16_tx_add.

Transaksi tersebut kemudian di-commit melalui m16_journal_commit: journal dibersihkan terlebih dahulu (m16_journal_clear), setiap record ditulis sebagai pasangan descriptor block (berisi target_lba dan checksum payload) dan data block, lalu ditutup dengan commit header (m16_journal_header) yang checksum-nya dihitung melalui m16_header_checksum. Parameter stop_after_commit_record memungkinkan simulasi crash tepat setelah commit header ditulis, sebelum data benar-benar diterapkan ke target_lba aslinya.

Ketika m16_mount atau pengujian secara eksplisit memanggil m16_journal_recover, fungsi ini membaca header journal, memvalidasi checksum dan status committed, lalu untuk tiap descriptor memvalidasi magic number, target LBA, dan checksum payload sebelum menulis ulang (replay) payload ke target_lba. Jika seluruh proses sukses, journal dibersihkan kembali (m16_journal_clear).

Setelah recovery, m16_fsck dipanggil untuk memverifikasi bahwa inode bitmap, block bitmap, inode root, dan entri direktori root tetap konsisten satu sama lain.

Batas tanggung jawab:
- m16_write_file_ex: menyiapkan perubahan dan memicu commit
- m16_journal_commit: menjamin atomicity penulisan melalui commit record + checksum
- m16_journal_recover: menjamin durability/consistency setelah crash dengan replay atau fail-closed
- m16_fsck: memverifikasi invariant struktur filesystem setelah recovery.
```

### 9.4 Kontrak Antarmuka

| Antarmuka | Pemanggil | Penerima | Precondition | Postcondition | Error path |
|---|---|---|---|---|---|
| `m16_format(dev)` | Host test / kernel init | `m16_mcsfs_journal.c` | `dev` sudah di-`m16_dev_init` | Superblock, bitmap, inode table, root dir tertulis; journal kosong | `M16_E_INVAL` jika `dev == NULL` |
| `m16_write_file_ex(dev, name, data, size, stop_after_commit_record)` | `m16_write_file` / host test | Journal + filesystem | Filesystem sudah di-format dan di-mount; nama file belum ada | File baru tercatat di direktori dan data ter-commit ke journal | `M16_E_INVAL`/`M16_E_IO` jika slot/inode/block penuh atau parameter tidak valid |
| `m16_journal_recover(dev)` | `m16_mount` / host test | Journal area pada `dev` | `dev` valid, journal mungkin berisi transaksi committed | Semua record committed di-replay ke `target_lba`; journal dibersihkan | `M16_E_CORRUPT` jika checksum/magic descriptor tidak valid |
| `m16_fsck(dev)` | Host test / setelah recovery | Struktur filesystem pada `dev` | Filesystem sudah di-mount | Mengonfirmasi inode root, bitmap, dan direktori konsisten | `M16_E_INVAL` jika invariant root inode/direktori dilanggar |
| `m16_read_file(dev, name, out, cap, out_size)` | Host test / kernel | Inode + data block | File `name` ada pada direktori root | Data file disalin ke buffer `out` | `M16_E_INVAL` jika file tidak ditemukan atau ukuran buffer tidak cukup |

### 9.5 Struktur Data Utama

| Struktur data | Field penting | Ownership | Lifetime | Invariant |
|---|---|---|---|---|
| `struct m16_super` | magic, version, total_blocks, clean_generation | Filesystem (block 0) | Dibuat saat `m16_format`, dibaca saat `m16_mount` | `sizeof(struct m16_super) == M16_BLOCK_SIZE` (`_Static_assert`) |
| `struct m16_inode` | used, kind, size, direct[M16_DIRECT_BLOCKS] | Inode table (`M16_INODE_TABLE_LBA`) | Hidup selama filesystem ter-mount | `sizeof(struct m16_inode) == 128` byte (`_Static_assert`) |
| `struct m16_journal_header` | magic, status (`M16_J_EMPTY`/`M16_J_COMMITTED`), record_count, seq, header_checksum | Journal block (`M16_JOURNAL_START`) | Dibuat saat commit, dihapus saat `m16_journal_clear` | Hanya dianggap valid jika `header_checksum` cocok dan `status == M16_J_COMMITTED` |
| `struct m16_journal_desc` | magic (`M16_JMAGIC`), target_lba, payload_checksum | Journal descriptor block | Hidup selama satu siklus commit→recover | `target_lba` harus valid menurut `m16_valid_lba` sebelum di-replay |
| `struct m16_tx` | `rec[M16_JOURNAL_MAX_RECORDS]`, count | Stack lokal pemanggil `m16_write_file_ex` | Hidup selama satu transaksi tulis | `count <= M16_JOURNAL_MAX_RECORDS` |

### 9.6 Invariants

Tuliskan invariant yang harus benar sepanjang eksekusi.

1. `Tidak ada data yang diterapkan ke target_lba kecuali sudah melewati commit header dengan checksum valid (status M16_J_COMMITTED).`
2. `Setiap descriptor yang akan di-replay harus memiliki magic == M16_JMAGIC dan target_lba yang valid menurut m16_valid_lba; jika tidak, recovery harus berhenti dengan M16_E_CORRUPT (fail closed).`
3. `sizeof(struct m16_super) == M16_BLOCK_SIZE dan sizeof(struct m16_inode) == 128 byte, dijamin oleh _Static_assert pada saat kompilasi.`
4. `Setelah m16_journal_recover berhasil (baik tanpa replay maupun setelah replay), journal harus dalam keadaan bersih (m16_journal_clear) sebelum digunakan kembali.`

### 9.7 Ownership, Locking, dan Concurrency

| Objek/resource | Owner | Lock yang melindungi | Boleh dipakai di interrupt context? | Catatan |
|---|---|---|---|---|
| `struct m16_blockdev` (block array in-memory) | Pemanggil (host test / kernel init) | none | Tidak | Diakses secara sekuensial; satu device per pengujian |
| Journal area (`M16_JOURNAL_START` .. blok descriptor/data) | `m16_journal_commit` / `m16_journal_recover` | none | Tidak | Tidak ada concurrency; commit dan recovery dipanggil sekuensial pada host test |
| Inode table & bitmap | `m16_write_file_ex` / `m16_fsck` | none | Tidak | Dibaca-modifikasi-ditulis dalam satu pemanggilan, tanpa interleaving |
| Symbol kernel `m16_*` pada `build/mcsos-m5.elf` | Linker (`ld.lld`) | none | Tidak | Hanya dilink statis, belum dipanggil oleh scheduler/IRQ pada M16 |

Lock order yang berlaku:

```text
Pada tahap M16, belum ada mekanisme locking maupun concurrency nyata karena modul journal masih diuji secara sekuensial (host test) dan baru dilink ke kernel tanpa dipanggil dari jalur interrupt atau thread paralel. Seluruh akses ke struct m16_blockdev berjalan sekuensial dalam satu konteks pemanggil.
```

### 9.8 Memory Safety dan Undefined Behavior Risk

| Risiko | Lokasi | Mitigasi | Bukti |
|---|---|---|---|
| Out-of-bounds pada array `dev->blocks[lba]` | `m16_read_block` / `m16_write_block` | Validasi `m16_valid_lba` sebelum akses block | Host test `M16 host tests PASS`, tidak ada crash/segfault |
| Buffer overflow saat menyalin nama file | `m16_write_file_ex` (`m16_copy(dir[slot].name, name, name_len)`) | `name_len` dibatasi `m16_strlen_bounded(name, M16_MAX_NAME)` | Review kode dan kompilasi `-Wall -Wextra -Werror` tanpa warning |
| Penerapan data tidak terverifikasi ke `target_lba` saat crash | `m16_journal_recover` | Validasi `m16_valid_lba` dan `payload_checksum` sebelum `m16_write_block` | Test `corrupt descriptor rejected` menghasilkan `M16_E_CORRUPT` |
| Alignment struct journal pada block 512 byte | `struct m16_journal_header`, `struct m16_journal_desc` | `_Static_assert` pada struct kritis (`m16_super`, `m16_inode`) | Kompilasi freestanding sukses tanpa error alignment |

### 9.9 Security Boundary

| Boundary | Data tidak tepercaya | Validasi yang dilakukan | Failure mode aman |
|---|---|---|---|
| Journal descriptor pada disk | Isi descriptor block hasil pembacaan ulang dari `dev->blocks` | Cek `magic == M16_JMAGIC`, `m16_valid_lba(target_lba)`, dan `payload_checksum` | Recovery dihentikan dan mengembalikan `M16_E_CORRUPT` |
| Header journal | Status dan checksum header | `m16_header_checksum(&h) == h.header_checksum` dan `status == M16_J_COMMITTED` | Jika tidak valid, dianggap journal kosong (`M16_E_OK` tanpa replay) |
| Nama file dari pemanggil | Parameter `name` pada `m16_write_file_ex` | `m16_strlen_bounded(name, M16_MAX_NAME)` dan cek duplikasi via `m16_find_dirent` | `M16_E_INVAL` jika nama terlalu panjang atau sudah ada |
| Source kompilasi (host vs freestanding) | Flag kompilasi pemanggil (`Makefile`) | Build dengan `-Wall -Wextra -Werror -pedantic` pada kedua mode | Build gagal (non-zero exit) jika ada warning/error |

---

## 10. Langkah Kerja Implementasi

Gunakan tabel berikut untuk setiap langkah. Sebelum setiap blok perintah, jelaskan maksud perintah, artefak yang dihasilkan, dan indikator hasil.

### Langkah 1 — `Preflight dan penyiapan struktur direktori M16`

Maksud langkah:

```text
Langkah ini dilakukan untuk membuat branch praktikum baru (praktikum-m16-journal-recovery), menyiapkan struktur direktori kerja (kernel/fs/mcsfs1j, tests/m16, scripts, build/m16, logs/m16, evidence/m16), serta mencatat metadata host dan toolchain sebelum implementasi dimulai.
```

Perintah:

```bash
git checkout -b praktikum-m16-journal-recovery
mkdir -p kernel/fs/mcsfs1j tests/m16 scripts build/m16 logs/m16 evidence/m16
./scripts/m16_preflight.sh | tee logs/m16/preflight.log
```

Output ringkas:

```text
Switched to a new branch 'praktikum-m16-journal-recovery'
== M16 preflight ==
2026-06-13T13:08:40+07:00
== tools ==
Ubuntu clang version 18.1.3 (1ubuntu1)
GNU Make 4.3
QEMU emulator version 8.2.2 (Debian 1:8.2.2+ds-0ubuntu1.16)
== git ==
0d87222
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| preflight.log | logs/m16/ | Mencatat metadata host, toolchain, dan probe subsistem kernel sebelum praktikum |

Indikator berhasil:

```text
Branch praktikum-m16-journal-recovery aktif, seluruh direktori kerja M16 berhasil dibuat, dan preflight.log berisi informasi host/toolchain yang lengkap.
```

### Langkah 2 — `Implementasi modul journal MCSFS1J`

Maksud langkah:

```text
Langkah ini dilakukan untuk menulis source utama write-ahead journal (m16_mcsfs_journal.c) yang berisi struktur superblock, inode, direktori, journal header/descriptor, fungsi format, commit, recovery, fsck, read/write file, beserta host test internal (#ifdef MCSOS_M16_HOST_TEST).
```

Perintah:

```bash
cat > kernel/fs/mcsfs1j/m16_mcsfs_journal.c <<'EOF'
... (isi modul journal, lihat Lampiran C) ...
EOF
wc -l kernel/fs/mcsfs1j/m16_mcsfs_journal.c
clang -fsyntax-only kernel/fs/mcsfs1j/m16_mcsfs_journal.c
```

Output ringkas:

```text
706 kernel/fs/mcsfs1j/m16_mcsfs_journal.c
(setelah perbaikan baris awal yang tercampur, file diverifikasi dengan tail -n +3 dan clang -fsyntax-only tanpa error)
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| m16_mcsfs_journal.c | kernel/fs/mcsfs1j/ | Implementasi inti journal (format, commit, recover, fsck, host test) |
| mcsfs1j_adapter.h | kernel/fs/mcsfs1j/ | Header adapter publik untuk integrasi lanjutan |

Indikator berhasil:

```text
File m16_mcsfs_journal.c berisi 706 baris dan lolos pemeriksaan sintaks (clang -fsyntax-only) tanpa error.
```

### Langkah 3 — `Build dan jalankan host test`

Maksud langkah:

```text
Langkah ini dilakukan untuk mengompilasi modul journal pada mode host (dengan printf, -DMCSOS_M16_HOST_TEST) lalu menjalankan binary uji untuk memvalidasi skenario format, write/read normal, crash mid-transaction beserta recovery, fsck, dan penolakan descriptor journal yang corrupt.
```

Perintah:

```bash
cat > tests/m16/Makefile <<'EOF'
... (lihat Lampiran C) ...
EOF
make -C tests/m16 clean all
./tests/m16/test_m16
```

Output ringkas:

```text
cc -std=c11 -Wall -Wextra -Werror -pedantic -DMCSOS_M16_HOST_TEST ../../kernel/fs/mcsfs1j/m16_mcsfs_journal.c -o test_m16
M16 host tests PASS
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| test_m16 | tests/m16/ | Binary host test journal MCSFS1J |
| nm_undefined.txt / readelf_header.txt / objdump_disasm.txt / sha256sum.txt | tests/m16/ | Evidence audit terhadap object freestanding hasil kompilasi |

Indikator berhasil:

```text
Binary test_m16 berhasil dijalankan dan menampilkan "M16 host tests PASS" tanpa ada fails > 0.
```

### Langkah 4 — `Verifikasi object freestanding (audit nm/readelf/objdump/sha256sum)`

Maksud langkah:

```text
Langkah ini dilakukan untuk memastikan modul journal dapat dikompilasi pada mode freestanding (tanpa libc) sebagaimana dibutuhkan kernel, serta memverifikasi tidak ada simbol undefined dari luar modul, format ELF sesuai target x86_64, dan mencatat checksum object sebagai evidence.
```

Perintah:

```bash
cc -ffreestanding -fno-builtin -std=c11 -Wall -Wextra -Werror -c \
   ../../kernel/fs/mcsfs1j/m16_mcsfs_journal.c -o m16_mcsfs_journal.o
nm -u m16_mcsfs_journal.o
readelf -h m16_mcsfs_journal.o
sha256sum m16_mcsfs_journal.o
```

Output ringkas:

```text
(nm -u kosong, tidak ada undefined symbol)
ELF Header:
  Class:    ELF64
  Type:     REL (Relocatable file)
  Machine:  Advanced Micro Devices X86-64
46391be4dfc0fb8b6f7800d9c81953e51bc9e533a65c034dff008d034546c8bc  m16_mcsfs_journal.o
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| m16_mcsfs_journal.o | tests/m16/ | Object freestanding hasil kompilasi modul journal |
| evidence/m16/nm_undefined.txt, readelf_header.txt, objdump_disasm.txt, sha256sum.txt | evidence/m16/ | Salinan evidence audit object freestanding |

Indikator berhasil:

```text
nm -u tidak menghasilkan symbol undefined, readelf -h menunjukkan ELF64 x86-64 REL, dan sha256sum object tercatat sebagai evidence.
```

### Langkah 5 — `Integrasi ke kernel, build ISO, dan verifikasi QEMU`

Maksud langkah:

```text
Langkah ini dilakukan untuk menambahkan log integrasi M16 pada kernel/block/block_demo.c, membangun ulang seluruh kernel (make clean all) dengan modul m16_mcsfs_journal.o ikut dilink, membangun ISO (make iso), lalu menjalankan QEMU untuk membuktikan modul benar-benar aktif pada runtime kernel melalui serial log.
```

Perintah:

```bash
sed -i '/\[M15\] ready for integration/a\
    log_writeln("[M16] mcsfs1j linked into kernel");\
    log_writeln("[M16] journal recovery support available");' \
kernel/block/block_demo.c
make clean all
make iso
qemu-system-x86_64 -machine q35 -m 512M \
  -serial file:logs/m16/qemu_serial.log \
  -display none -no-reboot -no-shutdown \
  -cdrom build/mcsos.iso
nm build/mcsos-m5.elf | grep m16_
```

Output ringkas:

```text
[M14] block layer initialized
[M14] ram0: 64 blocks x 512 bytes registered
[M15] mcsfs1 linked into kernel
[M15] ready for integration
[M16] mcsfs1j linked into kernel
[M16] journal recovery support available

ffffffff80002fd0 T m16_journal_recover
ffffffff80004920 T m16_fsck
ffffffff800034f0 T m16_format
ffffffff80004670 T m16_read_file
ffffffff80004630 T m16_write_file
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| build/mcsos-m5.elf | build/ | Kernel ELF hasil link, memuat seluruh simbol m16_* |
| build/mcsos.iso | build/ | Image bootable berisi kernel dengan modul M16 |
| logs/m16/qemu_serial.log | logs/m16/ | Bukti runtime bahwa modul M16 ter-link dan aktif |
| evidence/m16/m16_symbols.log | evidence/m16/ | Daftar simbol m16_* pada ELF kernel |

Indikator berhasil:

```text
Kernel berhasil dibangun ulang tanpa error, ISO berhasil dibuat (18M), serial log QEMU menampilkan baris [M16] mcsfs1j linked into kernel dan [M16] journal recovery support available, serta nm pada ELF kernel menunjukkan simbol m16_journal_recover, m16_fsck, m16_format, m16_read_file, dan m16_write_file ter-link.
```

### Langkah 6 — `Commit dan push hasil praktikum`

Maksud langkah:

```text
Langkah ini dilakukan untuk meninjau status repository, menambahkan hanya file-file relevan praktikum M16 ke staging (tanpa file backup/bantu), membuat commit, lalu mendorong (push) branch praktikum-m16-journal-recovery ke remote GitHub.
```

Perintah:

```bash
git add kernel/fs/mcsfs1j kernel/block/block_demo.c scripts/m16_preflight.sh tests/m16 evidence/m16 logs/m16
git restore --staged tests/m16/Makefile.hosttest.bak tests/m16/Makefile.sebelum_panduan tests/m16/test_m16 tests/m16/evidence/m16/test_m16.sha256
git commit -m "M16 journal recovery implementation and verification"
git push -u origin praktikum-m16-journal-recovery
```

Output ringkas:

```text
[praktikum-m16-journal-recovery 274e01f] M16 journal recovery implementation and verification
 14 files changed, 4510 insertions(+)
...
 * [new branch]      praktikum-m16-journal-recovery -> praktikum-m16-journal-recovery
branch 'praktikum-m16-journal-recovery' set up to track 'origin/praktikum-m16-journal-recovery'.
```

Artefak yang dihasilkan:

| Artefak | Lokasi | Fungsi |
|---|---|---|
| Commit 274e01f | repository lokal & remote | Snapshot resmi hasil praktikum M16 |
| Branch praktikum-m16-journal-recovery | GitHub | Backup dan bukti praktikum M16 |

Indikator berhasil:

```text
Commit 274e01f berhasil dibuat dengan 14 file berubah, dan branch praktikum-m16-journal-recovery berhasil di-push serta dapat diakses di GitHub.
```

## 11. Checkpoint Buildable

Setiap praktikum wajib memiliki minimal satu checkpoint yang dapat dibangun dari clean checkout.

| Checkpoint | Perintah | Expected result | Status |
|---|---|---|---|
| Host test journal | `` make -C tests/m16 clean all `` | `test_m16` berjalan dan mencetak `M16 host tests PASS` | `PASS` |
| Audit object freestanding | `` make -C tests/m16 audit `` (nm/readelf/objdump/sha256sum) | `nm_undefined.txt` kosong, `readelf` ELF64 x86-64 | `PASS` |
| Build kernel penuh | `` make clean all `` | `build/mcsos-m5.elf` terbentuk, simbol `m16_*` ter-link | `PASS` |
| Build ISO | `` make iso `` | `build/mcsos.iso` terbentuk (~18M) | `PASS` |
| QEMU smoke test | `` qemu-system-x86_64 -cdrom build/mcsos.iso ... `` | Serial log memuat `[M16] mcsfs1j linked into kernel` | `PASS` |

Catatan checkpoint:

```text
Seluruh checkpoint utama pada M16 berhasil dijalankan dan lulus: host test journal (test_m16) lulus dengan pesan "M16 host tests PASS", audit object freestanding tidak menunjukkan undefined symbol dan format ELF sesuai target x86_64, build kernel penuh (make clean all) berhasil tanpa error dan menyertakan seluruh simbol m16_* pada ELF kernel, build ISO berhasil menghasilkan image sebesar 18M, dan QEMU smoke test membuktikan modul M16 benar-benar aktif pada runtime melalui serial log.

Catatan: integrasi MCSFS1J sebagai filesystem default pada VFS (mount point nyata, dipanggil dari syscall open/read/write) belum dilakukan pada M16; modul baru dilink ke kernel dan dibuktikan keberadaannya melalui simbol dan log inisialisasi, bukan melalui alur syscall pengguna.
```

---

## 12. Perintah Uji dan Validasi

### 12.1 Build Test

Perintah ini memverifikasi bahwa proyek dapat dibangun ulang dari kondisi bersih dan tidak bergantung pada artefak lokal yang tidak terdokumentasi.

```bash
make clean
make all
```

Hasil:

```text
rm -rf build
... (kompilasi seluruh kernel termasuk kernel/fs/mcsfs1j/m16_mcsfs_journal.c) ...
ld.lld -nostdlib -static -z max-page-size=0x1000 -T linker.ld -Map=build/mcsos-m5.map -o build/mcsos-m5.elf ...
grep -q 'ELF64' build/readelf-header.txt
grep -q 'Machine:[[:space:]]*Advanced Micro Devices X86-64' build/readelf-header.txt
grep -q 'kmain' build/symbols.txt
grep -q 'kernel_panic_at' build/symbols.txt
grep -q 'cpu_halt_forever' build/disassembly.txt
```

Status: `PASS`

### 12.2 Static Inspection

Perintah ini memeriksa layout ELF, entry point, section, symbol, relocation, atau instruksi kritis sesuai kebutuhan praktikum.

```bash
readelf -h tests/m16/m16_mcsfs_journal.o
nm -u tests/m16/m16_mcsfs_journal.o
nm build/mcsos-m5.elf | grep m16_
```

Hasil penting:

```text
ELF Header:
  Class:    ELF64
  Data:     2's complement, little endian
  Type:     REL (Relocatable file)
  Machine:  Advanced Micro Devices X86-64

(nm -u kosong: tidak ada undefined symbol)

ffffffff80002f20 T m16_dev_init
ffffffff800034f0 T m16_format
ffffffff80004920 T m16_fsck
ffffffff80002fd0 T m16_journal_recover
ffffffff80003850 T m16_mount
ffffffff80004670 T m16_read_file
ffffffff80004630 T m16_write_file
ffffffff80003950 T m16_write_file_ex
```

Status: `PASS`

### 12.3 QEMU Smoke Test

Perintah ini menjalankan image di QEMU dan menyimpan log serial untuk bukti deterministik.

```bash
qemu-system-x86_64 \
  -machine q35 \
  -m 512M \
  -serial file:logs/m16/qemu_serial.log \
  -display none \
  -no-reboot \
  -no-shutdown \
  -cdrom build/mcsos.iso
```

Hasil:

```text
[M13] VFS/RAMFS kernel selftest: PASS
[M14] block layer initialized
[M14] ram0: 64 blocks x 512 bytes registered
[M15] mcsfs1 linked into kernel
[M15] ready for integration
[M16] mcsfs1j linked into kernel
[M16] journal recovery support available
```

Status: `PASS`

### 12.4 GDB Debug Evidence

Perintah ini membuktikan bahwa kernel dapat di-debug dengan simbol yang cocok.

```bash
qemu-system-x86_64 \
  -machine q35 \
  -m 512M \
  -serial stdio \
  -display none \
  -no-reboot \
  -no-shutdown \
  -s -S \
  -cdrom build/mcsos.iso
```

Hasil:

```text
Tidak dilakukan pada tahap M16 karena fokus praktikum adalah pembuktian commit/replay journal melalui host test dan serial log, bukan debugging interaktif. Simbol m16_* sudah terverifikasi tersedia pada build/mcsos-m5.elf melalui nm sehingga GDB dapat digunakan pada milestone berikutnya jika diperlukan.
```

Status: `NA`

### 12.5 Unit Test

```bash
make -C tests/m16 clean all
./tests/m16/test_m16
```

Hasil:

```text
cc -std=c11 -Wall -Wextra -Werror -pedantic -DMCSOS_M16_HOST_TEST ../../kernel/fs/mcsfs1j/m16_mcsfs_journal.c -o test_m16
M16 host tests PASS
```

Status: `PASS`

### 12.6 Stress/Fuzz/Fault Injection Test

Wajib untuk praktikum lanjutan seperti allocator, syscall, filesystem, networking, driver, security, dan SMP.

```bash
./tests/m16/test_m16
```

Hasil:

```text
Pengujian fault injection yang dilakukan pada M16 bersifat terbatas dan manual, yaitu dua skenario di dalam host test:
1. Simulasi crash mid-transaction: m16_write_file_ex dipanggil dengan stop_after_commit_record=1, lalu m16_journal_recover dipanggil untuk membuktikan replay berhasil dan data ("crash.txt") tetap terbaca benar setelah "crash".
2. Simulasi descriptor journal corrupt: setelah commit, satu byte pada descriptor journal di-XOR (dev.blocks[M16_JOURNAL_START + 1u][0] ^= 0x7fu), lalu m16_journal_recover dipanggil dan diharapkan mengembalikan M16_E_CORRUPT.
Kedua skenario tersebut lulus sebagai bagian dari "M16 host tests PASS". Fuzzing otomatis berskala besar belum dilakukan pada M16.
```

Status: `PASS (terbatas/manual)`

### 12.7 Visual Evidence

Praktikum M16 tidak menghasilkan output framebuffer atau antarmuka grafis. Oleh karena itu, visual evidence pada praktikum ini berupa log terminal, log serial QEMU, dan artefak hasil verifikasi.

| No. | Artefak | Lokasi File | Keterangan |
|---:|---|---|---|
| 1 | Preflight Log | `logs/m16/preflight.log` | Log pemeriksaan lingkungan, toolchain, dan status repository sebelum pengujian |
| 2 | Host Test Log | `logs/m16/m16_make_all.log` | Hasil kompilasi dan host test dengan status **M16 host tests PASS** |
| 3 | QEMU Serial Log | `logs/m16/qemu_serial.log` | Bukti integrasi kernel dan munculnya log M16 saat proses boot |
| 4 | Undefined Symbol Audit | `evidence/m16/nm_undefined.txt` | Hasil audit menunjukkan tidak terdapat undefined symbol |
| 5 | ELF Header | `evidence/m16/readelf_header.txt` | Verifikasi object freestanding bertipe ELF64 (REL, x86-64) |
| 6 | Disassembly | `evidence/m16/objdump_disasm.txt` | Hasil disassembly object M16 |
| 7 | SHA-256 Checksum | `evidence/m16/sha256sum.txt` | Fingerprint artefak object hasil kompilasi |



## 13. Hasil Uji

### 13.1 Tabel Ringkasan Hasil

| No. | Uji | Expected result | Actual result | Status | Evidence |
|---|---|---|---|---|---|
| 1 | Host test journal (`./tests/m16/test_m16`) | Semua assert lulus, mencetak `M16 host tests PASS` | `M16 host tests PASS` | PASS | evidence/m16/host_test.log |
| 2 | Audit object freestanding (`nm -u`) | Tidak ada undefined symbol | `nm_undefined.txt` kosong (0 byte) | PASS | evidence/m16/nm_undefined.txt |
| 3 | Static inspection (`readelf -h`) | ELF64, x86-64, REL | Class: ELF64, Machine: Advanced Micro Devices X86-64, Type: REL | PASS | evidence/m16/readelf_header.txt |
| 4 | Build kernel penuh (`make clean all`) | Build sukses, symbol kmain/panic ada | Build selesai, `grep -q 'kmain'`, dll. lulus | PASS | logs/m16/kernel_build.log |
| 5 | Symbol m16_* pada kernel ELF | Symbol m16_format, m16_journal_recover, m16_fsck, dll. ditemukan | Seluruh symbol m16_* ditemukan via `nm` | PASS | evidence/m16/m16_symbols.log |
| 6 | QEMU smoke test | Serial log memuat baris [M16] | `[M16] mcsfs1j linked into kernel`, `[M16] journal recovery support available` | PASS | evidence/m16/qemu_serial.log |

### 13.2 Log Penting

```text
./tests/m16/test_m16
M16 host tests PASS

nm -u m16_mcsfs_journal.o
(kosong)

readelf -h m16_mcsfs_journal.o
  Class:                             ELF64
  Type:                              REL (Relocatable file)
  Machine:                           Advanced Micro Devices X86-64

nm build/mcsos-m5.elf | grep m16_journal_recover
ffffffff80002fd0 T m16_journal_recover

(serial log QEMU)
[M16] mcsfs1j linked into kernel
[M16] journal recovery support available
```

### 13.3 Artefak Bukti

| Artefak | Path | SHA-256 / hash | Fungsi |
|---|---|---|---|
| m16_mcsfs_journal.o | tests/m16/m16_mcsfs_journal.o | 46391be4dfc0fb8b6f7800d9c81953e51bc9e533a65c034dff008d034546c8bc | Object freestanding hasil kompilasi modul journal |
| test_m16 | tests/m16/test_m16 | 859da1fe079c475834c487add2d6276a955dfe26c668396afa9e5fdf3ab9a09b | Binary host test journal MCSFS1J |
| build/mcsos.iso | build/mcsos.iso | (tercatat pada build/mcsos.iso.sha256) | Image bootable kernel dengan modul M16 |

Perintah hash:

```bash
sha256sum tests/m16/m16_mcsfs_journal.o
sha256sum tests/m16/test_m16
sha256sum build/mcsos.iso
```

---

## 14. Analisis Teknis

### 14.1 Analisis Keberhasilan

```text
Hasil uji pada tahap M16 menunjukkan bahwa mekanisme write-ahead journal MCSFS1J berhasil diimplementasikan dan diverifikasi pada dua mode kompilasi sekaligus. Pada mode host, seluruh skenario unit test (format, write/read normal, fsck, simulasi crash mid-transaction beserta recovery, dan penolakan descriptor corrupt) lulus dan mencetak "M16 host tests PASS".

Pada mode freestanding, modul berhasil dikompilasi tanpa undefined symbol (dibuktikan oleh nm -u yang kosong) dan menghasilkan object ELF64 dengan arsitektur x86_64 sesuai target kernel. Modul tersebut kemudian berhasil dilink ke dalam kernel MCSOS (build/mcsos-m5.elf), dibuktikan oleh keberadaan seluruh simbol m16_* (termasuk m16_format, m16_journal_recover, m16_fsck, m16_read_file, m16_write_file) pada hasil nm.

Bukti runtime juga diperoleh melalui QEMU: log serial menunjukkan baris "[M16] mcsfs1j linked into kernel" dan "[M16] journal recovery support available" yang ditambahkan pada kernel/block/block_demo.c, membuktikan modul tersebut benar-benar dieksekusi sebagai bagian dari proses inisialisasi kernel saat boot.
```

### 14.2 Analisis Kegagalan atau Perbedaan Hasil

```text
Pada tahap M16, tidak ditemukan kegagalan pada hasil akhir (final result) dari host test, build kernel, build ISO, maupun QEMU smoke test. Namun, terdapat beberapa kendala proses (bukan kegagalan fungsional) yang sempat terjadi selama pengerjaan:

1. Heredoc yang sempat tertimpa/terputus (Ctrl+C pada penulisan m16_mcsfs_journal.c pertama) menyebabkan file kosong dan harus dihapus lalu ditulis ulang.
2. Baris-baris awal hasil heredoc kedua tercampur dengan perintah shell sebelumnya (mkdir -p ... cat > ... <<'EOF') sehingga file memiliki 2 baris "sampah" di awal, diperbaiki dengan tail -n +3.
3. File tests/m16/Makefile sempat menjadi 0 byte karena penulisan ulang Makefile secara langsung tanpa heredoc yang benar (setiap baris dieksekusi sebagai perintah shell terpisah, menimbulkan banyak pesan "command not found"). Masalah ini diperbaiki dengan mengembalikan isi Makefile dari salinan backup (Makefile.sebelum_panduan).
4. File mcsfs1j_adapter.h sempat dibuat pada path yang salah (tests/m16/kernel/fs/mcsfs1j/) karena working directory yang tidak sesuai saat menjalankan heredoc, sehingga harus dipindahkan (mv) ke lokasi kernel/fs/mcsfs1j/ yang benar.

Tidak ada perbedaan hasil pada level fungsional journal itu sendiri: setelah seluruh perbaikan proses tersebut, host test tetap lulus seluruhnya ("M16 host tests PASS") dan integrasi ke kernel tetap berhasil.

Tindakan perbaikan yang direncanakan adalah lebih berhati-hati saat menggunakan heredoc multi-baris di shell interaktif (memastikan delimiter EOF benar-benar berada di baris terpisah dan working directory sudah benar sebelum menjalankan perintah penulisan file).
```

### 14.3 Perbandingan dengan Teori

| Konsep teori | Implementasi praktikum | Sesuai/tidak sesuai | Penjelasan |
|---|---|---|---|
| Write-ahead logging (WAL) | Data ditulis ke journal (descriptor+payload) sebelum diterapkan ke target_lba, ditandai dengan commit header | Sesuai | Mengikuti prinsip dasar WAL: log dulu, baru apply, sebagaimana dijelaskan pada *Three Easy Pieces* bab Crash Consistency |
| Atomicity melalui commit record | Header journal dengan checksum menandai transaksi sebagai "committed" sebelum replay dapat dipercaya | Sesuai | Crash sebelum commit header ditulis dianggap tidak pernah terjadi (journal kosong) |
| Fail-closed terhadap data corrupt | `m16_journal_recover` mengembalikan `M16_E_CORRUPT` saat checksum/magic descriptor tidak valid | Sesuai | Menghindari penerapan data tidak terpercaya ke lokasi yang salah, sejalan dengan prinsip keamanan filesystem |
| Cross/dual-target compilation | Source tunggal dikompilasi sebagai host test (`-DMCSOS_M16_HOST_TEST`) dan sebagai freestanding object untuk kernel | Sesuai | Menjamin logika journal yang diuji pada host identik dengan yang dilink ke kernel |

### 14.4 Kompleksitas dan Kinerja

| Aspek | Estimasi/hasil | Bukti | Catatan |
|---|---|---|---|
| Kompleksitas commit | O(N) terhadap jumlah record transaksi (N ≤ M16_JOURNAL_MAX_RECORDS = 8) | `m16_journal_commit` melakukan loop sebanyak `tx->count` | Linear terhadap jumlah block yang diubah dalam satu transaksi |
| Kompleksitas recovery | O(M16_JOURNAL_MAX_RECORDS) | `m16_journal_recover` melakukan loop tetap sejumlah slot descriptor maksimum | Tidak bergantung pada ukuran filesystem (128 block) |
| Waktu host test | < 1 detik | Output `./tests/m16/test_m16` langsung mencetak `M16 host tests PASS` | Device simulasi hanya berukuran kecil (`M16_MAX_BLOCKS = 128`) |
| Ukuran object freestanding | 11K (`m16_mcsfs_journal.o`) | `ls -lh tests/m16/m16_mcsfs_journal.o` | Modul tergolong ringan untuk kernel pendidikan |
| Ukuran ISO akhir | 18M (`build/mcsos.iso`) | `ls -lh build/mcsos.iso` | Konsisten dengan ukuran ISO milestone sebelumnya |

---

## 15. Debugging dan Failure Modes

### 15.1 Failure Modes yang Ditemukan

| Failure mode | Gejala | Penyebab sementara | Bukti | Perbaikan |
|---|---|---|---|---|
| File source journal kosong (0 byte) | `cat > m16_mcsfs_journal.c <<'EOF'` terhenti oleh Ctrl+C | Heredoc dibatalkan sebelum delimiter EOF tercapai | `ls -l kernel/fs/mcsfs1j` menunjukkan file 0 byte | File dihapus (`rm`) lalu ditulis ulang dari awal dengan heredoc lengkap |
| Baris awal file tercampur perintah shell | `head -5` menunjukkan baris `mkdir -p ...` dan `cat > ... <<'EOF'` ikut masuk sebagai isi file | Heredoc kedua dijalankan menyatu dengan perintah sebelumnya tanpa newline pemisah yang bersih | `head -20 kernel/fs/mcsfs1j/m16_mcsfs_journal.c` | `tail -n +3 file.c > /tmp/m16_fixed.c && mv /tmp/m16_fixed.c file.c` |
| `tests/m16/Makefile` menjadi 0 byte | Banyak pesan `command not found` (`CLANG: command not found`, dll.) saat mencoba menjalankan ulang Makefile | Penulisan Makefile tanpa heredoc yang benar sehingga tiap baris dieksekusi langsung oleh shell | Output error bertumpuk saat `cat > tests/m16/Makefile` tanpa `<<'EOF'` | Mengembalikan isi dari backup `cp tests/m16/Makefile.sebelum_panduan tests/m16/Makefile` |
| Adapter header dibuat di path salah | `mcsfs1j_adapter.h` muncul di `tests/m16/kernel/fs/mcsfs1j/`, bukan `kernel/fs/mcsfs1j/` | Working directory saat heredoc tidak sesuai ekspektasi (berada di `tests/m16`) | `ls -lh ~/src/mcsos/tests/m16/kernel/fs/mcsfs1j` | `mv` file ke lokasi benar, lalu `rm -rf` direktori salah |

### 15.2 Failure Modes yang Diantisipasi

| Failure mode | Deteksi | Dampak | Mitigasi |
|---|---|---|---|
| Descriptor journal corrupt saat recovery | `m16_journal_recover` mengembalikan `M16_E_CORRUPT` | Recovery gagal total, data committed tidak ter-replay | Fail-closed by design; perlu fsck manual atau format ulang |
| Journal penuh (jumlah record melebihi `M16_JOURNAL_MAX_RECORDS`) | `m16_tx_add` mengembalikan error saat `tx->count >= M16_JOURNAL_MAX_RECORDS` | Transaksi tulis gagal sebelum commit | Pemanggil harus memecah transaksi besar menjadi beberapa transaksi kecil |
| Crash tepat saat commit header sedang ditulis (partial write) | Checksum header tidak valid saat recovery | Journal dianggap kosong, transaksi dianggap tidak pernah terjadi | Sesuai desain WAL: data asli tidak berubah karena belum ada replay |
| Build freestanding gagal karena fungsi libc tidak tersedia | `nm -u` menunjukkan undefined symbol | Linker kernel akan gagal (`undefined reference`) | Modul hanya menggunakan fungsi internal (`m16_zero`, `m16_copy`, dll.), tidak memanggil libc |

### 15.3 Triage yang Dilakukan

```text
Urutan diagnosis yang dilakukan pada tahap M16 dimulai dari pemeriksaan ukuran dan isi file menggunakan ls -lh, wc -l, head, dan tail setiap kali ada kecurigaan file rusak atau tertimpa.

Untuk masalah file source yang tercampur perintah shell, dilakukan pemeriksaan baris awal (head -20) dan baris akhir (tail -20) file, lalu diperbaiki dengan memotong baris yang tidak relevan menggunakan tail -n +3.

Untuk masalah Makefile yang menjadi 0 byte, dilakukan pengecekan dengan cat -A pada versi Makefile yang benar (untuk memastikan penggunaan tab, bukan spasi, pada resep make) sebelum Makefile dikembalikan dari backup.

Validasi akhir dilakukan dengan menjalankan clang -fsyntax-only pada source, lalu make -C tests/m16 clean all untuk memastikan baik target host maupun freestanding berhasil dibangun, dan terakhir menjalankan ./tests/m16/test_m16 untuk memverifikasi output "M16 host tests PASS".

Setelah modul terverifikasi pada level host, triage dilanjutkan pada level kernel: make clean all untuk memastikan tidak ada error kompilasi/link, lalu nm build/mcsos-m5.elf | grep m16_ untuk memastikan simbol modul ter-link, dan terakhir qemu-system-x86_64 dengan -serial file:logs/m16/qemu_serial.log untuk memverifikasi log runtime [M16] muncul.
```

### 15.4 Panic Path

Jika terjadi panic, tempel output panic.

```text
Tidak terjadi kernel panic pada tahap M16. Trap/interrupt yang muncul pada log QEMU (mis. "[M4] trap dispatch: external-or-user-defined-interrupt") merupakan bagian dari mekanisme timer/IRQ milestone sebelumnya (M4/M5) yang memang sudah ditangani oleh trap handler, bukan kegagalan modul journal M16.

Pengujian panic path khusus untuk modul journal (misalnya panic saat journal benar-benar corrupt di kernel, bukan di host test) belum dilakukan pada M16 karena modul belum dipanggil dari jalur syscall VFS; pengujian tersebut direncanakan pada milestone integrasi VFS berikutnya.
```

---

## 16. Prosedur Rollback

Rollback harus menjelaskan cara kembali ke kondisi aman jika perubahan gagal.

| Skenario rollback | Perintah | Data yang harus diselamatkan | Status |
|---|---|---|---|
| Kembali ke commit sebelum M16 | `git checkout 0d87222` | Log dan perubahan laporan M16 | belum diuji |
| Revert commit praktikum M16 | `git revert 274e01f` | Log commit dan perubahan file M16 | belum diuji |
| Bersihkan artefak build kernel | `make clean` | Tidak ada, source aman | teruji |
| Bersihkan artefak host test M16 | `make -C tests/m16 clean` | Tidak ada, source `m16_mcsfs_journal.c` aman | teruji |
| Hapus journal yang corrupt dan format ulang | `m16_dev_init(&dev); m16_format(&dev);` | Data lama pada device hilang (memang disengaja sebagai reset) | teruji (host test) |

Catatan rollback:

```text
Pada tahap M16, prosedur rollback yang benar-benar diuji adalah make clean (membersihkan artefak build kernel) dan make -C tests/m16 clean (membersihkan artefak host test, termasuk m16_mcsfs_journal.o dan test_m16). Selain itu, prosedur "rollback" pada level data journal itu sendiri turut diuji secara fungsional melalui m16_format yang mengembalikan device ke kondisi bersih.

Sementara itu, perintah git checkout dan git revert belum diuji secara langsung pada branch praktikum-m16-journal-recovery, namun secara teoritis dapat digunakan untuk mengembalikan repository ke kondisi sebelum modul M16 ditambahkan.

Risiko dari rollback yang belum diuji adalah kemungkinan konflik saat revert jika ada file evidence yang sudah berubah, atau kehilangan log/test_m16 hasil build lokal yang belum di-commit.
```

---

## 17. Keamanan dan Reliability

### 17.1 Risiko Keamanan

| Risiko | Boundary | Dampak | Mitigasi | Evidence |
|---|---|---|---|---|
| Replay data ke target_lba yang salah akibat descriptor corrupt | Journal recovery boundary | Data filesystem lain berpotensi tertimpa secara tidak sengaja | Validasi `magic`, `m16_valid_lba`, dan `payload_checksum` sebelum replay; fail-closed dengan `M16_E_CORRUPT` | Test `corrupt descriptor rejected` lulus |
| Nama file terlalu panjang/tidak ter-null-terminate | Input `name` pada `m16_write_file_ex` | Potensi overflow saat menyalin ke `dir[slot].name` | `m16_strlen_bounded(name, M16_MAX_NAME)` membatasi panjang yang disalin | Review kode dan kompilasi `-Werror` tanpa warning |
| Toolchain/host test tidak valid | Build environment | Hasil verifikasi tidak dapat dipercaya | Menjalankan `scripts/m16_preflight.sh` untuk mencatat versi toolchain sebelum pengujian | logs/m16/preflight.log |
| File kerja tertimpa/rusak akibat heredoc shell | Proses penulisan file lokal | Source code journal atau Makefile rusak | Verifikasi `wc -l`, `head`, `tail`, dan `clang -fsyntax-only` sebelum melanjutkan | Lihat bagian 15.1 |

### 17.2 Reliability dan Data Integrity

| Risiko reliability | Dampak | Deteksi | Mitigasi |
|---|---|---|---|
| Crash mid-transaction sebelum commit header selesai | Filesystem tetap pada state lama (data baru hilang) | `m16_journal_recover` membaca header dan menemukan checksum tidak valid/status bukan committed | Dianggap aman: data lama tetap konsisten, sesuai prinsip WAL |
| Crash mid-transaction setelah commit header selesai | Data target_lba mungkin belum ter-update | `m16_journal_recover` mendeteksi header valid dan status committed | Replay seluruh record ke target_lba masing-masing |
| Journal descriptor corrupt setelah commit (mis. bit-flip) | Recovery tidak dapat dipercaya | Checksum payload/descriptor tidak cocok | Recovery dihentikan (`M16_E_CORRUPT`), tidak ada partial replay |
| Build freestanding tidak sinkron dengan host test | Modul yang dilink ke kernel berbeda perilaku dari yang sudah diuji | Source tunggal (`m16_mcsfs_journal.c`) dengan `#ifdef` untuk kedua mode | Logika inti identik di kedua mode kompilasi |

### 17.3 Negative Test

| Negative test | Input buruk | Expected result | Actual result | Status |
|---|---|---|---|---|
| Crash mid-transaction (stop after commit record) | `m16_write_file_ex(..., stop_after_commit_record=1)` lalu data asli belum diterapkan | Recovery (`m16_journal_recover`) berhasil me-replay dan file tetap terbaca benar | `journal replay after committed crash`, `read crash after replay`, `fsck after replay` lulus | PASS |
| Descriptor journal corrupt | `dev.blocks[M16_JOURNAL_START + 1u][0] ^= 0x7fu` setelah commit | `m16_journal_recover` mengembalikan `M16_E_CORRUPT` | `corrupt descriptor rejected` lulus dengan kode `M16_E_CORRUPT` | PASS |
| Object freestanding dengan symbol libc tak terduga | Kompilasi `-ffreestanding -fno-builtin` modul journal | `nm -u` tidak menunjukkan symbol undefined dari libc | `nm_undefined.txt` kosong (0 byte) | PASS |
| QEMU dijalankan tanpa ISO terbaru | `qemu-system-x86_64 -cdrom build/mcsos.iso` sebelum `make iso` dijalankan ulang | QEMU gagal membuka file (`No such file or directory`) | Error sesuai dugaan saat `build/mcsos.iso` belum dibuat ulang setelah `make clean` | PASS (perilaku sesuai ekspektasi) |

---

## 18. Pembagian Kerja Kelompok

Isi bagian ini hanya jika praktikum dikerjakan berkelompok. Untuk pengerjaan individu, tulis “Tidak berlaku”.

| Nama | NIM | Peran | Kontribusi teknis | Commit/artefak |
|---|---|---|---|---|
| `Neng Nagita Salma` | `25832071004` | `Anggota kelompok` | `Kerja bersama` | `Commit 274e01f dan artifact praktikum M16` |
| `Anisa Nur Azfa` | `25832072003` | `Anggota kelompok` | `Kerja bersama` | `Commit 274e01f dan artifact praktikum M16` |
| `Lailatul Zulfa` | `25832072001` | `Anggota kelompok` | `Kerja bersama` | `Commit 274e01f dan artifact praktikum M16` |
### 18.1 Mekanisme Koordinasi

```text
Koordinasi dalam kelompok dilakukan secara kolaboratif dengan pendekatan kerja bersama pada setiap tahap praktikum M16.

Pengelolaan kode dilakukan menggunakan satu branch praktikum (praktikum-m16-journal-recovery) tanpa pemisahan branch individu, sesuai kesepakatan kelompok dan arahan dosen.

Setiap perubahan, mulai dari penulisan modul journal, perbaikan Makefile, hingga integrasi ke kernel dan QEMU, dilakukan secara bergantian dan langsung di-commit ke repository, kemudian diverifikasi bersama melalui host test (./tests/m16/test_m16), build kernel (make clean all), dan log QEMU.

Diskusi dilakukan secara langsung dan melalui komunikasi online untuk memastikan setiap anggota memahami konsep write-ahead journal, commit record, dan mekanisme recovery, termasuk saat menelusuri penyebab file Makefile yang sempat kosong dan file adapter yang sempat berada di path yang salah.

Tidak terdapat konflik merge yang signifikan karena seluruh anggota bekerja secara terkoordinasi dan tidak melakukan perubahan secara bersamaan pada bagian yang sama.
```

### 18.2 Evaluasi Kontribusi

| Anggota | Persentase kontribusi yang disepakati | Bukti | Catatan |
|---|---:|---|---|
| Neng Nagita Salma |                                   33% | commit Git, log terminal host test/build kernel, evidence readelf/objdump/nm, dan dokumentasi praktikum | Berkontribusi bersama dalam seluruh tahap praktikum M16 |
| Anisa Nur Azfa    |                                   33% | commit Git, log terminal host test/build kernel, evidence readelf/objdump/nm, dan dokumentasi praktikum | Berkontribusi bersama dalam seluruh tahap praktikum M16 |
| Lailatul Zulfa    |                                   34% | commit Git, log terminal host test/build kernel, evidence readelf/objdump/nm, dan dokumentasi praktikum | Berkontribusi bersama dalam seluruh tahap praktikum M16 |


Note: Pembagian kontribusi dilakukan secara merata, perbedaan 1% hanya untuk memenuhi total 100%.

## 19. Kriteria Lulus Praktikum

Bagian ini wajib diisi. Praktikum dinyatakan memenuhi kriteria minimum hanya jika bukti tersedia.

| Kriteria minimum | Status | Evidence |
|---|---|---|
| Proyek dapat dibangun dari clean checkout | PASS | logs/m16/kernel_build.log (`make clean all`) |
| Perintah build terdokumentasi | PASS | Bagian 12.1 Build Test |
| QEMU boot atau test target berjalan deterministik | PASS | evidence/m16/qemu_serial.log |
| Semua unit test/praktikum test relevan lulus | PASS | `M16 host tests PASS` (evidence/m16/host_test.log) |
| Log serial disimpan | PASS | logs/m16/qemu_serial.log, evidence/m16/qemu_serial.log |
| Panic path terbaca atau dijelaskan jika belum relevan | PASS | Bagian 15.4 Panic Path |
| Tidak ada warning kritis pada build | PASS | `-Wall -Wextra -Werror` lulus tanpa error pada host & freestanding |
| Perubahan Git terkomit | PASS | git log (commit 274e01f) |
| Desain dan failure mode dijelaskan | PASS | Bagian 9 dan 15 |
| Laporan berisi screenshot/log yang cukup | PASS | Log terminal lengkap pada Bagian 10, 12, 13, 23 |

Kriteria tambahan untuk praktikum lanjutan:

| Kriteria lanjutan | Status | Evidence |
|---|---|---|
| Static analysis dijalankan | PASS | `clang -fsyntax-only`, `-Wall -Wextra -Werror -pedantic` |
| Stress test dijalankan | NA | Tidak dilakukan stress test berskala besar pada M16 |
| Fuzzing atau malformed-input test dijalankan | PASS (terbatas) | Skenario corrupt descriptor pada Bagian 12.6 |
| Fault injection dijalankan | PASS | Skenario crash mid-transaction dan descriptor corrupt |
| Disassembly/readelf evidence tersedia | PASS | evidence/m16/objdump_disasm.txt, readelf_header.txt |
| Review keamanan dilakukan | PASS | Bagian 17 |
| Rollback diuji | PASS | `make clean`, `make -C tests/m16 clean`, `m16_format` |


## 20. Readiness Review

Pilih satu status dengan alasan berbasis bukti.

| Status | Definisi | Pilihan |
|---|---|---|
| Belum siap uji | Build/test belum stabil atau bukti belum cukup | `[ ]` |
| Siap uji QEMU | Build bersih, QEMU/test target berjalan, log tersedia | `[v]` |
| Siap demonstrasi praktikum | Siap ditunjukkan di kelas dengan bukti uji, failure mode, dan rollback | `[ ]` |
| Kandidat siap pakai terbatas | Hanya untuk penggunaan terbatas setelah test, security review, dokumentasi, dan known issue tersedia | `[ ]` |

Alasan readiness:

```text
Berdasarkan hasil praktikum M16, modul MCSFS1J berhasil lulus host test secara menyeluruh (termasuk skenario crash dan descriptor corrupt), berhasil dikompilasi sebagai object freestanding tanpa undefined symbol, berhasil dilink ke dalam kernel MCSOS, serta terbukti aktif pada runtime melalui log serial QEMU.

Namun, modul belum diintegrasikan penuh ke VFS (belum dapat diakses melalui syscall open/read/write pengguna) dan belum diuji dengan stress/fuzz test berskala besar maupun pada banyak skenario corrupt sekaligus. Oleh karena itu, status yang paling sesuai adalah siap uji QEMU, bukan siap demonstrasi penuh atau kandidat siap pakai.
```

Known issues:

| No. | Issue | Dampak | Workaround | Target perbaikan |
|---|---|---|---|---|
| 1 | MCSFS1J belum terhubung ke VFS/syscall pengguna | Tidak dapat diakses melalui open/read/write biasa | Diuji langsung melalui fungsi `m16_*` pada host test dan kernel init | Milestone integrasi VFS berikutnya |
| 2 | Kapasitas journal terbatas (`M16_JOURNAL_MAX_RECORDS = 8`) | Transaksi besar harus dipecah | Memecah transaksi tulis menjadi beberapa commit kecil | Penyesuaian kapasitas pada milestone lanjutan |
| 3 | Belum ada stress/fuzz test berskala besar terhadap journal | Cakupan pengujian korupsi masih terbatas pada satu skenario | Pengujian manual dengan satu byte di-corrupt | Penambahan fuzz test otomatis di milestone berikutnya |

Keputusan akhir:

```text
Berdasarkan bukti host test (M16 host tests PASS), audit object freestanding (nm -u kosong, ELF64 x86-64), keberhasilan link ke kernel (simbol m16_* pada build/mcsos-m5.elf), serta bukti runtime pada QEMU (log [M16] mcsfs1j linked into kernel), praktikum M16 dinyatakan siap uji QEMU. Integrasi penuh ke VFS dan pengujian fault injection berskala lebih besar akan dilanjutkan pada milestone berikutnya.
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
Pada praktikum M16, implementasi write-ahead journal MCSFS1J berhasil dilakukan dengan baik. Hal ini dibuktikan dengan keberhasilan host test (./tests/m16/test_m16) yang mencetak "M16 host tests PASS" untuk seluruh skenario, termasuk format, write/read normal, fsck, simulasi crash mid-transaction beserta recovery-nya, dan penolakan descriptor journal yang corrupt.

Selain itu, modul berhasil dikompilasi sebagai object freestanding tanpa undefined symbol, dilink ke dalam kernel MCSOS, dan terbukti aktif saat runtime melalui log serial QEMU yang menampilkan "[M16] mcsfs1j linked into kernel" dan "[M16] journal recovery support available".

Hasil ini menunjukkan bahwa mekanisme dasar crash-consistency (commit record, replay, dan fail-closed terhadap korupsi) telah berhasil dibuktikan baik secara unit test maupun secara integrasi kernel.
```

### 22.2 Yang Belum Berhasil

```text
Pada tahap M16, modul MCSFS1J belum diintegrasikan ke dalam VFS sehingga belum dapat diakses melalui syscall open/read/write seperti filesystem pengguna pada umumnya. Modul baru dibuktikan melalui pemanggilan langsung fungsi m16_* pada host test serta keberadaan simbolnya pada kernel ELF.

Selain itu, pengujian fault injection masih terbatas pada dua skenario manual (crash mid-transaction dan satu descriptor corrupt), belum mencakup pengujian fuzz otomatis berskala besar maupun skenario korupsi pada banyak record sekaligus. Debugging interaktif menggunakan GDB juga belum dilakukan pada modul ini.
```

### 22.3 Rencana Perbaikan

```text
Langkah selanjutnya adalah mengintegrasikan MCSFS1J ke dalam lapisan VFS MCSOS agar dapat diakses melalui syscall standar (open, read, write, close), sehingga journal benar-benar menjadi bagian dari jalur I/O pengguna, bukan hanya dipanggil langsung dari host test/kernel init.

Selain itu, akan dilakukan penambahan pengujian fault injection/fuzz yang lebih sistematis terhadap journal (mis. corrupt pada berbagai posisi descriptor/payload, kombinasi multi-crash), serta debugging interaktif menggunakan GDB untuk memverifikasi perilaku modul saat dipanggil dari konteks kernel sesungguhnya.
```

---

## 23. Lampiran

### Lampiran A — Commit Log

```text
274e01f (HEAD -> praktikum-m16-journal-recovery, origin/praktikum-m16-journal-recovery) M16 journal recovery implementation and verification
0d87222 (commit sebelum praktikum M16 dimulai)
```

### Lampiran B — Diff Ringkas

```diff
Perubahan berfokus pada penambahan modul write-ahead journal MCSFS1J (kernel/fs/mcsfs1j/), script preflight, Makefile host test M16, evidence audit, serta dua baris log integrasi pada kernel/block/block_demo.c:

 kernel/block/block_demo.c | 2 ++
 1 file changed, 2 insertions(+)
```

### Lampiran C — Log Build Lengkap

```text
make -C tests/m16 clean all
rm -f test_m16
rm -f m16_mcsfs_journal.o
rm -f nm_undefined.txt readelf_header.txt objdump_disasm.txt sha256sum.txt
cc -std=c11 -Wall -Wextra -Werror -pedantic -DMCSOS_M16_HOST_TEST ../../kernel/fs/mcsfs1j/m16_mcsfs_journal.c -o test_m16
cc -ffreestanding -fno-builtin -std=c11 -Wall -Wextra -Werror -c ../../kernel/fs/mcsfs1j/m16_mcsfs_journal.c -o m16_mcsfs_journal.o
nm -u m16_mcsfs_journal.o > nm_undefined.txt
readelf -h m16_mcsfs_journal.o > readelf_header.txt
objdump -d m16_mcsfs_journal.o > objdump_disasm.txt
sha256sum m16_mcsfs_journal.o > sha256sum.txt

make clean all
rm -rf build
... (kompilasi seluruh kernel, termasuk kernel/fs/mcsfs1j/m16_mcsfs_journal.c) ...
ld.lld -nostdlib -static -z max-page-size=0x1000 -T linker.ld -Map=build/mcsos-m5.map -o build/mcsos-m5.elf ...
readelf -h build/mcsos-m5.elf > build/readelf-header.txt
nm -n build/mcsos-m5.elf > build/symbols.txt
nm -u build/mcsos-m5.elf > build/undefined.txt
objdump -d -Mintel build/mcsos-m5.elf > build/disassembly.txt

make iso
cp build/mcsos-m5.elf iso_root/boot/kernel.elf
xorriso -as mkisofs ... iso_root -o build/mcsos.iso
limine/limine bios-install build/mcsos.iso
sha256sum build/mcsos.iso > build/mcsos.iso.sha256
[ISO] build/mcsos.iso ready
```

### Lampiran D — Log QEMU Lengkap

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
[M14] block layer initialized
[M14] ram0: 64 blocks x 512 bytes registered
[M15] mcsfs1 linked into kernel
[M15] ready for integration
[M16] mcsfs1j linked into kernel
[M16] journal recovery support available
```

### Lampiran E — Output Readelf/Objdump

```text
ELF Header (m16_mcsfs_journal.o):
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  Type:                              REL (Relocatable file)
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Start of section headers:          9904 (bytes into file)
  Number of section headers:         13

nm build/mcsos-m5.elf | grep m16_
ffffffff80002f20 T m16_dev_init
ffffffff800034f0 T m16_format
ffffffff80004920 T m16_fsck
ffffffff80002fd0 T m16_journal_recover
ffffffff80003850 T m16_mount
ffffffff80004670 T m16_read_file
ffffffff80004630 T m16_write_file
ffffffff80003950 T m16_write_file_ex
```

### Lampiran F — Screenshot

| No. | File | Keterangan |
|---:|---|---|
| 1 | `logs/m16/preflight.log` | Log preflight sebelum pengujian |
| 2 | `logs/m16/m16_make_all.log` | Log kompilasi dan host test |
| 3 | `logs/m16/qemu_serial.log` | Log serial QEMU selama proses boot |
| 4 | `evidence/m16/nm_undefined.txt` | Hasil audit undefined symbol |
| 5 | `evidence/m16/readelf_header.txt` | Informasi header ELF object |
| 6 | `evidence/m16/objdump_disasm.txt` | Hasil disassembly object M16 |
| 7 | `evidence/m16/sha256sum.txt` | Nilai SHA-256 artefak M16 |

### Lampiran G — Bukti Tambahan

```text
- evidence/m16/host_test.log        : output ./tests/m16/test_m16
- evidence/m16/nm_undefined.txt     : hasil nm -u pada m16_mcsfs_journal.o (kosong)
- evidence/m16/readelf_header.txt   : hasil readelf -h pada m16_mcsfs_journal.o
- evidence/m16/objdump_disasm.txt   : hasil objdump -d pada m16_mcsfs_journal.o
- evidence/m16/sha256sum.txt        : checksum m16_mcsfs_journal.o
- evidence/m16/test_m16.sha256      : checksum binary test_m16
- evidence/m16/qemu_serial.log      : log serial QEMU yang memuat baris [M16]
- evidence/m16/m16_symbols.log      : daftar simbol m16_* pada build/mcsos-m5.elf
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
1 R. H. Arpaci-Dusseau and A. C. Arpaci-Dusseau, Operating Systems: Three Easy Pieces. Madison, WI, USA: Arpaci-Dusseau Books, 2018. [Online]. Available: https://pages.cs.wisc.edu/~remzi/OSTEP/. Accessed: Jun 13, 2026.

2 Intel Corporation, Intel 64 and IA-32 Architectures Software Developer’s Manual. [Online]. Available: https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html. Accessed: Jun 13, 2026.

3 Advanced Micro Devices, AMD64 Architecture Programmer’s Manual. [Online]. Available: https://www.amd.com/system/files/TechDocs/24594.pdf. Accessed: Jun 13, 2026.

4 GNU Project, “GNU Make Manual.” [Online]. Available: https://www.gnu.org/software/make/manual/. Accessed: Jun 13, 2026.

5 LLVM Project, “Clang Documentation.” [Online]. Available: https://clang.llvm.org/docs/. Accessed: Jun 13, 2026.

6 OSDev Wiki, “Journaling.” [Online]. Available: https://wiki.osdev.org/Journaling. Accessed: Jun 13, 2026.
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
| Log QEMU/test dilampirkan | Ya |
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
274e01f
```

Status akhir yang diklaim:

```text
siap uji QEMU
```

Ringkasan satu paragraf:

```text
Pada praktikum M16, telah berhasil diimplementasikan modul write-ahead journal MCSFS1J di atas filesystem MCSFS1, mencakup struktur commit record dan checksum, fungsi commit/recovery/fsck, serta host unit test yang menguji skenario normal, crash mid-transaction, dan descriptor journal yang corrupt — seluruhnya lulus dengan pesan "M16 host tests PASS". Modul juga berhasil dikompilasi sebagai object freestanding tanpa undefined symbol, dilink ke dalam kernel MCSOS (dibuktikan melalui nm pada build/mcsos-m5.elf), dan terbukti aktif pada runtime melalui log serial QEMU "[M16] mcsfs1j linked into kernel". Karena integrasi penuh ke VFS dan pengujian fault injection berskala besar belum dilakukan, hasil praktikum ini dikategorikan siap uji QEMU dan akan dilanjutkan pada milestone berikutnya untuk integrasi VFS dan pengujian lanjutan.
```
