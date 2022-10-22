# HW2 Simple Key-value Store

## Directories
- /server ->	server program related sources
- /client ->	client program related sources
- /common ->	common inclusions
- /util ->	common utilization
- /build ->	target build directory

## Building the Project
Code out your `/server/server.c` and `/client/client.c`, then
```shell
$ make
```
Test your `/build/server` and `build/client`.

## Implementations
### Please briefly describe your multi-threading design pattern
In client side, I use 2 threads to maintain sending and receiving information.
- For information sending thread, I use it to detect the valid instruction, if the instruction is valid, then I use send() to send the instruction out to server.
- For receiving thread, it is mainly used to receive data from server with recv() function. With the correct instruction typed, the user can insert, checkout and delete data by the way they want. With each correct instruction, the response is processed in receiving function and then output to screen.
In server side, I put my thread create() function in while loop, when one client connects with my server, server will create one specific thread to handle that connection until the number of clients reach the maximum limit.

### Please briefly describe your data structure implementation
I use client_t data structure in server to implement threading.
- client_t has 3 elements: address, sockfd and uid to represent different functions. Address to store the address of client that connect to the server, sockfd indicates the state of connection and uid is the ID of the client.
- Besides, I use hash table in server to store data that I get after SET instruction, then represent searching and remove of data by GET and DELETE through hash table.

## References
* [POSIX thread man pages](https://man7.org/linux/man-pages/man7/pthreads.7.html)
* [socket man pages](https://linux.die.net/man/7/socket)

