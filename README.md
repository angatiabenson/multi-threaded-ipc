# Multi-Threaded Calculator Server-Client Model

This project demonstrates a multi-threaded calculator application where a client sends calculation tasks to a server using shared memory. The server retrieves the expressions and returns them back to the client. The client then calculates the expressions concurrently using multiple threads . It leverages POSIX threads (pthreads) for multithreading and POSIX shared memory along with semaphores for inter-process communication.

## Included Files

### Source Files

- **`calculator.cc`**: Implements the calculator logic including tokenization of the input string, conversion of infix expressions to postfix (Reverse Polish Notation) using the Shunting Yard algorithm, and evaluation of the postfix expression.
- **`client.cc`**: The client part of the application. It writes request info to shared memory and signals the server to process them. After processing, it reads back the expressions from shared memory and performs the calculations.
- **`server.cc`**: The server part of the application. It waits for tasks from the client, processes them , and writes the results back to shared memory.

### Header Files

- **`calculator.h`**: The header file for the calculator functions, declaring the functionality provided by `calculator.cc`.

### Other Files

- **`Makefile`**: Used to compile the project. It defines rules for building the server and client executables as well as for cleaning up compiled outputs.

## Relationships and Functionality

- **Calculator (`calculator.cc` / `calculator.h`)**: This file pair provides the core functionality for parsing mathematical expressions, converting them to postfix notation, and evaluating them. The `calculator` namespace encapsulates all related functions to maintain a clear separation from other components of the project.

- **Client (`client.cc`)**: Implements the client-side logic for the application. It utilizes the calculator functionality to send requests to the server and waits for the expressions. This component demonstrates how to interact with shared memory and semaphores for IPC.

- **Server (`server.cc`)**: Acts as the receiver and processor of client requests. The server retrieves the expressions and returns them back to the client via the shared memory.

## Building and Running

The project uses a `Makefile` for easy compilation. You can build the server and client executables by running the following commands in the terminal:

- **Client**

```bash
make client
```

- **Server**

```bash
make server
```

To start the server, run:

```
./server
```

In a separate terminal, start the client with the path to a file containing calculation tasks and the number of lines to process:

```
./client <file_path> <num_lines>
```

Ensure that the server is running before starting the client.

To clean up compiled outputs, run:

```
make clean
```

This project illustrates the integration of multiple C++ programming concepts, including multithreading, inter-process communication, and expression evaluation algorithms, providing a solid foundation for further exploration and development of multi-threaded applications.
