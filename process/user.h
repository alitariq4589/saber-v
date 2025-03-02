#include "../common.h"

void putchar(char ch);
__attribute__((noreturn)) void exit(void);
__attribute__((section(".text.start"))) __attribute__((naked)) void start(void);