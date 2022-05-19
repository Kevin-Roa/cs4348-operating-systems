class concession
{
private:
  int id; // Unique ID for each concession stand worker

public:
  void run()
  {
    // Assign a unique id to this concession stand worker
    sem_wait(&td.concMutex1);
    id = td.concCount;
    td.concCount++;
    sem_post(&td.concMutex1);
    printf("Concession stand worker %d created.\n", id);

    // Loop over customers in queue
    while (true)
    {
      // Get next customer id
      sem_wait(&td.custAtConc);
      sem_wait(&td.concMutex2);
      int custId = td.concQ.front();
      td.concQ.pop();
      sem_post(&td.concMutex2);

      // Get customer's food choice, serve food
      string food = td.custFood[custId];
      printf("Concession stand worker %d takes order for %s from customer %d.\n", id, food.c_str(), custId);
      sleep(SECONDS(180));
      printf("Concession stand worker %d gives customer %d their %s.\n", id, custId, food.c_str());

      sem_post(&td.finishedAtConc[custId]);
    }
  }
};

// Create and run a concession stand worker object on the thread
void *createCStandWorker(void *args)
{
  (new concession())->run();
  return NULL;
}