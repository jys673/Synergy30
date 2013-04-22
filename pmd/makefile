SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
INCS = -I../include
OBJS = ../obj 
FLAGS = -O 
cc = gcc

all : pmd pmdtest upmdtest move

pmd : pmd.c 
	$(cc) $(EXTRA) $(INCS) $(FLAGS) -o pmd pmd.c -L$(OBJS) -lsng

pmdtest : pmdtest.c 
	$(cc) $(EXTRA) $(INCS) $(FLAGS) -o pmdtest pmdtest.c -L$(OBJS) -lsng

upmdtest : upmdtest.c 
	$(cc) $(EXTRA) $(INCS) $(FLAGS) -o upmdtest upmdtest.c -L$(OBJS) -lsng

move :
	mv pmd ../bin
	mv upmdtest ../bin
	mv pmdtest ../adm
