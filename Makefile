#Compiler
CC = gcc
CPP = g++

#Libraries and flags
LIBS = -lm
FLAGS = -g -Wall

#Executables
EXECS = dash

dash: dash.cpp
	$(CPP) $(LIBS) $(FLAGS) dash.cpp -o dash

clean:
	$(RM) $(EXECS)

