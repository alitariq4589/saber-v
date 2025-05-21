/*System Call numbers*/
#define SYS_PUTCHAR 0
#define SYS_EXIT    1


/*For printf*/
#define va_list __builtin_va_list
#define va_arg __builtin_va_arg
#define va_start __builtin_va_start
#define va_end __builtin_va_end

/*Common prototypes*/
void printf(char *fmt, ...);