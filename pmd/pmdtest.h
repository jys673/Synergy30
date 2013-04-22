/*.........................................................................*/
/*                  PMDTEST.H ------> PMD test program                     */
/*.........................................................................*/

#include "synergy.h"

char PMD_HOST[100] ;		/* host where PMD is running */
int pmdsock ;			/* socket to connect to PMD */

void OpNull(/*void*/) ;
void OpMap(/*void*/) ;
void OpUnmap(/*void*/) ;
void OpGetMap(/*void*/) ;
void OpGetTable(/*void*/) ;
void OpPurgeTable(/*void*/) ;
void OpExit(/*void*/) ;

int connectPmd(/*void*/) ;
u_short drawMenu(/*void*/) ;

