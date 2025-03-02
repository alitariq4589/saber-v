#include "user.h"
#include "../common.h"
void main(void)
{
  printf("AB");
  for (;;)
  {
    __asm__ __volatile__ ("nop\n");
    __asm__ __volatile__ ("nop\n");
    __asm__ __volatile__ ("nop\n");
    __asm__ __volatile__ ("nop\n");
  }
}