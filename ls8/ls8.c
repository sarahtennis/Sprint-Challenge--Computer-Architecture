#include <stdio.h>
#include "cpu.h"
#include "cpu.c"

/**
 * Main
 */
int main(int argc, char **argv)
{
  struct cpu cpu;

  // clear ram & registers, initialize R7 to SP
  cpu_init(&cpu);

  // load program into RAM
  cpu_load(&cpu, argc, argv);

  cpu_run(&cpu);

  return 0;
}