SYS_INCLUDE=-I/usr/include-I/usr/lib/gcc-lib/i486-linux/2.5.8/include
SYS_LINK=-L/usr/lib
EXTRA = $(SYS_TYPE) $(SYS_LINK)
INCS = -I../include
OBJS = ../obj 
FLAGS = -O 
all : bcr move

bcr : bcr.c 
	cc $(EXTRA) $(INCS) $(FLAGS) -o bcr bcr.c \
	 -L$(OBJS) -lsng

move :
	mv bcr ../bin
