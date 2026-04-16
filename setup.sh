#!/usr/bin/env bash
set -euo pipefail

# Installs host dependencies needed to build riscv-gnu-toolchain from source.
# Supports Ubuntu/Debian (apt) and Arch/Manjaro (pacman).

if [[ "${EUID}" -eq 0 ]]; then
  SUDO=""
else
  SUDO="sudo"
fi

if command -v apt-get >/dev/null 2>&1; then
  echo "[setup] Detected apt-based distro. Installing dependencies..."
  ${SUDO} apt-get update
  ${SUDO} apt-get install -y \
    autoconf automake autotools-dev curl python3 python3-pip \
    libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf \
    libtool patchutils bc zlib1g-dev libexpat1-dev ninja-build git cmake \
    device-tree-compiler pkg-config

  echo "[setup] Optional emulator packages (QEMU)..."
  ${SUDO} apt-get install -y qemu-user qemu-system-misc || true

elif command -v pacman >/dev/null 2>&1; then
  echo "[setup] Detected pacman-based distro. Installing dependencies..."
  ${SUDO} pacman -Syu --needed --noconfirm \
    base-devel git curl gawk bison flex texinfo gmp mpfr libmpc zlib expat \
    python python-pip cmake ninja pkgconf dtc

  echo "[setup] Optional emulator packages (QEMU)..."
  ${SUDO} pacman -S --needed --noconfirm qemu-system-riscv qemu-user || true

else
  echo "[setup] Unsupported distro package manager."
  echo "[setup] Please install toolchain build dependencies manually."
  exit 1
fi

echo "[setup] Done. You can now build riscv-gnu-toolchain from source if needed."
