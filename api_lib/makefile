SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
SYS_LINK=-L/usr/lib
EXTRA = $(SYS_TYPE) $(SYS_LINK)
INCS = -I../include
FLAGS = -O3 -c
all : activatespace cleanupqueue cleanupspace openspace closespace initspace readdata senddata gettoken settoken archive cleanup

openspace: _open_space.c
	gcc $(EXTRA) $(INCS) $(FLAGS)  _open_space.c

closespace: _close_space.c
	gcc $(EXTRA) $(INCS) $(FLAGS)  _close_space.c

activatespace: _activate_space.c
	gcc $(EXTRA) $(INCS) $(FLAGS)  _activate_space.c

cleanupqueue: _cleanup_queue.c
	gcc $(EXTRA) $(INCS) $(FLAGS)  _cleanup_queue.c

cleanupspace: _cleanup_space.c
	gcc $(EXTRA) $(INCS) $(FLAGS)  _cleanup_space.c

initspace: _init_space.c
	gcc $(EXTRA) $(INCS) $(FLAGS)  _init_space.c

readdata: _read_data.c
	gcc $(EXTRA) $(INCS) $(FLAGS)  _read_data.c

senddata: _send_data.c
	gcc $(EXTRA) $(INCS) $(FLAGS)  _send_data.c

gettoken: _get_token.c
	gcc $(EXTRA) $(INCS) $(FLAGS)  _get_token.c

settoken: _set_token.c
	gcc $(EXTRA) $(INCS) $(FLAGS)  _set_token.c

archive:
	ar rcv ../obj/libsng.a *.o

cleanup:
	/bin/rm -f *.o
