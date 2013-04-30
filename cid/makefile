SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
INCS = -I../include
OBJS = ../obj 
FLAGS = -O 
cc = gcc

all : cid cidtest move

cid : cid.c 
	$(cc) $(EXTRA) $(INCS) $(FLAGS) -o cid cid.c \
	 -L$(OBJS) -lsng

cidtest : cidtest.c 
	$(cc) $(EXTRA) $(INCS) $(FLAGS) -o cidtest cidtest.c -L$(OBJS) -lsng

move :
	mv cid ../bin
	mv cidtest ../bin
