## Compile/Run
  ```
  g++ project2.cpp -o Project2 -std=c++11 -pthread 

  ./Project2 <input_file>
  ```
  Sample input file is titled `movies.txt`
___
## Description
###### Taken straight from the problem document
Customer:
1.	50 customers are in line at the box office when the simulation starts (1 thread per customer..
2.	Movie titles and seats available are read from an input file.
3.	Each customer randomly chooses a movie and requests to buy a ticket from the box office.
4.	If the movie the customer wants to see is sold out, the customer leaves.
5.	The customer must see the ticket taker before entering the theater’s main lobby.
6.	Each customer randomly chooses whether to visit the concession stand (50% chance to visit it.. 
7.	Each customer randomly visiting the concession stand randomly chooses popcorn, soda, or both.

Box Office Agents:
1.	Two created initially, one thread each.
2.	Serves next customer in line.
3.	Maintains count of tickets sold for each movie—does not sell ticket if movie is sold out.
4.	If movie is sold out, this is communicated to the customer.

Ticket Taker:
1.	Created initially, one thread.
2.	Serves next customer in line.
3.	Takes and tears ticket.

Concession Stand Worker:
1.	Created initially, one thread.
2.	Serves next customer in line.
3.	Gets order from customer.
4.  Fills order.

Main
1.	Creates all threads and joins all customer threads.
2.	When last customer has exited, ends the simulation.
 
Other rules:
1.	Some actions take time.
2.	Each activity of each thread should be printed with identification (e.g., customer 1..
3.	A thread may not use sleeping as a means of coordinating with other threads.
4.	Busy waiting (polling. is not allowed. 
5.	Mutual exclusion should be kept to a minimum to allow the most concurrency.
6.	The semaphore value may not obtained and used as a basis for program logic.
7.	Each thread should print when it is created, and each customer should print when it is joined.
8.	All activities of a thread should only be output by that thread.

