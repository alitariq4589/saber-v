#ifndef KERNEL_H
#define KERNEL_H

#define va_list __builtin_va_list
#define va_arg __builtin_va_arg
#define va_start __builtin_va_start
#define va_end __builtin_va_end

#define MAX_PROCS           10
#define UNASSIGNED          0
#define ASSIGNED            1
#define RUNNING             1
#define IDLE                0
#define PROCESS_STACK_SIZE  8192
#define PAGE_SIZE           4096

#define PANIC(fmt, ...)                                                                    \
    do                                                                                     \
    {                                                                                      \
        printf("\nKernel Panic! : %s : %d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
        while (1)                                                                          \
        {                                                                                  \
        }                                                                                  \
    } while (0)

#define READ_CSR(csr) ({ unsigned long csr_value; __asm__ __volatile__ ("csrr %0," #csr : "=r"(csr_value) ); csr_value; })
#define WRITE_CSR(reg, value) ({ unsigned long csr_value = value; __asm__ __volatile__ ("csrw " #reg ", %[reg_value]"::[reg_value] "r" (csr_value) ); })

#define READ_REGISTER(reg) ({ unsigned int reg_val; __asm__ __volatile__ ("add %0," #reg ",x0" : "=r"(reg_val) ); reg_val; })
#define WRITE_REGISTER(reg, value) ({ __asm__ __volatile__("li " #reg "," #value); })

extern char __bss[], __bss_end[], __stack_top[], __free_mem[], __free_mem_end[], __kernel_base[];

// This is unused yet
struct trap_frame
{
    unsigned int ra;
    unsigned int gp;
    unsigned int tp;
    unsigned int t0;
    unsigned int t1;
    unsigned int t2;
    unsigned int t3;
    unsigned int t4;
    unsigned int t5;
    unsigned int t6;
    unsigned int a0;
    unsigned int a1;
    unsigned int a2;
    unsigned int a3;
    unsigned int a4;
    unsigned int a5;
    unsigned int a6;
    unsigned int a7;
    unsigned int s0;
    unsigned int s1;
    unsigned int s2;
    unsigned int s3;
    unsigned int s4;
    unsigned int s5;
    unsigned int s6;
    unsigned int s7;
    unsigned int s8;
    unsigned int s9;
    unsigned int s10;
    unsigned int s11;
    unsigned int sp;
} __attribute__((packed));

struct sbiret
{
    long error;
    long value;
};

struct process
{
    int pid;
    unsigned long state;
    unsigned long allocation;
    unsigned long *sp;
    unsigned long *page_table;
    unsigned char stack[PROCESS_STACK_SIZE];
};

#endif
