class Cpu
{
private:
  int pc, sp, ir, ac, x, y;
  bool isKernelMode, loaded;
  Timer *timer;

  std::ofstream debugFile;

public:
  Cpu(int maxTimerCount)
  {
    // Initialize registers
    pc = 0;   // Program counter
    sp = 999; // Stack pointer
    ir = 0;   // Instruction register
    ac = 0;   // Accumulator
    x = 0;    // X register
    y = 0;    // Y register

    // Initialize to normal mode
    isKernelMode = false;

    // Set loaded to false to wait for memory to load
    loaded = false;

    // Initialize timer
    timer = new Timer(maxTimerCount);

    // Close unused pipes
    close(cpuPipe[1]);
    close(memPipe[0]);

    // Seed random number generator
    srand(time(NULL));

    runtimeLoop();
  }

private:
  void runtimeLoop()
  {
    // Wait to recieve signal that memory is loaded
    if (read(cpuPipe[0], &loaded, sizeof(loaded)) == -1)
    {
      readError();
    }

    // Create debug file
    // Contains list of CPU state during execution
    debugFile.open("debug.txt");
    writeDebug(debugFile);

    while (loaded)
    {

      // Tick the timer
      // If the timer should interrupt, set ir to the interrupt instruction
      timer->tick();
      if (timer->shouldInterrupt)
      {
        // Inst 29 = interrupt
        ir = 29;
      }
      // Else, set ir to the next instruction
      else
      {
        // Read from memory
        ir = readFromMemory(pc);
        pc++;
      }

      writeDebug(debugFile);

      switch (ir)
      {
      case 1:
        loadValue();
        break;
      case 2:
        loadAddress();
        break;
      case 3:
        loadIndAddress();
        break;
      case 4:
        loadIdxAddressX();
        break;
      case 5:
        loadIdxAddressY();
        break;
      case 6:
        loadSpX();
        break;
      case 7:
        storeAddress();
        break;
      case 8:
        get();
        break;
      case 9:
        put();
        break;
      case 10:
        addX();
        break;
      case 11:
        addY();
        break;
      case 12:
        subX();
        break;
      case 13:
        subY();
        break;
      case 14:
        copyToX();
        break;
      case 15:
        copyFromX();
        break;
      case 16:
        copyToY();
        break;
      case 17:
        copyFromY();
        break;
      case 18:
        copyToSp();
        break;
      case 19:
        copyFromSp();
        break;
      case 20:
        jump();
        break;
      case 21:
        jumpIfEqual();
        break;
      case 22:
        jumpIfNotEqual();
        break;
      case 23:
        callAddress();
        break;
      case 24:
        ret();
        break;
      case 25:
        incX();
        break;
      case 26:
        decX();
        break;
      case 27:
        push();
        break;
      case 28:
        pop();
        break;
      case 29:
        interrupt();
        break;
      case 30:
        retInterrupt();
        break;
      case 50:
        end();
        break;
      default:
      {
        std::cout << "Error: Invalid instruction: " << ir << "." << std::endl;
        exit(1);
      }
      }
    }
    debugFile.close();
    // print the registers
  }

  void writeDebug(std::ofstream &stream)
  {
    stream << "PC: " << pc << " ";
    stream << "IR: " << ir << " ";
    stream << "SP: " << sp << " ";
    stream << "AC: " << ac << " ";
    stream << "X: " << x << " ";
    stream << "Y: " << y << " ";
    stream << "KM: " << isKernelMode << std::endl;
  }

  int readFromMemory(int addr)
  {
    MemoryFuncType func = READ;
    int data;

    // Write the function type to pipe
    if (write(memPipe[1], &func, sizeof(func)) == -1)
    {
      writeError();
    }

    // Write the address to pipe
    if (write(memPipe[1], &addr, sizeof(addr)) == -1)
    {
      writeError();
    }

    // Read the value from pipe
    if (read(cpuPipe[0], &data, sizeof(data)) == -1)
    {
      readError();
    }

    return data;
  }

  void writeToMemory(int addr, int val)
  {
    MemoryFuncType func = WRITE;

    // Write the function type to pipe
    if (write(memPipe[1], &func, sizeof(func)) == -1)
    {
      writeError();
    }

    // Write the address to pipe
    if (write(memPipe[1], &addr, sizeof(addr)) == -1)
    {
      writeError();
    }

    // Write the value to pipe
    if (write(memPipe[1], &val, sizeof(val)) == -1)
    {
      writeError();
    }
  }

  void checkAccessViolation(int addr)
  {
    if (!isKernelMode && addr >= 1000)
    {
      std::cout << "Memory violation: accessing system address " << addr << " in user mode." << std::endl;
      exit(1);
    }
  }

  // Inst 1
  // Load the value into the AC
  void loadValue()
  {
    ac = readFromMemory(pc);
    pc++;
  }

  // Inst 2
  // Load the value at the address into the AC
  void loadAddress()
  {
    int addr = readFromMemory(pc);

    checkAccessViolation(addr);

    ac = readFromMemory(addr);
    pc++;
  }

