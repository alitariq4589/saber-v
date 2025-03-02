#include "user.h"

extern char __stack_top[];

void syscall(unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long sysno ){
  register int a0 __asm__("a0") = arg1;
  register int a1 __asm__("a1") = arg2;
  register int a2 __asm__("a2") = arg3;
  register int a3 __asm__("a3") = sysno;

  __asm__ __volatile__(
    "ecall\n"
    :"=r"(a0)
    :"r"(a0),"r"(a1),"r"(a2),"r"(a3)
    :"memory"
  );
}

void putchar(char ch){
  syscall(ch,0,0,SYS_PUTCHAR);
}


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