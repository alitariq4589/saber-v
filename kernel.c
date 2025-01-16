#include "kernel.h"
#include "functions.h"

/*Main function for the kernel which is called by boot function*/
void kernel_main(void)
{

    print_title();
    printf("\nInitializing stvec with trap handler...\n");
    initialize_stvec();

    unsigned char *get_nine_bytes = malloc(9);
    printf("%d\n", *get_nine_bytes - 1 );
    for (int i = 0; i < 10; i++)
    {
        printf("%d\n", *get_nine_bytes);
        get_nine_bytes++;
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