  // Inst 3
  // Load the value from the address found in the given address into the AC
  void loadIndAddress()
  {
    int addr = readFromMemory(pc);

    checkAccessViolation(addr);

    addr = readFromMemory(addr);
    ac = readFromMemory(addr);
    pc++;
  }

  // Inst 4
  // Load the value from the given address + X into the AC
  void loadIdxAddressX()
  {
    int addr = readFromMemory(pc);

    checkAccessViolation(addr + x);

    ac = readFromMemory(addr + x);
    pc++;
  }

  // Inst 5
  // Load the value from the given address + Y into the AC
  void loadIdxAddressY()
  {
    int addr = readFromMemory(pc);

    checkAccessViolation(addr + y);

    ac = readFromMemory(addr + y);
    pc++;
  }

  // Inst 6
  // Load the value from Sp + X into the AC
  void loadSpX()
  {
    checkAccessViolation(sp + x);

    ac = readFromMemory(sp + x + 1);
  }

  // Inst 7
  // Store the value from the AC into the address
  void storeAddress()
  {
    int addr = readFromMemory(pc);

    checkAccessViolation(addr);

    writeToMemory(addr, ac);
    pc++;
  }

  // Inst 8
  // Get a random int between 1 and 100 into the AC
  void get()
  {
    ac = rand() % 100 + 1;
  }

  // Inst 9
  // Write the value in the AC to the screen
  // Port = 1: Write int
  // Port = 2: Write char
  void put()
  {
    int port = readFromMemory(pc);
    switch (port)
    {
    case 1:
      std::cout << (int)ac;
      if (ac != 10)
        debugFile << (int)ac << std::endl;
      break;
    case 2:
      std::cout << (char)ac;
      if (ac != 10)
        debugFile << (char)ac << std::endl;
      break;
    default:
      std::cout << "Error: Invalid port: " << port << "." << std::endl;
      exit(1);
    }
    pc++;
  }

  // Inst 10
  // Add the value at X into the AC
  void addX()
  {
    ac += x;
  }

  // Inst 11
  // Add the value at Y into the AC
  void addY()
  {
    ac += y;
  }

  // Inst 12
  // Subtract the value at X from the AC
  void subX()
  {
    ac -= x;
  }

  // Inst 13
  // Subtract the value at Y from the AC
  void subY()
  {
    ac -= y;
  }

  // Inst 14
  // Copy the value in AC to X
  void copyToX()
  {
    x = ac;
  }

  // Inst 15
  // Copy the value in AC to Y
  void copyToY()
  {
    y = ac;
  }

  // Inst 16
  // Copy the value in X to AC
  void copyFromX()
  {
    ac = x;
  }

  // Inst 17
  // Copy the value in Y to AC
  void copyFromY()
  {
    ac = y;
  }

  // Inst 18
  // Copy the value in AC to Sp
  void copyToSp()
  {
    sp = ac;
  }

  // Inst 19
  // Copy the value in Sp to AC
  void copyFromSp()
  {
    ac = sp;
  }

  // Inst 20
  // Jump to the address
  void jump()
  {
    int addr = readFromMemory(pc);
    pc = addr;
  }

  // Inst 21
  // Jump to the address if the AC is zero
  void jumpIfEqual()
  {
    if (ac == 0)
    {
      int addr = readFromMemory(pc);
      pc = addr;
    }
    else
    {
      pc++;
    }
  }

  // Inst 22
  // Jump to the address if the AC is not zero
  void jumpIfNotEqual()
  {
    if (ac != 0)
    {
      int addr = readFromMemory(pc);
      pc = addr;
    }
    else
    {
      pc++;
    }
  }

  // Inst 23
  // Push return address to stack, jump to address
  void callAddress()
  {
    int addr = readFromMemory(pc);
    pc++;
    writeToMemory(sp, pc);
    sp--;
    pc = addr;
  }

  // Inst 24
  // Pop return address from stack, jump to address
  void ret()
  {
    sp++;
    pc = readFromMemory(sp);
  }

  // Inst 25
  // Increment value in X
  void incX()
  {
    x++;
  }

  // Inst 26
  // Decrement value in X
  void decX()
  {
    x--;
  }

  // Inst 27
  // Push AC to stack
  void push()
  {
    writeToMemory(sp, ac);
    sp--;
  }

  // Inst 28
  // Pop from stack into AC
  void pop()
  {
    sp++;
    ac = readFromMemory(sp);
  }

  // Inst 29
  // Perform a syscall
  void interrupt()
  {
    if (!isKernelMode)
    {
      // Enable kernel mode
      isKernelMode = true;

      // Save registers to system stack
      writeToMemory(1999, sp);
      writeToMemory(1998, pc);

      // Move SP to system stack
      sp = 1997;

      // If the interrupt is from the timer, move PC to 1000
      if (timer->shouldInterrupt)
      {
        pc = 1000;
      }
      // Else, the interrupt is from the program, move PC to 1500
      else
      {
        pc = 1500;
      }
    }
  }

  // Inst 30
  // Return from syscall
  void retInterrupt()
  {
    // Retrieve SP and PC from system stack
    sp = readFromMemory(1999);
    pc = readFromMemory(1998);

    // Disable kernel mode
    isKernelMode = false;
  }

  // Inst 50
  void end()
  {
    loaded = false;
  }
};