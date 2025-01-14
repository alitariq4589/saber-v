#include "kernel.h"
void initialize_stvec();
extern char __bss[], __bss_end[], __stack_top[];

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4,
                       long arg5, long fid, long eid)
{
    register int a0 __asm__("a0") = arg0;
    register int a1 __asm__("a1") = arg1;
    register int a2 __asm__("a2") = arg2;
    register int a3 __asm__("a3") = arg3;
    register int a4 __asm__("a4") = arg4;
    register int a5 __asm__("a5") = arg5;
    register int a6 __asm__("a6") = fid;
    register int a7 __asm__("a7") = eid;

    __asm__ __volatile__("ecall"
                         : "=r"(a0), "=r"(a1)
                         : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                           "r"(a6), "r"(a7)
                         : "memory");
    return (struct sbiret){.error = a0, .value = a1};
}

void putchar(char ch)
{
    sbi_call(ch, 0, 0, 0, 0, 0, 0, 1 /* Console Putchar */);
}

void printf(char *fmt, ...)
{

    va_list args;
    va_start(args, fmt);

    char *str;

    int number_to_print;

    while (*fmt)
    {
        if (*fmt == '%')
        {

            fmt++;

            // The string print
            if (*fmt == 's')
            {
                fmt++;
                str = va_arg(args, char *);
                while (*str)
                {
                    putchar(*str);
                    str++;
                }
            }

            // The integer print
            if (*fmt == 'd')
            {
                // fmt++;
                unsigned long number = va_arg(args, int);
                unsigned long rev_number = 0;
                while (number > 9)
                {
                    rev_number = (rev_number * 10) + (number % 10);
                    number /= 10;
                }

                rev_number = (rev_number * 10) + number;
                while (rev_number > 9)
                {
                    number_to_print = rev_number % 10;
                    putchar('0' + number_to_print);
                    rev_number /= 10;
                }
                putchar('0' + rev_number);
            }

            // Hexadecimal print
            if (*fmt == 'x')
            {
                unsigned int number = va_arg(args, int);
                for (int i=7; i>=0; i--){
                    int hex_digit = (number >> (i*4)) % 15;
                    putchar("0123456789abcdefgh"[hex_digit]);
                }
            }
        }
        else
        {

            putchar(*fmt);
        }
        fmt++;
    }
}

void print_title(void)
{
    printf("███████╗ █████╗ ██████╗ ███████╗██████╗               ██╗   ██╗\n");
    printf("██╔════╝██╔══██╗██╔══██╗██╔════╝██╔══██╗              ██║   ██║\n");
    printf("███████╗███████║██████╔╝█████╗  ██████╔╝    █████╗    ██║   ██║\n");
    printf("╚════██║██╔══██║██╔══██╗██╔══╝  ██╔══██╗    ╚════╝    ╚██╗ ██╔╝\n");
    printf("███████║██║  ██║██████╔╝███████╗██║  ██║               ╚████╔╝ \n");
    printf("╚══════╝╚═╝  ╚═╝╚═════╝ ╚══════╝╚═╝  ╚═╝                ╚═══╝  \n");
}

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

    int hex_num = 9;

    int csr_value = READ_CSR(stvec);

    // printf("\nThe value of the register is: %d\n", csr_value);
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

void initialize_stvec()
{
    WRITE_CSR(stvec, (unsigned int)kernel_entry);
}

void handle_trap()
{
    unsigned int scause = READ_CSR(scause);
    unsigned int stval = READ_CSR(stval);
    unsigned int user_pc = READ_CSR(sepc);

    PANIC("unexpected trap scause=%x, stval=%x, sepc=%x\n", scause, stval, user_pc);
}