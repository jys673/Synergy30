/*.........................................................................*/
/*             	FDD.H ------> Fault Detecter Daemon		           */
/*.........................................................................*/

#include "synergy.h"


/* global variables */

host_t *host_list, *host_end;
int clist_cnt;
handlerinfo_t *handler_list, *handler_end;
int hlist_cnt;
u_short dacport;
u_long dachostid;
		/* Modified by FSUN 01/95 */
int debug;
int g_status;

/* prototypes */

void initFdd(/*void*/);
void startFdd(/*void*/);
void ccid (/*u_long hostid, u_short cidport*/);
int fdd_ccid(/*u_long, u_short, char *, int*/);	/* Added by FSUN 01/95 */
void arlm_handler(/*void*/);
