## Compile/Run
  ```
  g++ main.cpp -o Project1 -std=c++11

  ./Project1 <inputFile> <timerMax>
  ```
  Sample programs are provided in the `./samples` folder
___
## Problem Details
###### Taken directly from the problem document
- CPU
  - Registers:  PC, SP, IR, AC, X, Y.
  - Run the user program starting at address 0.
  - Instructions are fetched into the IR from memory.
  - Each instruction should be executed before the next instruction is fetched.
  - The user stack resides at the end of user memory (999) and grows down toward address 0.
  - The system stack resides at the end of system memory (1999) and grows down toward address 0.
  - There is no hardware enforcement of stack size.
  - The program ends when the End instruction is executed.
  - The user program cannot access system memory (exits with error message).
   
- Memory
  - 2000 integer entries, 0-999 for the user program, 1000-1999 for system code.
  - Rwo operations:
    - read(address): returns the value at the address
    - write(address, data): writes the data to the address
  - Memory will read an input file containing a program into its array, before any CPU fetching begins.
 
- Timer
  - A timer will interrupt the processor after every X instructions, where X is a command-line parameter.
  - The timer is always counting, whether in user mode or kernel mode.

- Interrupt processing
  - There are two forms of interrupts:  the timer and a system call using the int instruction.
    - In both cases the CPU should enter kernel mode.
    - The stack pointer should be switched to the system stack.
    - The SP and PC registers (and only these registers) should be saved o the system stack by the CPU.
      - The handler may save additional registers. 
  - A timer interrupt should cause execution at address 1000.
  - The int instruction should cause execution at address 1500.
  - The iret instruction returns from an interrupt.
  - Interrupts should be disabled during interrupt processing to avoi nested execution.
  - Interrupts are not allowed during system calls or vice versa.
___
## File Descriptions
- main.cpp:
  - Where the project initializes.
  - Creates both the CPU and Memory child processes.
  - Creates the pipes used between the processes.
  - Passes the input arguments to the relevant processes.
- main.h:
  - Declares the pipes.
  - Contains an Enum used when communicating between the processes.
  - Contains various error handling functions.
- cpu.h:
  - Declares the CPU class.
  - Declares/Implements the registers and instruction set.
  - Loops continuously until the program is completed.
  - Contains methods to communicate with the Memory process.
  - Outputs a debug file of the CPU's state while running.
- memory.h:
  - Declares the Memory class.
  - Declares/Implements the memory and read/write abilities.
  - Loops continuously until the program is completed.
  - Reads piped data from the CPU, pipes data back to the CPU.
  - Does actions based on the input pipe's contents.
- timer.h:
  - A simple class to handle the timer interrupts.
  - Counts the instructions and flips a boolean when the timer reaches its- max.
- samples/sample*.txt:
  - Various sample programs to test certain aspects of the project.
  - Uses the architecture defined in the CPU class.
  - Sample 1-4 are provided by the instructor.
  - Sample 5&6 are my own.
___
## Notes:
- If a timer value is not passed, the default value of -1 is used which stops timer interrupts all together.
- Sample programs are provided in the samples directory (./samples/sample*.txt).
- sample5.txt is the additional sample written for a grade.
- sample6.txt is not meant to be graded. It is simply a joke program that you may run for fun.
- The program generates a debug.txt file which logs the registers and any output during runtime.
  - This file is meant for debugging purposes, ignore it for grading.
  - Realistically this should be toggled with a flag argument but I as too lazy to implement it.
___
## Runtime Output
  Sample 1:
    ./Project1 ./samples/sample1.txt 30

    ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678910

  Sample 2:
    ./Project1 ./samples/sample2.txt 30

        ------
     /         \
    /   -*  -*  \
    |           |
    \   \____/  /
     \         /
        ------
    
  Sample 3:
    ./Project1 ./samples/sample3.txt 30

    A
    0
    A
    1
    A
    1
    A
    2
    A
    2
    A
    2
    A
    3
    A
    3
    A
    4
    A
    4

    ./Project1 ./samples/sample3.txt 10

    A
    0
    A
    2
    A
    4
    A
    7
    A
    9
    A
    12
    A
    14
    A
    17
    A
    19
    A
    22

  Sample 4:
    ./Project1 ./samples/sample4.txt 30

    999
    998
    999
    1997
    1996
    1997
    Memory violation: accessing system address 1000 in user mode.

  Sample 5:
    ./Project1 ./samples/sample5.txt 30

    Calc first twenty values of Fibonacci Sequence:
    0
    1
    1
    2
    3
    5
    8
    13
    21
    34
    55
    89
    144
    233
    377
    610
    987
    1597
    2584
    4181

  Sample 6: (NOT GRADED)
    ./Project1 ./samples/sample6.txt 30

    It's a suprise :)