#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <map>
#include <fstream>
#include <string>

#define SECONDS(x) x / 60   // Used to scale the time down to 1/60th the time
#define NUM_CUSTOMERS 50    // Number of customer threads
#define NUM_BOX_AGENTS 2    // Number of box office agent threads
#define NUM_TICKET_TAKERS 1 // Number of ticket taker threads
#define NUM_CONCESSIONS 1   // Number of concession worker threads

using namespace std;

void mapMovies(char const *);
void createThreads(pthread_t *, int, void *(void *));
void createThread(pthread_t *, void *(void *), void *);
void createSemaphores(sem_t *, int, int);
void createSemaphores(sem_t *, int, int = 0);
void createSemaphore(sem_t *, int);
void createSemaphore(sem_t *, int = 0);
void *createCustomer(void *);
void *createBoxOfficeAgent(void *);
void *createTicketTaker(void *);
void *createCStandWorker(void *);

struct THREAD_DATA
{
  sem_t boxAgents;    // Number of available box office agents
  sem_t ticketTakers; // Number of available ticket takers
  sem_t concessions;  // Number of available concession stand workers

  sem_t custAtBox;    // Signal the customer to be handled at the box office
  sem_t custAtTicket; // Signal the customer to be handled at the ticket booth
  sem_t custAtConc;   // Signal the customer to be handled at the concession stand

  sem_t finishedAtBox[NUM_CUSTOMERS];    // Track which customer has left the box office
  sem_t finishedAtTicket[NUM_CUSTOMERS]; // Track which customer has left the ticket booth
  sem_t finishedAtConc[NUM_CUSTOMERS];   // Track which customer has left the concession stand

  sem_t custMutex1;   // Mutex for tassigning ids to each customer
  sem_t custMutex2;   // Mutex for enqueing the customer in the box office
  sem_t custMutex3;   // Mutex for enqueing the customer in the ticket booth
  sem_t custMutex4;   // Mutex for enqueing the customer in the concession stand
  sem_t boxMutex1;    // Mutex for assigning ids to each box office agent
  sem_t boxMutex2;    // Mutex for dequeuing customers in the box office
  sem_t boxMutex3;    // Mutex for buying a ticket at the box office
  sem_t ticketMutex1; // Mutex for assigning ids to each ticket taker
  sem_t ticketMutex2; // Mutex for dequeuing customers in the ticket booth
  sem_t concMutex1;   // Mutex for assigning ids to each concession stand worker
  sem_t concMutex2;   // Mutex for dequeuing customers in the concession stand

  string custMovie[NUM_CUSTOMERS] = {""};    // Track the movie each customer is watching
  string custFood[NUM_CUSTOMERS] = {""};     // Track the food each customer is eating
  bool shouldLeave[NUM_CUSTOMERS] = {false}; // Track whether or not each customer should leave

  int custCount = 0;   // Track the number of customers
  int boxCount = 0;    // Track the number of box office agents
  int ticketCount = 0; // Track the number of ticket takers
  int concCount = 0;   // Track the number of concession stand workers

  map<string, int> movies;   // Map the movie name to the number of tickets available
  map<int, string> movieMap; // Map the movie id to the movie name

  queue<int> boxQ = {};    // Queue of customers in line at the box office
  queue<int> ticketQ = {}; // Queue of customers in line at the ticket booth
  queue<int> concQ = {};   // Queue of customers in line at the concession stand

  THREAD_DATA() : boxAgents{NUM_BOX_AGENTS}, ticketTakers{NUM_TICKET_TAKERS}, concessions{NUM_CONCESSIONS}, custAtBox{0}, custAtTicket{0}, custAtConc{0}, custMutex1{1}, custMutex2{1}, custMutex3{1}, custMutex4{1}, boxMutex1{1}, boxMutex2{1}, boxMutex3{1}, ticketMutex1{1}, ticketMutex2{1}, concMutex1{1}, concMutex2{1}
  {
    createSemaphores(finishedAtBox, NUM_CUSTOMERS, 0);
    createSemaphores(finishedAtTicket, NUM_CUSTOMERS, 0);
    createSemaphores(finishedAtConc, NUM_CUSTOMERS, 0);
  }
} td;

void mapMovies(char const *movieFile)
{
  // Open the file
  ifstream file(movieFile);

  // Read the file line by line
  string line;
  int i = 0;
  while (getline(file, line))
  {
    // Split the line into the movie name and the number of tickets available
    string title = line.substr(0, line.find('\t'));
    int seats = stoi(line.substr(line.find('\t') + 1));

    td.movieMap.insert(pair<int, string>(i, title));
    td.movies.insert(pair<string, int>(title, seats));
    i++;
  }

  // for (const auto &n : td.movies)
  //   cout << n.first << ": " << n.second << endl;
}

// Create various threads and store their PIDs in the array
void createThreads(pthread_t *PIDArr, int numThreads, void *(startRoutine)(void *))
{
  for (int i = 0; i < numThreads; i++)
  {
    createThread(&PIDArr[i], startRoutine, NULL);
  }
}

// Create a thread and store its PID in the pointer
void createThread(pthread_t *PID, void *(startRoutine)(void *), void *arg)
{
  if (pthread_create(PID, NULL, startRoutine, arg) != 0)
  {
    cout << "Error creating thread" << endl;
    exit(1);
  }
}

// Creates various semaphores and stores them in the array
void createSemaphores(sem_t *semArr, int numSemaphores, int initVal)
{
  for (int i = 0; i < numSemaphores; i++)
  {
    createSemaphore(&semArr[i], initVal);
  }
}

// Creates a semaphore and stores it in the pointer
void createSemaphore(sem_t *sem, int initVal)
{
  if (sem_init(sem, 0, initVal) != 0)
  {
    cout << "Error initializing semaphore" << endl;
    exit(1);
  }
}

// Open the theater after all of the threads have been created
void openTheater()
{
  sleep(1);
  printf("Theater is open.\n");
}