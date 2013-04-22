SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
INCL = -I../include
OBJS = -L../obj -lsng
cc = gcc

all :  pmdlibtest cidlibtest move 

pmdlibtest : pmdlibtest.c
	$(cc) $(EXTRA) $(INCL) -o pmdlibtest  -g -lm pmdlibtest.c $(OBJS) 

cidlibtest : cidlibtest.c
	$(cc) $(EXTRA) $(INCL) -o cidlibtest  -g -lm cidlibtest.c $(OBJS) 
move :
	mv pmdlibtest ../bin
	mv cidlibtest ../bin
