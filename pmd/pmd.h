/*.........................................................................*/
/*                     PMD.H ------> Port Mapper Daemon                    */
/*                     February '13, updated by Justin Y. Shi              */
/*.........................................................................*/

#include "synergy.h"

/* constants */

#define OPS_FILE      "/tmp/pmdops.log"
#define OPS_FILE_OLD  "/tmp/pmdops.old"
#define OPS_MAX       5000	/* max # of operations stored in log file */

#define TABLE_SIZE    200	/* size of the PMD table */

#define USED          1		/* entry in PMD table is used/unused */
#define UNUSED        0
  
/* PMD data structure */

typedef struct {
  char mapid[MAP_LEN] ;		/* logical name for port */
  u_short protocol ;		/* tcp/udp */
  u_short port ;		/* port # */
  int pid ;			/* pid of process that has the port */
  int used ;			/* is the entry in table used/unused */
} table_t ;

table_t *table ;		/* PMD table */
long start_time ;		/* time when the PMD was started */

int oldsock ;			/* socket on which requests are accepted */
int newsock ;			/* new socket identifying a connection */
u_short this_op ;		/* the current operation that is serviced */
u_long peer ;			/* address of the requestor */

/*  Prototypes.  */

void OpNull(/*void*/) ;
void OpMap(/*void*/) ;
void OpUnmap(/*void*/) ;
void OpGetMap(/*void*/) ;
void OpGetTable(/*void*/) ;
void OpPurgeTable(/*void*/) ;
void OpExit(/*void*/) ;

void initOther(/*void*/) ;
void initSocket(/*void*/) ;
void start(/*void*/) ;
int getSlot(/*char *, u_short*/) ;
int unmapMapid(/*char *, u_short*/) ;
int unmapPid(/*u_long*/) ;
void sendTable(/*void*/) ;
void sigtermHandler(/*void*/) ;
void logOps(/*void*/) ;

