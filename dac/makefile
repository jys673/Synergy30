SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
EXTRA =
INCS = -I../include
OBJS = ../obj 
FLAGS = -O3 
cc = gcc

all : dac move 

dac : dac.c 
	$(cc) $(EXTRA) $(INCS) $(FLAGS) -o dac dac.c -L$(OBJS) -lsng
move :
	mv dac ../bin
