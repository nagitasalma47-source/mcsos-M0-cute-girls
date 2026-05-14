#!/usr/bin/env bash
set -euo pipefail

echo "[M7-PREFLIGHT] pemeriksaan lingkungan dan hasil M0-M6"

need_cmd() {
  if ! command -v "$1" >/dev/null 2>&1; then
    echo "[FAIL] command tidak ditemukan: $1" >&2
    exit 1
  fi
  echo "[OK] $1 -> $(command -v "$1")"
}

need_file() {
  if [ ! -f "$1" ]; then
    echo "[FAIL] file wajib tidak ada: $1" >&2
    exit 1
  fi
  echo "[OK] file ada: $1"
}

need_dir() {
  if [ ! -d "$1" ]; then
    echo "[FAIL] direktori wajib tidak ada: $1" >&2
    exit 1
  fi
  echo "[OK] direktori ada: $1"
}

need_cmd git
need_cmd make
need_cmd clang
need_cmd ld.lld
need_cmd readelf
need_cmd objdump
need_cmd nm
need_cmd qemu-system-x86_64

need_dir include
need_dir src
need_dir tests
need_file include/pmm.h
need_file src/pmm.c
need_file include/vmm.h
need_file src/vmm.c
need_file tests/test_vmm_host.c
need_file Makefile
