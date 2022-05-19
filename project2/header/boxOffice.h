class boxOffice
{
private:
  int id; // Unique ID for each box office agent

  map<string, int> *movies; // Track how many tickets are available for each movie

public:
  boxOffice()
  {
    movies = &td.movies;
  }

  void run()
  {
    // Assign a unique id to this box office agent
    sem_wait(&td.boxMutex1);
    id = td.boxCount;
    td.boxCount++;
    sem_post(&td.boxMutex1);
    printf("Box office agent %d created.\n", id);

    // Loop over customers in queue
    while (true)
    {
      // Get next customer id
      sem_wait(&td.custAtBox);
      sem_wait(&td.boxMutex2);
      int custId = td.boxQ.front();
      td.boxQ.pop();
      sem_post(&td.boxMutex2);

      printf("Box office agent %d is serving customer %d.\n", id, custId);

      // Get customer's movie choice
      string cm = td.custMovie[custId];
      sem_wait(&td.boxMutex3);
      sleep(SECONDS(90));
      // Check if movie is available
      if ((*movies)[cm] > 0)
      {
        // Movie available, give ticket
        td.custMovie[custId] = cm;
        (*movies)[cm]--;
        printf("Box office agent %d sold a ticket for %s to customer %d.\n", id, cm.c_str(), custId);
      }
      else
      {
        // Movie sold out, customer should leave
        printf("Box office agent %d tells customer %d that tickets to %s are sold out.\n", id, custId, cm.c_str());
        td.shouldLeave[custId] = true;
      }
      sem_post(&td.boxMutex3);

      sem_post(&td.finishedAtBox[custId]);
    }
  }
};

// Create and run a box office agent object on the thread
void *createBoxOfficeAgent(void *args)
{
  (new boxOffice())->run();
  return NULL;
}