SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
INCS = -I../include
OBJS = ../obj 
FLAGS = -O3
all : tsh  copy

tsh : tsh.c 
	gcc $(EXTRA) $(INCS) $(FLAGS) -o tsh tsh.c -L$(OBJS) -lsng

copy:
	cp tsh ../bin
