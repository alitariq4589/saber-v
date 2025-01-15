#include "kernel.h"
#include "functions.h"

void kernel_main(void)
{
    printf("\n\n\n");
    print_title();
    printf("                WELCOME TO SABER-V!              \n");
    printf("\n");
    printf("         Vendor Agnostic Open-source RISC-V OS          \n");
    printf("\n\n");

    printf("\nInitializing stvec trap handler...\n");
    initialize_stvec();

 
    printf("The address of the kernel entry is: %d\n", (unsigned long) kernel_entry);
    printf("The address of the kernel main is: %d\n", (unsigned long) kernel_main);

    int hex_num = 9;

    int csr_value = READ_CSR(stvec);
     csr_value = READ_CSR(stvec);
     csr_value = READ_CSR(sepc);
    printf ("The value of sepc before exception: %d\n", csr_value);
    __asm__ __volatile__ ("unimp");

    printf ("The value in hex = %x",hex_num);
    PANIC("Code Completed!!!");

    for (;;)
    {
        __asm__ __volatile__("wfi");
    }
}

// Boot code. Has to remain here because it is used in the linker file
__attribute__((section(".text.boot")))
__attribute__((naked)) void
boot(void)
{
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n" // Set the stack pointer
        "j kernel_main\n"       // Jump to the kernel main function
        :
        : [stack_top] "r"(__stack_top) // Pass the stack top address as %[stack_top]
    );
}



