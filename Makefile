#Compiler
CC = gcc
CPP = g++

#Libraries and flags
LIBS = -lm
FLAGS = -g -Wall -lpthread -std=c++11

#Executables
EXECS = dash

%.o: %.cpp
	$(CPP) -c -o $@ $< $(FLAGS)

dash: dash.o psim.o msim.o mmu.o mailbox.o
	$(CPP) $(LIBS) $(FLAGS) -o $@ $^

clean:
	$(RM) $(EXECS) *.o

