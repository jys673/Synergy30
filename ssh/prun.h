/*.........................................................................*/
/*                  PRUN.H ------> Synergy shell program                   */
/*                  February '13, updated by Justin Y. Shi                 */
/*.........................................................................*/

#include "synergy.h"
#include "iolib.h"

extern u_short dacerrno;	/* to access errors from daclib */
char fname[MAP_LEN];
int debug=0;
long start_t, stop_t;

int OpStart(/*u_short dacport*/) ;
int OpVerifyApp(/*u_short dacport*/) ;
void OpExit(/*u_short dacport*/) ;
int waitdacexit(/*int oldsock*/);

