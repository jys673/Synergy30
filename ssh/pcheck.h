/*.........................................................................*/
/*                  PCHECK.H ------> Synergy shell program                 */
/*                  February '13, updated by Justin Y. Shi                 */
/*.........................................................................*/

#include "synergy.h"

appmsg_t *list_app, *end_app;
int alist_cnt;
procinfo_t *list_proc, *end_proc;
int plist_cnt;

int frshid;
u_short dacport;		/* port of the associated DAC */
extern u_short dacerrno;	/* to access errors from daclib */

int display_app(/*int *app_num*/);
void display_proc(/*void*/);
int addr_to_hostname(/*char *host_addr, char **host_name*/);
void check_proc(/*void*/);
void killfresh(/*void*/);
void freshproc(/*void*/);
int hand_proc(/*void*/);
void killapp(/*void*/);
void killallapp(/*void*/);
void OpKill(/*void*/);
void OpKillAll(/*void*/);
