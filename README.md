# saber-v

A very minimal lightweight RISC-V OS.

Following are the details of each file.

1. `kernel.c` contains the main function of the kernel
2. `kernel.h` contains the headers and function macros
3. `functions.c` contians the necessary functions
4. `kernel.ld` is the linker file containing the memory map
5. `run.sh` is the bash script for compiling and running the code

Other compiled files are not necessary since they can be reproduced again by running `run.sh`

This kernel is implemented for the following machine.

- `qemu-system-riscv32`
