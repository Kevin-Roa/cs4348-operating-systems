#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "main.h"
#include "timer.h"
#include "cpu.h"
#include "memory.h"

int main(int argc, char *argv[])
{

  // Check for error on pipe creation
  if (pipe(cpuPipe) == -1 || pipe(memPipe) == -1)
  {
    pipeError();
  }

  // Fork the cpu process
  int cpuPid = fork();
  if (cpuPid < 0)
  {
    forkError();
  }
  // On the cpu process
  else if (cpuPid == 0)
  {
    // Create the cpu instance
    // If a timer count is given as an argument, use that
    // Otherwise, use set the timer count to -1
    // (-1 means the timer will never interrupt the cpu)
    // argv[2] is the max timer count
    if (argc >= 3)
    {
      Cpu cpu(std::stoi(argv[2]));
    }
    else
    {
      Cpu cpu(-1);
    }

    exit(0);
  }

  // Fork the memory process
  int memPid = fork();
  if (memPid < 0)
  {
    forkError();
  }
  // On the memory process
  else if (memPid == 0)
  {
    // Create the memory instance
    // Check if a file is given as an argument
    // If no file is given, exit
    // argv[1] is the file name
    if (argc >= 2)
    {
      Memory memory(argv[1]);
    }
    else
    {
      fileError();
    }
    exit(0);
  }

  // Wait for CPU to exit, then exit the program
  waitpid(cpuPid, nullptr, 0);

  return 0;
}