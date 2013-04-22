#define AIX_ (0)
#define BSD (0)
#define VMS (0)
#define SYSV (1) 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#if VMS  /* for VAX only */ 
	#include <sys/signal.h>
	#include <sys/netdb.h>
#endif
/**** For Unix: ******/
#include <netdb.h> 
#include <fcntl.h> 
/* #include <sys/ipc.h> */
/* #include <sys/msg.h> */
#include <sys/time.h>
#include <sys/times.h>
#if SYSV 
#include <sys/param.h>
#include <sys/ioctl.h>
#endif 
#include <math.h>
/**********************/
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <stdio.h>

#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif
