#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
META_DIR="$ROOT_DIR/build/meta"
mkdir -p "$META_DIR"

fail=0

echo "[M0] Repository root: $ROOT_DIR"

case "$ROOT_DIR" in
  /mnt/c/*|/mnt/d/*|/mnt/e/*)
    echo "[WARN] Repository appears to be on Windows filesystem"
    ;;
  *)
    echo "[OK] Repository is not under /mnt/<drive>."
    ;;
esac

echo "[M0] Checking required tools"

for tool in git make clang ld.lld llvm-readelf llvm-objdump readelf objdump nasm qemu-system-x86_64 gdb python3 shellcheck cppcheck; do
  if command -v "$tool" >/dev/null 2>&1; then
    printf '[OK]   %-24s %s\n' "$tool" "$(command -v "$tool")"
  else
    printf '[FAIL] %-24s not found\n' "$tool"
    fail=1
  fi
done

echo "[M0] Writing toolchain metadata"

{
  echo "date_utc=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
  echo "root_dir=$ROOT_DIR"
  echo "uname=$(uname -a)"
  echo "wsl_distro=${WSL_DISTRO_NAME:-unknown}"
  echo
  echo "## Tool versions"
  git --version || true
  make --version | head -n 1 || true
  clang --version | head -n 1 || true
  ld.lld --version | head -n 1 || true
  llvm-readelf --version | head -n 1 || true
  llvm-objdump --version | head -n 1 || true
  readelf --version | head -n 1 || true
  objdump --version | head -n 1 || true
  nasm -v || true
  qemu-system-x86_64 --version | head -n 1 || true
  gdb --version | head -n 1 || true
  python3 --version || true
  shellcheck --version | head -n 2 || true
  cppcheck --version || true
} > "$META_DIR/toolchain-versions.txt"

echo "[M0] Metadata written to build/meta/toolchain-versions.txt"

if [ "$fail" -ne 0 ]; then
  echo "[M0] Environment check failed."
  exit 1
fi

echo "[M0] Environment check completed. M0 siap uji lingkungan, bukan siap boot."

