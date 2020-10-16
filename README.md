# Program: socket

## Description 
The socket 1.1 program can be trivally used to create a server.  This server can listen on either a TCP or a UNIX domain socket, and when a connection to this socket is made, a specific program can be executed.  Under the current implementation, a separate process is created (forked) when  a socket connection is made.

### Example
$ socket -p "cat" -l -f -B localhost -s 7000

The above command line implements an 'echo' server.  The socket program creates a server-side (-s) socket on port 7000 bound (-B) to the localhost address. When the program accepts a network connection it forks (-f) a child proccess and then loops (-l) around to accept another connection.  The child process then executes a  program (-p), 'cat', in which its stdin and stdout is wired through the socket.

## Proposed Changes
In this project, we seek to modify the existing socket program to create a master node that manages a set of workers. An initial set of workers are created (forked) at the start of the program.  As new requests are received, one of the workers fields the request.  Once complete, the work exits and the master preforks another worker.

Via this approach, there is always a set of workers ready to respond to a network request.

### Example
$ socket -p "cat" -l -f -W 4 -B localhost -s 7000

Notice in the above command line, we have added the -W option, with a value of 4.  The socket program will now maintain a pool of 4 workers.  The general operation of the program, the flow of the master node is as follows:

* create() a socket bound to localhost:7000
* listen() on said socket
* fork() a 4 child processes
* LOOP
  * wait() for a child process to exit
  * fork() an additional child process

Whereas the flow of each child node (i.e., worker) is as follows:

   * accept() on socket
   * execv("cat", NULL) 
   * exit()

a set of child processes to serve as worker nodes.  Under this change, the program will reduce the turn-around time associated with program execution.  The large cost associated with process large cost of process creation

Under this change, the socket program has greater utilize to serve as as mini-daemon.

## Planned Usage
We are currently working on two other project that will immediately use the revised socket program.  The two projects are:
* [https://github.com/csuntechlab/scgi-daemon](SCGI Daemon)
* [https://github.com/csuntechlab/fcgi-daemon](FCGI Daemon)
Both these projects already use the socket program, within the scgi-launch and fcgi-launch scripts.  The use is als follows:

* socket -B ${ADDR} -s ${PORT} -b -f -q -l -p "${FCGI_WRAPPER} ${CGI_PROGRAM}"




By use of this project, coupled with the SCGI and FCGI projects that include wrappers, we can quickly and easy create An SCGI server and a FCGI server.


