# Makefile for burst
# 
# Haw Yang
# Princeton University
#
# May 28, 2021

PROGRAM = burst
OBJECTS = burst.o cusum.o sprt.o

LIB = -lm -lgsl -lgslcblas
OPT = -O3
CC = gcc

$(PROGRAM): $(OBJECTS)
	$(CC) $(FLAG) $(OPT) $(OBJECTS) -o $(PROGRAM) $(LIB)
