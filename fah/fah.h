/*.........................................................................*/
/*                     FAH.H ------> File Access Handler
/*.........................................................................*/

#include "synergy.h"

#define TABLE_SIZE    32	/* Max. # of open files */
   
/* File data structure */

typedef struct {
   char path[PATH_LEN] ;	/* information about each file */
   FILE *fp ;			/* fp is NULL if entry not used */
} table_t ;

struct {
   char appid[NAME_LEN] ;	/* application id to which FAH belongs */
   char name[NAME_LEN] ;	/* name of the FAH */
   u_short port ;		/* port at which to accept connections */
} fah ;

table_t *table ;		/* table of file entries */

int oldsock ;			/* socket on which requests are accepted */
int newsock ;			/* new socket identifying a connection */
u_short this_op ;		/* the current operation that is serviced */

/* Prototypes */

void OpFopen(/*void*/) ;
void OpFclose(/*void*/) ;
void OpFgetc(/*void*/) ;
void OpFputc(/*void*/) ;
void OpFgets(/*void*/) ;
void OpFputs(/*void*/) ;
void OpFread(/*void*/) ;
void OpFwrite(/*void*/) ;
void OpFseek(/*void*/) ;
void OpFflush(/*void*/) ;
void OpExit(/*void*/) ;

void initFromsocket(/*int*/) ;
void initFromline(/*char *, char **/) ;
void initFromline2(/*char *, char **/) ;
int initCommon(/*void*/) ;
void start(/*void*/) ;
void sigtermHandler(/*void*/) ;
int getFahport(/*void*/) ;
int mapFahport(/*void*/) ;
int unmapFahport(/*void*/) ;


