class ticketTaker
{
private:
  int id; // Unique ID for each ticket taker

public:
  void run()
  {
    // Assign a unique id to this ticket taker
    sem_wait(&td.ticketMutex1);
    id = td.ticketCount;
    td.ticketCount++;
    sem_post(&td.ticketMutex1);
    printf("Ticket taker %d created.\n", id);

    // Loop over customers in queue
    while (true)
    {
      // Get next customer id
      sem_wait(&td.custAtTicket);
      sem_wait(&td.ticketMutex2);
      int custId = td.ticketQ.front();
      td.ticketQ.pop();
      sem_post(&td.ticketMutex2);

      // Tear the customer's ticket
      printf("Ticket taker %d takes customer %d's ticket.\n", id, custId);
      sleep(SECONDS(15));

      sem_post(&td.finishedAtTicket[custId]);
    }
  }
};

// Create and run a ticket taker object on the thread
void *createTicketTaker(void *args)
{
  (new ticketTaker())->run();
  return NULL;
}