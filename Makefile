#Compiler
CC = gcc
CPP = g++

#Libraries and flags
LIBS = -lm
FLAGS = -g -Wall -std=c++11

#Executables
EXECS = dash

%.o: %.cpp
	$(CPP) -c -o $@ $< $(FLAGS)

dash: dash.o psim.o
	$(CPP) $(LIBS) $(FLAGS) -o $@ $?

clean:
	$(RM) $(EXECS) *.o

