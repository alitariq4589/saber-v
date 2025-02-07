#!/bin/bash
set -xue

MODE=${1:-run}
echo "$MODE"
QEMU=qemu-system-riscv32

# new: Path to clang and compiler flags
CC=clang  # Ubuntu users: use CC=clang
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib"

# new: Build the kernel
$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o kernel.elf \
    kernel.c functions.c 

llvm-objdump -d kernel.elf > kernel.obj

if [[ "$MODE" == "debug" ]];
then
  echo "Starting in debug mode"
  # Start QEMU
  $QEMU -s -S -machine virt -bios opensbi-riscv32-generic-fw_dynamic.bin -nographic -serial mon:stdio --no-reboot \
  -kernel kernel.elf # new: Load the kernel
else
  # Start QEMU
  $QEMU -machine virt -bios opensbi-riscv32-generic-fw_dynamic.bin -nographic -serial mon:stdio --no-reboot \
  -kernel kernel.elf # new: Load the kernel
fi
