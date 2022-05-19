
int cpuPipe[2], memPipe[2];

enum MemoryFuncType
{
  READ,
  WRITE,
};

void readError()
{
  std::cout << "Read Error" << std::endl;
  exit(1);
}

void writeError()
{
  std::cout << "Write Error" << std::endl;
  exit(1);
}

void pipeError()
{
  std::cout << "Pipe Error" << std::endl;
  exit(1);
}

void forkError()
{
  std::cout << "Fork Error" << std::endl;
  exit(1);
}

void fileError()
{
  std::cout << "File Error" << std::endl;
  exit(1);
}