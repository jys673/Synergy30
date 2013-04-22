/*.........................................................................*/
/*                  FAHTEST.H ------> FAH test program                     */
/*.........................................................................*/
#include "synergy.h"

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

char FAH_HOST[100] ;
char mapid[MAP_LEN] ;
int fahsock ;

int connectFah(/*void*/) ;
u_short drawMenu(/*void*/) ;
