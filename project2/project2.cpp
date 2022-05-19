#include "./header/main.h"        // Main header file
#include "./header/concession.h"  // Concession stand worker class
#include "./header/boxOffice.h"   // Box office agent class
#include "./header/ticketTaker.h" // Ticket taker class
#include "./header/customer.h"    // Customer class

int main(int argc, char const *argv[])
{
  // Seed the random number generator
  srand(time(NULL));

  // Read the input file
  // Map the movies to their available seats in td.movieMap
  mapMovies(argv[1]);

  // PIDs of the threads
  pthread_t boxAgents[NUM_BOX_AGENTS];
  pthread_t ticketTakers[NUM_TICKET_TAKERS];
  pthread_t concessions[NUM_CONCESSIONS];
  pthread_t customers[NUM_CUSTOMERS];

  // Create the threads
  createThreads(boxAgents, NUM_BOX_AGENTS, &createBoxOfficeAgent);
  createThreads(ticketTakers, NUM_TICKET_TAKERS, &createTicketTaker);
  createThreads(concessions, NUM_CONCESSIONS, &createCStandWorker);
  openTheater();
  createThreads(customers, NUM_CUSTOMERS, &createCustomer);

  // Wait for all customer threads to finish
  for (int i = 0; i < NUM_CUSTOMERS; i++)
  {
    pthread_join(customers[i], NULL);
  }

  return 0;
}