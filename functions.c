#include "functions.h"

struct process *current_process;
struct process procs[MAX_PROCS];

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
  // printf("Unexpected trap scause=%x, stval=%x, sepc=%x\n", scause, stval, user_pc);
  // __asm__ __volatile__("ebreak\n");
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

/*This variable works as the pointer which slides forward as the memory is allocated and it prevents memory to be cobbered by other operations once it is allocated by one function*/
unsigned char *next_address = (unsigned char *)__free_mem;

/*Allocates the number of bytes of memory and returns the starting address of the alloacted memory. The memory which is used for this is defined in the linker script as __free_mem and is 64MB. The function sets 0s in all the addresses in the memory which are to be allocated*/
unsigned char *malloc(unsigned long number_of_bytes)
{
  unsigned char *starting_address = next_address;

  next_address += number_of_bytes;

  memset(starting_address, 0, number_of_bytes);

  return starting_address;
}

/*Allocates the contiguous number of pages in memory and returns the starting address of the first page. The memory which is used for this is defined in the linker script as __free_mem and is 64MB. The function sets 0s in all the addresses in the memory which are to be allocated*/
unsigned long *malloc_pages(unsigned long number_of_pages)
{
  unsigned long *starting_address = (unsigned long *)next_address;

  next_address += number_of_pages * PAGE_SIZE;

  memset((unsigned char *)starting_address, 0, number_of_pages * PAGE_SIZE);
  return starting_address;
}

/*Sets number of bytes from a starting address with the given value*/
void memset(unsigned char *start_addr, unsigned char set_value, unsigned long number_of_bytes_to_set)
{
  unsigned char *slider = (unsigned char *)start_addr;

  for (unsigned int i = 0; i < number_of_bytes_to_set; i++)
  {
    *slider = set_value;
    slider++;
  }
}

