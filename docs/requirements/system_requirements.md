# System Requirements MCSOS 260502 — Baseline M0

## Scope
Dokumen ini menetapkan requirement awal untuk proyek MCSOS 260502.
Requirement pada M0 berfokus pada lingkungan, governance, dan evidence.
Requirement runtime kernel akan diperinci pada milestone berikutnya.

| ID | Requirement | Rationale | Verification evidence |
|---|---|---|---|
| REQ-M0-001 | Repository harus di filesystem Linux, bukan /mnt/c | Hindari masalah permission | pwd + check_env |
| REQ-M0-002 | Semua tool harus terdeteksi | Build harus reproducible | check_env |
| REQ-M0-003 | Versi toolchain dicatat | Traceability | file metadata |
| REQ-M0-004 | Struktur repo harus lengkap | Konsistensi | tree |
| REQ-M0-005 | Smoke test hasil ELF64 x86-64 | Validasi toolchain | readelf |
| REQ-M0-006 | Ada assumptions & non-goals | Hindari scope creep | docs |
| REQ-M0-007 | Ada ADR awal | Trace keputusan | docs |
| REQ-M0-008 | Ada threat model | Security awareness | docs |
| REQ-M0-009 | Ada risk register | Manajemen risiko | docs |
| REQ-M0-010 | Ada verification matrix | Validasi requirement | docs |
| REQ-M0-011 | Semua perubahan di-commit | Traceability | git log |
| REQ-M0-012 | Laporan M0 lengkap | Evidence | laporan |
