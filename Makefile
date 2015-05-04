#Compiler
CC = gcc
CPP = g++

#Libraries and flags
LIBS = -lm
FLAGS = -g -Wall -lpthread

#Executables
EXECS = dash

dash: dash.cpp mailbox.cpp
	$(CPP) $(LIBS) $(FLAGS) dash.cpp mailbox.cpp -o dash

clean:
	$(RM) $(EXECS)

