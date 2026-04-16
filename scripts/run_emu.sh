#!/usr/bin/env bash
set -euo pipefail

# Usage:
#   ./scripts/run_emu.sh qemu [binary]
#   ./scripts/run_emu.sh spike [binary]
#
# Defaults:
#   emulator: qemu
#   binary:   build/rvv_sandbox.elf

EMU="${1:-qemu}"
BIN="${2:-build/rvv_sandbox.elf}"

if [[ ! -f "${BIN}" ]]; then
  echo "[run_emu] Binary not found: ${BIN}"
  echo "[run_emu] Build first: make"
  exit 1
fi

case "${EMU}" in
  qemu)
    QEMU_BIN=""
    if command -v qemu-riscv64 >/dev/null 2>&1; then
      QEMU_BIN="qemu-riscv64"
    elif command -v qemu-riscv64-static >/dev/null 2>&1; then
      QEMU_BIN="qemu-riscv64-static"
    else
      echo "[run_emu] qemu-riscv64 (or qemu-riscv64-static) not found in PATH"
      echo "[run_emu] Install qemu user-mode package (e.g. qemu-user)"
      echo "[run_emu] Note: qemu-system-riscv64 alone cannot directly run linux user ELF binaries"
      exit 1
    fi

    echo "[run_emu] Running with QEMU user-mode and RVV enabled..."

    cpu_configs=(
      "rv64,v=true,vlen=256,elen=64,vext_spec=v1.0"
      "rv64,v=true,vlen=256,elen=64"
      "max,v=true"
    )

    for cpu_cfg in "${cpu_configs[@]}"; do
      echo "[run_emu] Trying CPU config: ${cpu_cfg}"
      set +e
      "${QEMU_BIN}" -cpu "${cpu_cfg}" "${BIN}"
      exit_code=$?
      set -e

      if [[ ${exit_code} -eq 0 ]]; then
        exit 0
      fi

      echo "[run_emu] CPU config failed with exit code ${exit_code}"
    done

    echo "[run_emu] All QEMU CPU configurations failed"
    exit 1
    ;;

  spike)
    if ! command -v spike >/dev/null 2>&1; then
      echo "[run_emu] spike not found in PATH"
      exit 1
    fi

    if ! command -v pk >/dev/null 2>&1; then
      echo "[run_emu] pk (riscv proxy kernel) not found in PATH"
      echo "[run_emu] Install/build riscv-pk to use spike mode"
      exit 1
    fi

    echo "[run_emu] Running with Spike (ISA=rv64gcv)..."
    exec spike --isa=rv64gcv pk "${BIN}"
    ;;

  *)
    echo "[run_emu] Unknown emulator: ${EMU}"
    echo "[run_emu] Use: qemu or spike"
    exit 1
    ;;
esac
