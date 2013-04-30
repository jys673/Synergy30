SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
FLAGS = -O3 -o
cc = gcc

all: dhosts sds sfs shosts shosts2 addhost delhost chosts cds kds kads kobjs move 

dhosts: dhosts.c
	$(cc) $(EXTRA) -I../include $(FLAGS) dhosts dhosts.c

sds: sds.c
	$(cc) $(EXTRA) -I../include $(FLAGS) sds sds.c

sfs: sfs.c
	$(cc) $(EXTRA) -I../include $(FLAGS) sfs sfs.c

shosts: shosts.c 
	$(cc) $(EXTRA) -I../include $(FLAGS) shosts shosts.c

shosts2: shosts2.c
	$(cc) $(EXTRA) -I../include $(FLAGS) shosts2 shosts2.c

addhost: addhost.c
	$(cc) $(EXTRA) -I../include $(FLAGS) addhost addhost.c

delhost: delhost.c
	$(cc) $(EXTRA) -I../include $(FLAGS) delhost delhost.c

chosts: chosts.c
	$(cc) $(EXTRA) -I../include $(FLAGS) chosts chosts.c

cds: cds.c
	$(cc) $(EXTRA) -I../include $(FLAGS) cds cds.c -L../obj -lsng

kds:	kds.c
	$(cc) $(EXTRA) -I../include $(FLAGS) kds kds.c -L../obj -lsng

kads: 	kads.c
	$(cc) $(EXTRA) -I../include $(FLAGS) kads kads.c -L../obj -lsng

kobjs:	kobjs.c
	$(cc) $(EXTRA)  -I../include $(FLAGS) kobjs kobjs.c -L../obj -lsng

move: 
	mv sfs ../bin
	mv shosts ../bin
	mv shosts2 ../bin
	mv addhost ../bin
	mv delhost ../bin
	mv dhosts ../bin
	mv chosts ../bin
	mv cds ../bin
	mv kobjs ../bin
	mv sds ../bin
	mv kads ../bin
	mv kds ../bin
