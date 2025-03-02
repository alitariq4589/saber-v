#include "common.h"

extern void putchar(char ch);

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