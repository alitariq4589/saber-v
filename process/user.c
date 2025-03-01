
extern char __stack_top[];


__attribute__((noreturn))
void exit(void){
  for (;;);
}

// Entry point of user-mode process. Has to remain here because it is used in the linker file
__attribute__((section(".text.start")))
__attribute__((naked)) void
start(void)
{
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n" // Set the stack pointer
        "call main\n"       // Jump to the user mode main function
        "call exit\n"       // Jump to the user mode main function
        :
        : [stack_top] "r"(__stack_top) // Pass the stack top address as %[stack_top]
    );
}