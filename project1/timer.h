class Timer
{
private:
  int maxCount;
  int count;

public:
  bool shouldInterrupt;

public:
  Timer(int maxCount)
  {
    this->maxCount = maxCount;
    count = 0;
    shouldInterrupt = false;
  }

  void tick()
  {
    count++;

    // If the maxCount is -1, the timer will never interrupt
    if (count == maxCount)
    {
      count = 0;
      shouldInterrupt = true;
    }
    else
    {
      shouldInterrupt = false;
    }
  }
};