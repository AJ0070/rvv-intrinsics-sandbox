# rvv-intrinsics-sandbox

Minimal, self-contained C sandbox for learning and validating **RISC-V Vector (RVV) intrinsics** with cross-compilation and emulation.

This first commit focuses on environment setup:
- Installing a RISC-V GCC toolchain
- Installing an emulator (QEMU and/or Spike)
- Preparing host build dependencies for toolchain-from-source fallback

---

## 1) Host Assumptions

- OS: Linux (Ubuntu/Debian or Arch/Manjaro)
- Shell: bash
- Disk: at least ~8 GB free for source builds
- RAM: 8 GB+ recommended for faster toolchain builds

---

## 2) Fast Path: Install Prebuilt Toolchain + Emulator

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install -y \
  gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu \
  qemu-user qemu-system-misc
```

Check binaries:

```bash
riscv64-linux-gnu-gcc --version
qemu-riscv64 --version
qemu-system-riscv64 --version
```

### Arch / Manjaro

```bash
sudo pacman -Syu --needed \
  riscv64-linux-gnu-gcc riscv64-linux-gnu-binutils \
  qemu-system-riscv qemu-user
```

Check binaries:

```bash
riscv64-linux-gnu-gcc --version
qemu-riscv64 --version
qemu-system-riscv64 --version
```

> Note: Package names can vary slightly by distro release. If your package manager cannot find these names, use `setup.sh` (below) to install source-build dependencies and build the toolchain from source.

---

## 3) Fallback: Build `riscv-gnu-toolchain` From Source

If prebuilt packages are unavailable or outdated, build locally.

### 3.1 Install build dependencies

Use the provided script:

```bash
bash setup.sh
```

### 3.2 Clone and build

```bash
git clone https://github.com/riscv-collab/riscv-gnu-toolchain.git
cd riscv-gnu-toolchain

git submodule update --init --recursive
```

#### Option A: Bare-metal (`riscv64-unknown-elf-gcc`)

```bash
./configure --prefix=$HOME/opt/riscv --with-arch=rv64gcv --with-abi=lp64d
make -j"$(nproc)"
```

#### Option B: Linux target (`riscv64-unknown-linux-gnu-gcc`)

```bash
./configure --prefix=$HOME/opt/riscv --enable-multilib
make linux -j"$(nproc)"
```

Add to PATH:

```bash
echo 'export PATH=$HOME/opt/riscv/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
```

Validate:

```bash
riscv64-unknown-elf-gcc --version || true
riscv64-unknown-linux-gnu-gcc --version || true
```

---

## 4) Emulator Options

## Option A: QEMU (recommended for this sandbox)

You can run Linux-user mode (`qemu-riscv64`) or full-system mode (`qemu-system-riscv64`).

Verify installation:

```bash
qemu-riscv64 --version
qemu-system-riscv64 --version
```

## Option B: Spike (ISA simulator)

Build from source when distro packages are unavailable:

```bash
git clone https://github.com/riscv-software-src/riscv-isa-sim.git
cd riscv-isa-sim
mkdir build && cd build
../configure --prefix=$HOME/opt/riscv
make -j"$(nproc)"
make install
```

Optional: build Proxy Kernel (`pk`) if running ELF payloads under Spike:

```bash
git clone https://github.com/riscv-software-src/riscv-pk.git
cd riscv-pk
mkdir build && cd build
../configure --prefix=$HOME/opt/riscv --host=riscv64-unknown-elf
make -j"$(nproc)"
make install
```

---

## 5) RVV Target Flags (used in later commits)

Primary target flags we will use in this project:

- `-march=rv64gcv`
- `-mabi=lp64d`

Alternative (if your toolchain supports additional extensions):

- `-march=rv64gcv_zba_zbb_zbc_zbs`

We will explicitly include RVV intrinsics header in vector code:

```c
#include <riscv_vector.h>
```

---

## 6) Next Steps in This Repo

Upcoming commits will add:
- Project layout + `Makefile`
- Scalar baseline math ops
- RVV intrinsics implementation
- Verification driver
- Emulator run script

This repository intentionally remains minimal and educational.
