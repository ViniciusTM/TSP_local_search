CC = g++

CFLAGS = -g -Wall

demo:
	$(CC) $(CFLAGS) main.cpp Graph.cpp Solver.cpp -o run

