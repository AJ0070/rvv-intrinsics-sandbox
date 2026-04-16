# rvv-intrinsics-sandbox

[![CI](https://github.com/AJ0070/rvv-intrinsics-sandbox/actions/workflows/ci.yml/badge.svg)](https://github.com/AJ0070/rvv-intrinsics-sandbox/actions/workflows/ci.yml)

Minimal C sandbox to validate **RISC-V Vector (RVV) intrinsics** end-to-end:
- cross-compile with RISC-V GCC
- run under QEMU/Spike
- verify RVV math output vs scalar baseline

## Quick Start (Ubuntu/Debian)

Install dependencies:

```bash
sudo apt update
sudo apt install -y \
  gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu \
  qemu-user qemu-system-misc
```

Build and run:

```bash
make clean
make -j"$(nproc)" TOOLCHAIN_PREFIX=riscv64-linux-gnu-
./scripts/run_emu.sh qemu
```

Expected output:

```text
RVV Verification Passed!
```

## Install Notes

### Ubuntu / Debian (prebuilt toolchain)

```bash
sudo apt update
sudo apt install -y \
  gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu \
  qemu-user qemu-system-misc
```

### Arch / Manjaro (prebuilt toolchain)

```bash
sudo pacman -Syu --needed \
  riscv64-linux-gnu-gcc riscv64-linux-gnu-binutils \
  qemu-system-riscv qemu-user
```

Validate binaries:

```bash
riscv64-linux-gnu-gcc --version
qemu-riscv64 --version
qemu-system-riscv64 --version
```

If packages are unavailable/outdated, use [setup.sh](setup.sh) then build `riscv-gnu-toolchain` from source.

## Build From Source (Fallback)

Install host dependencies:

```bash
bash setup.sh
```

Clone toolchain:

```bash
git clone https://github.com/riscv-collab/riscv-gnu-toolchain.git
cd riscv-gnu-toolchain

git submodule update --init --recursive
```

Build bare-metal toolchain (`riscv64-unknown-elf-gcc`):

```bash
./configure --prefix=$HOME/opt/riscv --with-arch=rv64gcv --with-abi=lp64d
make -j"$(nproc)"
```

Or Linux toolchain (`riscv64-unknown-linux-gnu-gcc`):

```bash
./configure --prefix=$HOME/opt/riscv --enable-multilib
make linux -j"$(nproc)"
```

Add to `PATH`:

```bash
echo 'export PATH=$HOME/opt/riscv/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
```

Validate:

```bash
riscv64-unknown-elf-gcc --version || true
riscv64-unknown-linux-gnu-gcc --version || true
```

## Emulator Modes

### QEMU (recommended)

Use Linux-user mode (`qemu-riscv64`) for this sandbox. Full system (`qemu-system-riscv64`) is optional.

Runner script:

```bash
./scripts/run_emu.sh qemu [binary]
```

### Spike

Build Spike from source when distro packages are unavailable:

```bash
git clone https://github.com/riscv-software-src/riscv-isa-sim.git
cd riscv-isa-sim
mkdir build && cd build
../configure --prefix=$HOME/opt/riscv
make -j"$(nproc)"
make install
```

Optional: build Proxy Kernel (`pk`) for Spike payload execution:

```bash
git clone https://github.com/riscv-software-src/riscv-pk.git
cd riscv-pk
mkdir build && cd build
../configure --prefix=$HOME/opt/riscv --host=riscv64-unknown-elf
make -j"$(nproc)"
make install
```

## RVV Compilation Flags

Primary flags used in this project:

- `-march=rv64gcv`
- `-mabi=lp64d`

Optional alternative (toolchain-dependent):

- `-march=rv64gcv_zba_zbb_zbc_zbs`

RVV source explicitly includes:

```c
#include <riscv_vector.h>
```

## CI

Current CI checks include:
- Linux GNU cross-build
- artifact handoff + QEMU runtime verification
- bare-metal build
- `cppcheck`
- `clang-format` check
- `shellcheck`
