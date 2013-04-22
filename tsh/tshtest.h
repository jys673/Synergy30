/*.........................................................................*/
/*                  TSHTEST.H ------> TSH test program                     */
/*                  February '13, updated by Justin Y. Shi                 */
/*.........................................................................*/

#include "synergy.h"

char login[NAME_LEN];

void OpPut(/*void*/) ;
void OpGet(/*void*/) ;
void OpExit(/*void*/) ;
void OpRetrieve(/*void*/) ;

char TSH_HOST[100] ;
char mapid[MAP_LEN] ;
int tshsock ;

int connectTsh(/*void*/) ;
u_short drawMenu(/*void*/) ;

