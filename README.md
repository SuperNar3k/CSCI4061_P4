# UMN CSCI 4061 Project 4
## Socket Programming
## Spring 2021

**Purpose:** 
This project extends the word length statistics application covered in our previous projects. There is one server and multiple clients. Mappers are implemented as client processes, and reducers as a single server process. Clients (mappers) read files, compute word length statistics for the files, and send the data to the server. The server (reducer) accumulates the word length statistics sent by clients. The server and clients communicate via sockets-based TCP connections, rather than files or pipes.

**How to compile:**
Open two terminals, one to run client and one to run server.    

In order to run the client, be in the PA4_Client directory and execute:
`make`
`./client <foldername> <#ofclients> <serverIP> <serverPort>`


 
In order to run server, be in the PA4_Server directory and execute:
`make`
`./server <serverPort>`

**What this does:**
We made two executables, server and client. For the server program, we used multithreading to implement a multi-threaded server. For the client program, we generate multiple client processes. Each client process reads a file, processes the word length statistics and sends a message to the server through TCP connections. The server spawns a thread whenever it establishes a new TCP connection with a client. The statistics sent by clients are accumulated in a shared resource named "Result Histogram". We reused our codes for computing word length statistics from PA3, since the definition of a word for this assignment is the same. Space and newline character are the only delimiters.

**Assumptions:**
* TCP sockets are used.
* Maximum number of clients is 20.
* Maximum number of concurrent connections at the server side is 50.
* A client connects to a single server at any time.
* A server considers multiple client requests at a time.
* The server program is not terminated unless it is killed by a user.
* Requests and response messages are integer arrays.
* There will be no symbolic or hard links in the input directory.
* The server can receive any types of requests, including both successful and unsuccessful requests.
* GCC version 9.3.0 is used to compile the code.
* Our code can be compiled and run on the CSELabs machines.

**Team names and x500s:** 
* Yna Roxas (roxas007)
* Narek Ohanyan (ohany004)
* Christopher Liu (liux3770)

**Contribution:**
* Yna Roxas: server.c, client.c, readme.md
* Narek Ohanyan: readme.md, documentations, word length calculation, custom IP inputting
* Christopher Liu: server.c, client.c, histogram, testing
  
We tried our best to divide and conquer this project equally, giving each other more work if needed to make our contributions equal and fair to each other.
