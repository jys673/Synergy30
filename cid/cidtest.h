/*.........................................................................*/
/*                  CIDTEST.H ------> CID test program                     */
/*                  February '13, updated by Justin Y. Shi                 */
/*.........................................................................*/

#include "synergy.h"

#define SEGMENT 5000

char CID_HOST[100] ;
char CID_LOGIN[32]; 
u_short this_op ;
int cidsock ;

void OpNull(/*voide*/) ;
void OpExecTsh(/*voide*/) ;
void OpExecComponent(/*voide*/) ;
void OpKill(/*voide*/) ;
void OpKillApp(/*voide*/) ;
void OpKillAll(/*voide*/) ;
void OpGetApptable(/*voide*/) ;
void OpGetFulltable(/*voide*/) ;
void OpPing(/*voide*/) ;
void OpPingApp(/*voide*/) ;
void OpRemove(/*voide*/) ;
void OpFileGet(/*voide*/) ;
void OpFilePut(/*voide*/) ;
void OpAddUser(/*voide*/) ;
void OpDeleteUser(/*voide*/) ;
void OpVerifyUser(/*voide*/) ;
void OpVerifyProcess(/*voide*/) ;
void OpVerifyHandler(/*voide*/) ;
void OpExit(/*voide*/) ;
void OpExitProcess(); 
void OpResourceCk();

/* V3 FSUN 07/94 */

void OpSaveAppMsg();
void OpChangeAppMsg();
void OpGetAppList();
void OpGetApp();
void OpDelAppList();
void OpDelApp();

u_short drawMenu(/*voide*/) ;
int connectCid(/*voide*/) ;

/*.........................................................................*/
/*                  PMDTEST.H ------> PMD test program                     */
/*.........................................................................*/

char PMD_HOST[100] ;		/* host where PMD is running */
int pmdsock ;			/* socket to connect to PMD */

void pmd_OpNull();
void OpMap(/*void*/) ;
void OpUnmap(/*void*/) ;
void OpGetMap(/*void*/) ;
void OpGetTable(/*void*/) ;
void OpPurgeTable(/*void*/) ;

u_short drawPMDMenu(/*void*/) ;

/*.........................................................................*/
/*                  TSHTEST.H ------> TSH test program                     */
/*.........................................................................*/

int tshsock ;

void OpPut(/*void*/) ;
void OpGet(/*void*/) ;

u_short drawTSHMenu(/*void*/) ;

