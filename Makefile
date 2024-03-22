CC=g++
CFLAGS=-Wall -std=c++11
LIBS=-lpthread -lrt

# Target for the client program
client: src/client.cc src/calculator.cc
	$(CC) $(CFLAGS) -o client src/client.cc src/calculator.cc $(LIBS)

# Target for the server program
server: src/server.cc
	$(CC) $(CFLAGS) -o server src/server.cc $(LIBS)

# Clean compiled binaries
clean:
	rm -f client server
