#include "kernel.h"
#include "functions.h"

/*Main function for the kernel which is called by boot function*/
void kernel_main(void)
{

    print_title();
    printf("\nInitializing stvec trap handler...\n");
    initialize_stvec();

    unsigned long *start_addr;
    start_addr = (unsigned long *) __free_mem;

    memset(start_addr, 1, 40);

    for (int i=0;i<40; i++){
        printf("%d\n", *start_addr);
        start_addr++;
    }


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



