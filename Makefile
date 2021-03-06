CC=g++
FLAGS=-O3 -fopenmp
SOURCES=speedtest.cpp
EXECUTABLE=bin/speedtest

all: $(SOURCES)
	$(CC) $(FLAGS) -o $(EXECUTABLE) $(SOURCES)
