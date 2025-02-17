
#include "functions.h"

struct process *proc_app;
// struct process *proc_b;

void process_a_entry()
{
    printf("\nIn Process A\n");
    __asm__ __volatile__("nop\n"
                         "nop\n"
                         "nop\n");
    // switch_context(&proc_a->sp, &proc_b->sp);
    yield();
}
void process_b_entry()
{
    printf("\nIn Process B\n");

    // switch_context(&proc_b->sp, &proc_a->sp);
    yield();
}

/*Main function for the kernel which is called by boot function*/
void kernel_main(void)
{

    print_title();
    printf("\nInitializing stvec with trap handler...\n");
    initialize_stvec();

    // ============================================//
    // Write your code here!

    // Created processes
    proc_app = create_process(_binary___process_user_bin_start, (unsigned long)_binary___process_user_bin_size);


    initialize_processes();
    yield();

    // ============================================//

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
