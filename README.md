# saber-v

A very minimal lightweight RISC-V OS.

Following are the details of each file.

1. `kernel.c` contains the main function of the kernel
2. `kernel.h` contains the headers and function macros
3. `functions.c` contians the necessary functions
4. `kernel.ld` is the linker file containing the memory map
5. `run.sh` is the bash script for compiling and running the code

Other compiled files are not necessary since they can be reproduced again by running `run.sh`

This OS is implemented for the following machine.

- `qemu-system-riscv32`

## Pre-requisites

This source code is tested on 

- `qemu-system-riscv32` version 8.2.0 
- `clang` version 18.1.8

## Running the OS

For running the OS, you can simply execute `run.sh` and you will see following screen.

```
./run.sh
+ QEMU=qemu-system-riscv32
+ CC=clang
+ CFLAGS='-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib'
+ clang -std=c11 -O2 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o kernel.elf kernel.c functions.c
+ llvm-objdump -d kernel.elf
+ qemu-system-riscv32 -machine virt -bios opensbi-riscv32-generic-fw_dynamic.bin -nographic -serial mon:stdio --no-reboot -kernel kernel.elf

OpenSBI v1.2
   ____                    _____ ____ _____
  / __ \                  / ____|  _ \_   _|
 | |  | |_ __   ___ _ __ | (___ | |_) || |
 | |  | | '_ \ / _ \ '_ \ \___ \|  _ < | |
 | |__| | |_) |  __/ | | |____) | |_) || |_
  \____/| .__/ \___|_| |_|_____/|____/_____|
        | |
        |_|

Platform Name             : riscv-virtio,qemu
Platform Features         : medeleg
Platform HART Count       : 1
Platform IPI Device       : aclint-mswi
Platform Timer Device     : aclint-mtimer @ 10000000Hz
Platform Console Device   : uart8250
Platform HSM Device       : ---
Platform PMU Device       : ---
Platform Reboot Device    : sifive_test
Platform Shutdown Device  : sifive_test
Firmware Base             : 0x80000000
Firmware Size             : 208 KB
Runtime SBI Version       : 1.0

Domain0 Name              : root
Domain0 Boot HART         : 0
Domain0 HARTs             : 0*
Domain0 Region00          : 0x02000000-0x0200ffff (I)
Domain0 Region01          : 0x80000000-0x8003ffff ()
Domain0 Region02          : 0x00000000-0xffffffff (R,W,X)
Domain0 Next Address      : 0x80200000
Domain0 Next Arg1         : 0x87e00000
Domain0 Next Mode         : S-mode
Domain0 SysReset          : yes

Boot HART ID              : 0
Boot HART Domain          : root
Boot HART Priv Version    : v1.12
Boot HART Base ISA        : rv32imafdch
Boot HART ISA Extensions  : time,sstc
Boot HART PMP Count       : 16
Boot HART PMP Granularity : 4
Boot HART PMP Address Bits: 32
Boot HART MHPM Count      : 16
Boot HART MIDELEG         : 0x00001666
Boot HART MEDELEG         : 0x00f0b509



███████╗ █████╗ ██████╗ ███████╗██████╗               ██╗   ██╗
██╔════╝██╔══██╗██╔══██╗██╔════╝██╔══██╗              ██║   ██║
███████╗███████║██████╔╝█████╗  ██████╔╝    █████╗    ██║   ██║
╚════██║██╔══██║██╔══██╗██╔══╝  ██╔══██╗    ╚════╝    ╚██╗ ██╔╝
███████║██║  ██║██████╔╝███████╗██║  ██║               ╚████╔╝ 
╚══════╝╚═╝  ╚═╝╚═════╝ ╚══════╝╚═╝  ╚═╝                ╚═══╝  



                    WELCOME TO SABER-V!              

         A very lightweight Open-source RISC-V OS          



Initializing stvec with trap handler...

Kernel Panic! : kernel.c : 16: Code Completed!!!
```

## Debugging with GDB

The file `gdb_commands.txt` contains the commands which are passed to the GDB and can be used for debugging.

Use `run.sh` with `debug` argument to switch to debugging.

Then use the gdb from riscv cross-compiler toolchain with following command to start the program in GDB.

```
riscv32-unknown-linux-gnu-gdb ./kernel.elf -x ./gdb_commands.txt
```


## Future Objectives

- [ ] Retire the process properly by freeing the held pages in memory and the memory allocated by the exited process
- [ ] Add essential linux bianaries (e.g. gcc compiler, sh, bash) in the disk space
- [ ] Add mutithreading support
- [ ] Add support for running this kernel on RISC-V SBCs (such as VisionFive 1&2, HiFive Unleashed etc.)


### Long-term objective

- [ ] Develop a rust port of this kernel (and possibly opensbi too)