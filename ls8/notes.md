##### Objective
Write an emulator for an 8-bit computer with 8-bit memory addressing

##### Specifications
- has a total of 256 bytes (1 char = 1 byte) of memory
- can only compute values up to 255

##### Example
```
# print8.ls8: Print the number 8 on the screen

10000010 # LDI R0,8
00000000
00001000
01000111 # PRN R0
00000000
00000001 # HLT
```

##### Files
- examples - sample programs to run on emulator
- cpu.c - represents CPU for 8-bit computer (ls8.c)
  - TODO: initialize CPU struct
  - TODO: refactor cpu_load, read file after cpu_init()
  - TODO: additional ALU options
  - TODO: complete cpu_run()
- cpu.h - header file for cpu.c
  - TODO: write struct cpu
  - TODO: add ALU options as created
  - TODO: add binary literals
- ls8.c - represents 8-bit computer

##### Registers
IR: instruction register (copy of current instructions)
MAR: memory address register (address reading or writing to)
MDR: memory data register (holds value to write or just read)
FL: flags
R4 - PC: program counter (address of the currently executing instruction)
R5 - IM: interrupt mask
R6 - IS: interrupt status
R7 - SP: stack pointer (head of stack, otherwise 0xF4)