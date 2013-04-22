SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
INCS = -I../include
OBJS = ../obj 
FLAGS = -O 
cc = gcc

all : fah fahtest move 

fah : fah.c 
	$(cc) $(EXTRA) $(INCS) $(FLAGS) -o fah fah.c -L$(OBJS) -lsng

fahtest : fahtest.c 
	$(cc) $(EXTRA) $(INCS) $(FLAGS) -o fahtest fahtest.c -L$(OBJS) -lsng

move :
	mv fah ../bin
	mv fahtest ../bin
