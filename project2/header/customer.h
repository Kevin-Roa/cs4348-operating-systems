class customer
{
private:
  int id; // Unique ID for each customer

public:
  void run()
  {
    // Assign a unique id to this customer
    sem_wait(&td.custMutex1);
    id = td.custCount;
    td.custCount++;
    sem_post(&td.custMutex1);

    // Box office
    sem_wait(&td.boxAgents);
    string movie = td.movieMap[rand() % td.movies.size()];
    td.custMovie[id] = movie;
    sem_wait(&td.custMutex2);
    printf("Customer %d created, buying ticket to %s.\n", id, movie.c_str());
    td.boxQ.push(id);
    sem_post(&td.custAtBox);
    sem_post(&td.custMutex2);
    sem_wait(&td.finishedAtBox[id]);
    sem_post(&td.boxAgents);

    // If movie not available, leave
    if (td.shouldLeave[id])
      leave();

    // Ticket booth
    sem_wait(&td.ticketTakers);
    sem_wait(&td.custMutex3);
    printf("Customer %d in line to see ticket taker.\n", id);
    td.ticketQ.push(id);
    sem_post(&td.custAtTicket);
    sem_post(&td.custMutex3);
    sem_wait(&td.finishedAtTicket[id]);
    sem_post(&td.ticketTakers);

    // 50% chance of visiting the concession stand
    bool visitConc = (rand() % 2) == 0;
    if (visitConc)
    {
      // Concession stand
      sem_wait(&td.concessions);
      string food = getFood();
      td.custFood[id] = food;
      sem_wait(&td.custMutex4);
      printf("Customer %d in line to buy %s.\n", id, food.c_str());
      td.concQ.push(id);
      sem_post(&td.custAtConc);
      sem_post(&td.custMutex4);
      sem_wait(&td.finishedAtConc[id]);
      printf("Customer %d recieves %s.\n", id, food.c_str());
      sem_post(&td.concessions);
    }

    // Watch movie
    printf("Customer %d enters the theater to see %s.\n", id, movie.c_str());

    // Exit theater
    leave();
  }

  // Exit the thread
  void leave()
  {
    printf("Joined customer %d.\n", id);
    pthread_exit(NULL);
  }

  // Get a random order of food
  string getFood()
  {
    switch (rand() % 3)
    {
    case 0:
      return "popcorn";
    case 1:
      return "soda";
    case 2:
      return "popcorn and soda";
    }
    return "";
  }
};

// Create and run a customer object on the thread
void *createCustomer(void *args)
{
  (new customer())->run();
  return NULL;
}
