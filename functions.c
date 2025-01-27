
#include "functions.h"

const char *exception_cause[] = {
    "Instruction address misaligned",
    "Instruction access fault",
    "Illegal instruction",
    "Breakpoint",
    "Load address misaligned",
    "Load access fault",
    "Store/AMO address misaligned",
    "Store/AMO access fault",
    "Instruction page fault",
    "Environment call from U-mode",
    "Environment call from S-mode",
    "Reserved",
    "Reserved",
    "Instruction page fault",
    "Load page fault",
    "Store/AMO page fault",
    "Reserved",
    "Reserved",
    "Software check",
    "Hardware error",

    "Reserved",
    "Supervisor software interrupt",
    "Reserved",
    "Reserved",
    "Reserved",
    "Supervisor timer interrupt",
    "Reserved",
    "Reserved",
    "Reserved",
    "Supervisor external interrupt",
    "Reserved",
    "Reserved",
    "Reserved",
    "Counter-overflow interrupt"};

/*SBI function calls*/
struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4,
                       long arg5, long fid, long eid)
{
  register unsigned long a0 __asm__("a0") = arg0;
  register unsigned long a1 __asm__("a1") = arg1;
  register unsigned long a2 __asm__("a2") = arg2;
  register unsigned long a3 __asm__("a3") = arg3;
  register unsigned long a4 __asm__("a4") = arg4;
  register unsigned long a5 __asm__("a5") = arg5;
  register unsigned long a6 __asm__("a6") = fid;
  register unsigned long a7 __asm__("a7") = eid;

  __asm__ __volatile__("ecall"
                       : "=r"(a0), "=r"(a1)
                       : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                         "r"(a6), "r"(a7)
                       : "memory");
  return (struct sbiret){.error = a0, .value = a1};
}

/*Uses SBI function calls to print characters to terminal*/
void putchar(char ch)
{
  sbi_call(ch, 0, 0, 0, 0, 0, 0, 1 /* Console Putchar */);
}

