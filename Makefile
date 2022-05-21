CC=g++
AR=ar
FLAGS= -Wall -g

all: client server

client:  client.o
	$(CC) $(FLAGS) -o client client.o

test: test.o
	$(CC) $(FLAGS) -o test test.o

test.o: test.cpp
	$(CC) $(FLAGS) -c test.cpp
server:  server.o
	$(CC) $(FLAGS) -pthread -o server server.o

server.o: server.cpp
	$(CC) $(FLAGS) -pthread -c server.cpp

client.o: client.cpp
	$(CC) $(FLAGS) -c client.cpp

.PHONY: clean all
clean:
	rm -f *.o client server test