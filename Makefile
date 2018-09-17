CC = g++

CFLAGS = -g -Wall

demo:
	$(CC) $(CFLAGS) main.cpp Graph.cpp -o run