/*Uses the putchar function to mimic conventional printf() function*/
void printf(char *fmt, ...)
{

  va_list args;
  va_start(args, fmt);

  char *str;

  unsigned long number_to_print;

  while (*fmt)
  {
    if (*fmt == '%')
    {

      fmt++;

      // The string print
      if (*fmt == 's')
      {
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
        unsigned long number = va_arg(args, unsigned long);
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
        putchar('0');
        putchar('x');
        unsigned long number = va_arg(args, unsigned long);
        for (int i = 7; i >= 0; i--)
        {
          unsigned long hex_digit = (number >> (i * 4)) & 0xf;
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

/*Title at the QEMU screen - Mimics the splash screen of bios*/
void print_title(void)
{
  printf("\n\n\n");
  printf("███████╗ █████╗ ██████╗ ███████╗██████╗               ██╗   ██╗\n");
  printf("██╔════╝██╔══██╗██╔══██╗██╔════╝██╔══██╗              ██║   ██║\n");
  printf("███████╗███████║██████╔╝█████╗  ██████╔╝    █████╗    ██║   ██║\n");
  printf("╚════██║██╔══██║██╔══██╗██╔══╝  ██╔══██╗    ╚════╝    ╚██╗ ██╔╝\n");
  printf("███████║██║  ██║██████╔╝███████╗██║  ██║               ╚████╔╝ \n");
  printf("╚══════╝╚═╝  ╚═╝╚═════╝ ╚══════╝╚═╝  ╚═╝                ╚═══╝  \n");
  printf("\n\n\n");

  printf("                    WELCOME TO SABER-V!              \n");
  printf("\n");
  printf("         A very lightweight Open-source RISC-V OS          \n");
  printf("\n\n");
}

/*Write the address of the trap handler in stvec*/
void initialize_stvec()
{
  WRITE_CSR(stvec, (unsigned int)kernel_entry);
}

/*Function which is called in trap handler - Prints the values of registers for information of exception*/
void handle_trap()
{
  unsigned long scause = READ_CSR(scause);
  unsigned long stval = READ_CSR(stval);
  unsigned long user_pc = READ_CSR(sepc);

  unsigned long exception_type_index = (scause & 0x1f) + ((0x80000000 & scause) * 20);


  printf("\n\n      === Exception Encountered ===    \n\n");
  printf("\nException Type: %s!\n", exception_cause[exception_type_index]);
  PANIC("Unexpected trap scause=%x, stval=%x, sepc=%x\n", scause, stval, user_pc);
}

/*This is the trap handler*/
__attribute__((naked))
__attribute__((aligned(4))) void
kernel_entry(void)
{
  __asm__ __volatile__(
      "csrw sscratch, sp\n"
      "addi sp, sp, -4 * 31\n"
      "sw ra,  4 * 0(sp)\n"
      "sw gp,  4 * 1(sp)\n"
      "sw tp,  4 * 2(sp)\n"
      "sw t0,  4 * 3(sp)\n"
      "sw t1,  4 * 4(sp)\n"
      "sw t2,  4 * 5(sp)\n"
      "sw t3,  4 * 6(sp)\n"
      "sw t4,  4 * 7(sp)\n"
      "sw t5,  4 * 8(sp)\n"
      "sw t6,  4 * 9(sp)\n"
      "sw a0,  4 * 10(sp)\n"
      "sw a1,  4 * 11(sp)\n"
      "sw a2,  4 * 12(sp)\n"
      "sw a3,  4 * 13(sp)\n"
      "sw a4,  4 * 14(sp)\n"
      "sw a5,  4 * 15(sp)\n"
      "sw a6,  4 * 16(sp)\n"
      "sw a7,  4 * 17(sp)\n"
      "sw s0,  4 * 18(sp)\n"
      "sw s1,  4 * 19(sp)\n"
      "sw s2,  4 * 20(sp)\n"
      "sw s3,  4 * 21(sp)\n"
      "sw s4,  4 * 22(sp)\n"
      "sw s5,  4 * 23(sp)\n"
      "sw s6,  4 * 24(sp)\n"
      "sw s7,  4 * 25(sp)\n"
      "sw s8,  4 * 26(sp)\n"
      "sw s9,  4 * 27(sp)\n"
      "sw s10, 4 * 28(sp)\n"
      "sw s11, 4 * 29(sp)\n"

      "csrr a0, sscratch\n"
      "sw a0, 4 * 30(sp)\n"

      "mv a0, sp\n"
      "call handle_trap\n"

      "lw ra,  4 * 0(sp)\n"
      "lw gp,  4 * 1(sp)\n"
      "lw tp,  4 * 2(sp)\n"
      "lw t0,  4 * 3(sp)\n"
      "lw t1,  4 * 4(sp)\n"
      "lw t2,  4 * 5(sp)\n"
      "lw t3,  4 * 6(sp)\n"
      "lw t4,  4 * 7(sp)\n"
      "lw t5,  4 * 8(sp)\n"
      "lw t6,  4 * 9(sp)\n"
      "lw a0,  4 * 10(sp)\n"
      "lw a1,  4 * 11(sp)\n"
      "lw a2,  4 * 12(sp)\n"
      "lw a3,  4 * 13(sp)\n"
      "lw a4,  4 * 14(sp)\n"
      "lw a5,  4 * 15(sp)\n"
      "lw a6,  4 * 16(sp)\n"
      "lw a7,  4 * 17(sp)\n"
      "lw s0,  4 * 18(sp)\n"
      "lw s1,  4 * 19(sp)\n"
      "lw s2,  4 * 20(sp)\n"
      "lw s3,  4 * 21(sp)\n"
      "lw s4,  4 * 22(sp)\n"
      "lw s5,  4 * 23(sp)\n"
      "lw s6,  4 * 24(sp)\n"
      "lw s7,  4 * 25(sp)\n"
      "lw s8,  4 * 26(sp)\n"
      "lw s9,  4 * 27(sp)\n"
      "lw s10, 4 * 28(sp)\n"
      "lw s11, 4 * 29(sp)\n"
      "lw sp,  4 * 30(sp)\n"
      "sret\n");
}

/*Allocates the number of bytes of memory and returns the starting address of the alloacted memory. The memory which is used for this is defined in the linker script as __free_mem and is 64MB. The function sets 0s in all the addresses in the memory*/
unsigned char *malloc(unsigned long number_of_bytes)
{
  unsigned char *next_address = (unsigned char *) __free_mem;
  unsigned char *starting_address = next_address;

  next_address += number_of_bytes;

  for (unsigned int i = 0; i < number_of_bytes - 1; i++)
  {
    memset(starting_address, 0, number_of_bytes);
  }
  return starting_address;
}

/*Sets number of bytes from a starting address with the given value*/
void memset(unsigned char *start_addr, unsigned char set_value, unsigned long number_of_bytes_to_set)
{
  unsigned char *slider = start_addr;

  for (unsigned char i = 0; i < number_of_bytes_to_set; i++)
  {
    *slider = set_value;
    slider++;
  }
}

__attribute__((naked))

void
switch_context(unsigned long *prev_process_stack_pointer, unsigned long *next_process_stack_pointer)
{

  __asm__ __volatile__(
      "addi sp, sp, -13*4\n"

      // Save Callee saved registers
      "sw ra, 0(sp)\n"
      "sw s0, 4(sp)\n"
      "sw s1, 8(sp)\n"
      "sw s2, 12(sp)\n"
      "sw s3, 16(sp)\n"
      "sw s4, 20(sp)\n"
      "sw s5, 24(sp)\n"
      "sw s6, 28(sp)\n"
      "sw s7, 32(sp)\n"
      "sw s8, 36(sp)\n"
      "sw s9, 40(sp)\n"
      "sw s10, 44(sp)\n"
      "sw s11, 48(sp)\n"
      

      // swap the stackpointer and save the previous one
      "sw sp, 0(a0)\n"
      "lw sp, 0(a1)\n"

      // Get the registers from new stackpointer
      "lw ra, 0(sp)\n"
      "lw s0, 4(sp)\n"
      "lw s1, 8(sp)\n"
      "lw s2, 12(sp)\n"
      "lw s3, 16(sp)\n"
      "lw s4, 20(sp)\n"
      "lw s5, 24(sp)\n"
      "lw s6, 28(sp)\n"
      "lw s7, 32(sp)\n"
      "lw s8, 36(sp)\n"
      "lw s9, 40(sp)\n"
      "lw s10, 44(sp)\n"
      "lw s11, 48(sp)\n"
      "addi sp, sp, 13*4\n"
      "ret\n"

  );
}

struct process procs[MAX_PROCS];

struct process *create_process(unsigned long proc_entry_point)
{
  struct process *new_process;
  int i;
  for (i = 0; i < MAX_PROCS; i++)
  {
    if (procs[i].state == UNASSIGNED)
    {
      new_process = &procs[i];
      break;
    }
  }

  if (i == MAX_PROCS)
    PANIC("\nNo free process to allocate!\n");

  new_process->pid = i;

  //Start the stack from the last index
  unsigned long *sp = (unsigned long *) &new_process->stack[PROCESS_STACK_SIZE];


  //Allocate memory for 13 registers using decrement
  *--sp = 0;                // s11
  *--sp = 0;                // s10
  *--sp = 0;                // s9
  *--sp = 0;                // s8
  *--sp = 0;                // s7
  *--sp = 0;                // s6
  *--sp = 0;                // s5
  *--sp = 0;                // s4
  *--sp = 0;                // s3
  *--sp = 0;                // s2
  *--sp = 0;                // s1
  *--sp = 0;                // s0
  *--sp = proc_entry_point; // ra (program counter)



  new_process->sp = sp;
  new_process->state = IDLE;
  new_process->state = ASSIGNED;
  return new_process;
}

void yield(void){

}


