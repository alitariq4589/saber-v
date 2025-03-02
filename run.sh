#!/bin/bash
set -xue

MODE=${1:-run}
echo "$MODE"

QEMU=qemu-system-riscv32

OBJCOPY=llvm-objcopy

#Build the user-mode application first
# process/run.sh

# new: Path to clang and compiler flags
CC=clang  # Ubuntu users: use CC=clang
CFLAGS="-std=c11 -O0 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib"

$CC $CFLAGS -Wl,-Tprocess/user.ld -Wl,-Map=./process/user.map -o ./process/user.elf \
    ./process/user.c ./process/app.c ./common.c

llvm-objdump -d ./process/user.elf > ./process/user.obj

$OBJCOPY --set-section-flags .bss=alloc,contents -O binary ./process/user.elf ./process/user.bin
$OBJCOPY -Ibinary -Oelf32-littleriscv ./process/user.bin ./process/user.bin.o

# new: Build the kernel
$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o kernel.elf \
    kernel.c kernel_functions.c common.c ./process/user.bin.o

llvm-objdump -d kernel.elf > kernel.obj

if [[ "$MODE" == "debug" ]];
then
# Start QEMU
$QEMU -s -S -machine virt  -bios opensbi-riscv32-generic-fw_dynamic.bin -nographic -serial mon:stdio -smp cpus=1 --no-reboot \
    -kernel kernel.elf # new: Load the kernel
else
echo "inside run"
# Start QEMU
$QEMU -machine virt -bios opensbi-riscv32-generic-fw_dynamic.bin -nographic -serial mon:stdio --no-reboot \
    -kernel kernel.elf # new: Load the kernel
fi
