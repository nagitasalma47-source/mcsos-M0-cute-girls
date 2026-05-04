# Verification Matrix MCSOS 260502 — M0

| Requirement | Verification command | Expected evidence | Pass/Fail |
|---|---|---|---|
| REQ-M0-001 | pwd | path di /home/.../src/mcsos | TBD |
| REQ-M0-002 | bash tools/check_env.sh | semua tool OK | TBD |
| REQ-M0-003 | cat build/meta/toolchain-versions.txt | versi tercatat | TBD |
| REQ-M0-004 | tree -a -L 3 | struktur lengkap | TBD |
| REQ-M0-005 | make smoke | ELF64 x86-64 | TBD |
| REQ-M0-006 | test -s docs/requirements/assumptions_and_nongoals.md | file ada | TBD |
| REQ-M0-007 | test -s docs/adr/ADR-0001-toolchain-and-boot-baseline.md | file ada | TBD |
| REQ-M0-008 | test -s docs/security/threat_model.md | file ada | TBD |
| REQ-M0-009 | test -s docs/governance/risk_register.md | file ada | TBD |
| REQ-M0-010 | test -s docs/testing/verification_matrix.md | file ada | TBD |
| REQ-M0-011 | git log --oneline -n 3 | ada commit | TBD |
| REQ-M0-012 | test -s docs/reports/M0-laporan.md | laporan ada | TBD |
