#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_LEN 6

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, int argc, char **argv)
{
  // ----------------------------
  // char data[DATA_LEN] = {
  //     // From print8.ls8
  //     0b10000010, // LDI R0,8
  //     0b00000000,
  //     0b00001000,
  //     0b01000111, // PRN R0
  //     0b00000000,
  //     0b00000001 // HLT
  // };

  // int address = 0;
  // for (int i = 0; i < DATA_LEN; i++)
  // {
  //   cpu->ram[address++] = data[i];
  // }

  // cpu->registers[3] = cpu->PC;
  // ---------------------------

  // declare file pointer
  FILE *fp;

  // holder for current instruction
  char instruction[256];

  // if anything but 2 args in command line, exit
  if (argc != 2)
  {
    printf("Usage: cpu filename\n");
    exit(EXIT_FAILURE);
  }

  // open file
  // printf("argv[1]: %s\n", argv[1]);
  fp = fopen(argv[1], "r");

  // if file pointer is null, exit
  if (fp == NULL)
  {
    printf("Error opening file %s\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  int address = 0;

  while (fgets(instruction, 256, fp) != NULL)
  {
    char *ptr;
    unsigned char value = strtoul(instruction, &ptr, 2);
    // printf("instruction: %x\n", value);
    if (instruction != ptr)
    {
      cpu->ram[address++] = value;
      // printf("%s\n", instruction);
    }
  }

  fclose(fp);

  // initialize PC
  cpu->registers[4] = cpu->PC;
}

/**
 * Read ram inside struct cpu
 **/
unsigned char cpu_ram_read(struct cpu *cpu, unsigned char index)
{
  return cpu->ram[index];
};

/**
 * Write to ram inside struct cpu
 **/
void cpu_ram_write(struct cpu *cpu, unsigned char index, unsigned char value)
{
  cpu->ram[index] = value;
};

/**
 * Arithmetic logic unit, ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op)
  {
  case ALU_MUL:
    cpu->registers[regA] = (cpu->registers[regA] * cpu->registers[regB]) & 0xFF;
    break;
  case ALU_ADD:
    cpu->registers[regA] = (cpu->registers[regA] + cpu->registers[regB]) & 0xFF;
    // TODO: implement more ALU ops
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction

  while (running)
  {
    // Get the value of the current instruction
    // (in address PC).
    unsigned char currentPC = cpu->registers[4];
    unsigned char instruction = cpu->ram[currentPC];
    // printf("PC: %d - instruction: %x\n", currentPC, instruction);
    // break;

    // Figure out how many operands this next instruction requires
    // AABCDDDD ---> AA is number of operands
    int countOperands = instruction >> 6;
    // printf("# operands: %d\n", countOperands);

    // Get the appropriate value(s) of the operands following this instruction
    int operandA = 0;
    int operandB = 0;

    if (countOperands > 0)
    {
      operandA = cpu->ram[currentPC + 1];
    }

    if (countOperands == 2)
    {
      operandB = cpu->ram[currentPC + 2];
    }
    // switch() over it to decide on a course of action.
    // Do whatever the instruction should do according to the spec.
    switch (instruction)
    {
    case ADD:
      alu(cpu, ALU_ADD, operandA, operandB);
      break;

    // Calls a subroutine at the address stored in the register
    // The address of the instruction directly after CALL is pushed onto the stack. This allows us to return to where we left off when the subroutine finishes executing.
    // The PC is set to the address stored in the given register. We jump to that location in RAM and execute the first instruction in the subroutine.
    // The PC can move forward or backwards from its current location.
    case CALL:
      // increment SP
      cpu->registers[7]--;

      // push return location to SP
      cpu->registers[7] = (currentPC) + countOperands;

      // move to instruction from value in register
      cpu->PC = cpu->registers[operandA] - countOperands - 1;
      cpu->registers[4] = cpu->PC;

      break;

    // exit process
    case HLT:
      running = 0;
      break;

    // Set value of register (operandA) to integer (operandB)
    case LDI:
      cpu->registers[operandA] = operandB;
      break;

    // Multiply the values in two registers together and store the result in registerA.
    // This is an instruction handled by the ALU.
    case MUL:
      alu(cpu, ALU_MUL, operandA, operandB);
      break;

    // Pop the value at the top of the stack into the given register.
    // Copy the value from the address pointed to by SP to the given register.
    // Increment SP.
    case POP:
      cpu->registers[operandA] = cpu->ram[cpu->registers[7]];
      cpu->registers[7]++;
      break;

    // print value stored in given register to console
    case PRN:
      printf("%d\n", cpu->registers[operandA]);
      break;

    // Push the value in the given register on the stack.
    // 1. Decrement the SP.
    // 2. Copy the value in the given register to the address pointed to by SP.
    case PUSH:
      cpu->registers[7]--;
      cpu->ram[cpu->registers[7]] = cpu->registers[operandA];
      break;

    // Return from subroutine.
    // Pop the value from the top of the stack and store it in the PC.
    case RET:
      cpu->PC = cpu->registers[7];
      cpu->registers[4] = cpu->PC;
      cpu->registers[7]++;
      break;

    default:
      printf("ERROR: Invalid instruction");
      running = 0;
      break;
    }

    // Move the PC to the next instruction.
    cpu->PC = (cpu->PC) + countOperands + 1;
    cpu->registers[4] = cpu->PC;
  }
}

/**
 * Initialize a CPU struct
 * PC, registers, and RAM should be cleared to zero
 * 
 * R0-R6 are cleared to 0.
 * R7 is set to 0xF4.
 * PC and FL registers are cleared to 0.
 * RAM is cleared to 0.
 */
void cpu_init(struct cpu *cpu)
{
  // Initialize the PC to 0
  cpu->PC = 0;

  // ram and registers to 0
  memset(cpu->ram, 0, sizeof(cpu->ram));
  memset(cpu->registers, 0, sizeof(cpu->registers));

  // initialize special registers
  // R7 --> stack pointer (SP)
  // SP points at the value at the top of the stack
  // (most recently pushed), or at address F4 if the
  // stack is empty.
  cpu->registers[7] = 0xF4;
}
