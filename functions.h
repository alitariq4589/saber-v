void putchar(char ch);
void printf(char *fmt, ...);
void print_title(void);
void initialize_stvec();
void handle_trap();
void kernel_entry(void);
void memset(unsigned char *start_addr, unsigned char set_value, unsigned long number_of_bytes_to_set);
unsigned char *malloc(unsigned long number_of_bytes);