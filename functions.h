
// #ifndef FUNCTION_H
// #define FUNCTION_H
#include "kernel.h"

// struct process procs[MAX_PROCS];

void putchar(char ch);
void printf(char *fmt, ...);
void print_title(void);
void initialize_stvec();
void handle_trap();
void kernel_entry(void);
void memset(unsigned char *start_addr, unsigned char set_value, unsigned long number_of_bytes_to_set);
unsigned char *malloc(unsigned long number_of_bytes);
void switch_context(unsigned long *prev_process_stack_pointer, unsigned long *next_process_stack_pointer);
struct process *create_process(unsigned long proc_entry_point);
void yield();
void start_processes(unsigned long *process_entry_point, struct process *process_structure);
void map_page(unsigned long *root_page_addr, unsigned long virt_addr, unsigned long phy_addr, unsigned int flags);
// #endif