void *memcpy(void *dst, const void *src, unsigned long n)
{
  unsigned char *d = (unsigned char *)dst;
  unsigned char *s = (unsigned char *)src;

  while (n--)
  {
    *d++ = *s++;
  }
  return dst;
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

void user_entry(void)
{
  unsigned char spp = (unsigned char)0x100;
  unsigned long user_base = USER_BASE;
  __asm__ __volatile__(
      "csrc sstatus, %[SPP]\n"
      "csrw sepc, %[USERBASE]\n"
      "sret"
      :
      : [SPP] "r"(spp), [USERBASE] "r"(user_base));
}

struct process *create_process(const void *app_img, unsigned long img_size)
{
  printf("Size of the user binary: %d\n", img_size);
  unsigned long user_page_copy_size;
  unsigned long *new_page_table;
  struct process *new_process;
  int i;

  for (i = 0; i < MAX_PROCS; i++)
  {
    if (procs[i].allocation == UNASSIGNED)
    {
      new_process = &procs[i];
      break;
    }
  }

  if (i == MAX_PROCS)
    PANIC("\nNo free process to allocate!\n");

  new_process->pid = i;

  new_page_table = malloc_pages(1);
  // printf("\nDebug Statement\n");
  for (unsigned long addr = (unsigned long)__kernel_base; addr < (unsigned long)__free_mem_end; addr += PAGE_SIZE)
  {
    map_page(new_page_table, addr, addr, PAGE_V | PAGE_R | PAGE_W | PAGE_X);
  }

  // Start the stack from the last index
  unsigned long *sp = (unsigned long *)&new_process->stack[PROCESS_STACK_SIZE];

  // Allocate memory for 13 registers using decrement
  *--sp = 0;                         // s11
  *--sp = 0;                         // s10
  *--sp = 0;                         // s9
  *--sp = 0;                         // s8
  *--sp = 0;                         // s7
  *--sp = 0;                         // s6
  *--sp = 0;                         // s5
  *--sp = 0;                         // s4
  *--sp = 0;                         // s3
  *--sp = 0;                         // s2
  *--sp = 0;                         // s1
  *--sp = 0;                         // s0
  *--sp = (unsigned long)user_entry; // ra (program counter)

  // Mapping user pages
  for (unsigned long addr = 0; addr < img_size; addr += PAGE_SIZE)
  {
    unsigned long *physical_page = malloc_pages(1);

    // This checks if the data being copied is smaller than the page size. If so then dont copy bytes of entire page
    if (img_size - addr < PAGE_SIZE)
      user_page_copy_size = img_size - addr;
    else
      user_page_copy_size = PAGE_SIZE;

    /* Copy the user memory to the kernel space dynamic memory region for process to be executed. This is copied so that we dont end up using the same memory region of user image for multiple processes which can potentially overwrite the data of one process of same application with another process of same application */
    memcpy(physical_page, app_img + addr, user_page_copy_size);

    // Map the physical page to virtual page
    map_page(new_page_table, (unsigned long)USER_BASE + addr, (unsigned long)physical_page, PAGE_V | PAGE_R | PAGE_W | PAGE_X | PAGE_U);
  }
  new_process->sp = sp;
  new_process->page_table = new_page_table;
  new_process->allocation = ASSIGNED;
  new_process->state = IDLE;
  procs[i] = *new_process;

  return new_process;
}

void initialize_processes()
{
  struct process *idle_process;
  idle_process = create_process(0, 0);
  idle_process->pid = -1;
  current_process = idle_process;
}

void yield()
{
  printf("\nInside yield function\n");
  struct process *prev_process;
  struct process *next_process = current_process; // This is so that if the loop exits and no process is free, the next process is going to be the current process in that case (until a better idea comes to mind)
  // next_process = current_process;
  for (int i = 0; i < MAX_PROCS; i++)
  {
    if (procs[i].allocation == ASSIGNED && i != current_process->pid)
    {
      next_process = &procs[i];
      break;
    }
  }
  prev_process = current_process;
  current_process = next_process;
  unsigned long satp_value = (((unsigned long)next_process->page_table) >> 12) | 0x80000000;

  __asm__ __volatile__(
      "sfence.vma\n"
      "csrw satp, %[root_page_addr]\n"
      "sfence.vma\n"
      "csrw sscratch, %[sscratch]\n"
      :
      : [root_page_addr] "r"(satp_value), [sscratch] "r"(&next_process->stack[PROCESS_STACK_SIZE]));

  switch_context((unsigned long *)&prev_process->sp, (unsigned long *)&next_process->sp);
}

/* Maps the physical memory space to the virutal memory space. Essentially you give the physical address to map and also the virtual address to map to. One address represents a whole page since the next address will be page aligned so it is not needed to map every address lying in single page size memory range, just the start address of the page*/
void map_page(unsigned long *root_page_addr, unsigned long virt_addr, unsigned long phy_addr, unsigned int flags)
{

  unsigned long vpn1 = (virt_addr >> 22) & 0x3ff;
  unsigned long vpn0 = (virt_addr >> 12) & 0x3ff;

  unsigned long *second_page_addr; // This is the page which will be created if there is no second page already and the page 1 entry is invalid
  if ((root_page_addr[vpn1] & 0x1) != 1)
  {

    second_page_addr = malloc_pages(1);                                              // Allocate a page in the memory and get its starting address
    root_page_addr[vpn1] = (((unsigned long)second_page_addr >> 12) << 10) | PAGE_V; // Setting the valid bit and adding the address of second table
  }

  // When code reaches this point, a second page exists, so we get the address of it from the pte of page 1 and then store the physical address along with flags in it
  unsigned int *second_table = (unsigned int *)(((root_page_addr[vpn1]) >> 10) << 12);
  second_table[vpn0] = ((phy_addr >> 12) << 10) | flags | PAGE_V;
  if (DEBUG)
  {
    printf("Virtual Address: %x\n", virt_addr);
    printf("Physical Address: %x\n", phy_addr);
  }
}
