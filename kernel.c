
#include "functions.h"

struct process *proc_a;
struct process *proc_b;



void process_a_entry(){
    printf("\nIn Process A\n");

    __asm__ __volatile__("nop\n" "nop\n" "nop\n");
    switch_context(&proc_a->sp, &proc_b->sp);
}
void process_b_entry(){
    printf("\nIn Process B\n");

    switch_context(&proc_b->sp, &proc_a->sp);
}


/*Main function for the kernel which is called by boot function*/
void kernel_main(void)
{

    print_title();
    printf("\nInitializing stvec with trap handler...\n");
    initialize_stvec();

    // ============================================//
    // Write your code here!

    // Created first process
    proc_a = create_process((unsigned long) process_a_entry);
    proc_b = create_process((unsigned long) process_b_entry);

    // unsigned long *process_a_stackpointer_variable = proc_a->sp;
    unsigned long *process_b_stackpointer = proc_b->sp;
    printf("Process A entry point: %x\n", process_a_entry);
    printf("Process B entry point: %x\n", process_b_entry);
    // printf("Process a stack array address: %x\n", process_a_stackpointer);
    for (int i=0; i<13; i++){
        printf("The address:%x      ", process_b_stackpointer+i);
        printf("The value: %x\n",*(process_b_stackpointer+i));


    }
    process_a_entry();



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
