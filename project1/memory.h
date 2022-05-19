class Memory
{
private:
  // 0-999: user stack
  // 1000-1999: system stack
  int *memory;

public:
  Memory(char *inputFile)
  {
    // Initialize memory
    memory = new int[2000]();

    // Close unused pipes
    close(cpuPipe[0]);
    close(memPipe[1]);

    // Read input file
    parseInput(inputFile);

    // Send signal to cpu to start now that memory is loaded
    bool loaded = true;
    if (write(cpuPipe[1], &loaded, sizeof(loaded)) == -1)
    {
      writeError();
    }

    runtimeLoop();
  }

private:
  void parseInput(char *inputFile)
  {
    // Open the file
    std::ifstream file(inputFile);
    std::string line;
    int index = 0;

    // Read the file line by line
    while (std::getline(file, line))
    {
      std::string inst;
      // Read the characters in the line
      for (int i = 0; i < line.length(); i++)
      {
        // Move current memory index to new location
        if (line[0] == '.')
        {
          index = std::stoi(line.substr(1, line.length()));
        }
        // Append digit to inst
        else if (isdigit(line[i]))
        {
          inst += line[i];
        }
        // If current character is not a digit or a period, go to next line
        else
        {
          break;
        }
      }

      // Add inst to memory
      if (inst != "")
      {
        memory[index] = std::stoi(inst);
        index++;
      }
    }
  }

  void runtimeLoop()
  {
    MemoryFuncType func;
    int addr;

    while (true)
    {
      // Read the function type from pipe
      if (read(memPipe[0], &func, sizeof(func)) == -1)
      {
        readError();
      }

      // Read the address from pipe
      if (read(memPipe[0], &addr, sizeof(addr)) == -1)
      {
        readError();
      }

      switch (func)
      {
      case READ:
        readData(addr);
        break;
      case WRITE:
        writeData(addr);
        break;
      }
    }
  }

  void readData(int addr)
  {
    int val = memory[addr];

    if (write(cpuPipe[1], &val, sizeof(val)) == -1)
    {
      writeError();
    }
  }
  void writeData(int addr)
  {
    int data;

    // Read the data from pipe
    if (read(memPipe[0], &data, sizeof(data)) == -1)
    {
      readError();
    }

    memory[addr] = data;
  }

public:
  // (DEBUG): Prints a portion of the memory
  void dump(int start = 0, int end = 2000)
  {
    for (int i = start; i < end; i++)
    {
      std::cout << "[" << i << "] => " << memory[i] << std::endl;
    }
  }
};