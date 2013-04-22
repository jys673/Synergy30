/*.........................................................................*/
/*                  SNG_PRUN.C ------> MP Specific Parallel Run Command            */
/*.........................................................................*/

#include "sng_prun.h"

/*---------------------------------------------------------------------------
  Prototype   : int main(int argc, char **argv)
  Parameters  : argc   - argument count
                argv[1]- .prcd file name 
  Returns     : -
  Called by   : User.  
  Calls       : close, exit, printf, start_dac, OpSaveStatusAll, OpVerifyApp, 
		OpExit, OpStart 
  Notes       : Argv[1] is .prcd filename, DAC is started and the file
                is passed to it. Then, the application is started.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification: 07/94 Feijian Sun:
		Removed the option for non-return process status.
		Changed non-blocking to blocking run. 
                February '13, updated by Justin Y. Shi
---------------------------------------------------------------------------*/  

int main(argc, argv)
int argc; 
char **argv;
{
	int ret, mp_pid, fdd_id;
	u_short dacport, prunport;
	int fdd_start = 0;
	char cmd[MAP_LEN];

	prunport = 0; 
	if (argc < 2) {
		printf("Usage : %s <prcd file>\n", argv[0]);
		exit(1);
	}	
	if ((argc > 2) && (isdigit(argv[2][0]))) mp_pid = atoi(argv[2]);
	if (argc > 3) 
	{
		if (!strcmp(argv[3], "-ft")) {
			fdd_start = 1;
			debug = 0;
		} else {
			fdd_start = 0;
			debug = 1;
		}
	}
	else {
		fdd_start = 0;
		debug = 0;
	};

	/* start DAC, send ssh_port to it, and get it's port */
	sprintf(fname,"%s.prcd",argv[1]);
	ret = start_dac(&dacport, fname, prunport, debug, mp_pid, fdd_start);
	switch (ret) {
		case 0 : 
			printf("Error : Unable to execute DAC\n");
			exit(1);
		case 1 : 
			printf("Error : PMD error\n");
			exit(1);
		case 2 : 
			printf("Error : %s error.\n", argv[1]);
			exit(1);
	}
	
	ret = 0;
        if (OpVerifyApp(dacport))
                if (OpStart(dacport))
		    if (fdd_start)
		    {

			ret = start_fdd2(dacport, debug, &fdd_id);
	 //	printf(" Fault Tolerance activated ::: ret(%d) \n",ret);
			if (ret) {
				sprintf(cmd, "kill -9 %d", fdd_id);
				printf(" FDD started successfully.\n");
			} else printf("FDD failed to start.\n");
		    }
		return(1); 
                    
        OpExit(dacport);
	return(0);
}

int start_fdd2(dacport, debug, fid)
u_short dacport;
int debug;
int *fid;
{
   int pid, noexec ;
   char FDD_PATH[128] ; /* Remove path dependency. YS 1994 */
   char buf_dacport[16], buf_debug[5];

   sprintf(buf_dacport, "%u", dacport);
   if (getenv("SNG_PATH") == NULL)
   {
	printf("SNG_PATH undefined. Exiting...\n");
	exit(0);
   }
   sprintf(FDD_PATH,"%s/bin/fdd",getenv("SNG_PATH"));
   sprintf(buf_debug,"%d",debug);

   noexec = 0 ;
   if (!(pid = fork()))         /* exec FDD */
    {
/*
#if IRIX
       BSD_setpgrp(0, getpid()) ;
#endif
#if Linux
*/
       setpgrp();
/*
#else
       setpgrp(0, getpid()) ;
#endif
*/
       execl(FDD_PATH, "fdd", buf_dacport, buf_debug, (char *)0) ;
       noexec = 1 ;
       exit(1) ;
    }
   *fid = pid;
   if (noexec)
       return 0 ;
   else
       return 1 ;
}

/*---------------------------------------------------------------------------
  Prototype   : int OpStart(u_short dacport)
  Parameters  : -
  Returns     : If application is started sucessfully, return 1. Otherwise, 
		return 0.
  Called by   : start
  Calls       : start_app, printf
  Notes       : The application is started. 'app_started' is set. 
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification: Added returns. Feijian Sun, 07/94.
---------------------------------------------------------------------------*/

int OpStart(dacport)
u_short dacport;
{
        int stat;
                                /* start application */
        if ((stat = start_app(dacport)) == -1) {
                printf("Error : Communication error with DAC. \n");
                return(0);
        }
        if (stat) {
                printf("****** (%s) started. \n",fname);
		return(1);
	}
        if (dacerrno == DAC_ER_EXECHANDLER) {
                printf("Error : Unable to start objects.\n");
                return(0);
	}        

        printf("Error : Unknown error, call system operator.\n");
        return(0);
}


/*---------------------------------------------------------------------------
  Prototype   : int OpSaveStatusAll(u_short dacport)
  Parameters  : -
  Returns     : If Saving all status succeed, return 1. Otherwise return 0. 
  Called by   : start
  Calls       : save_all_status, printf
  Notes       : Exit-status of all processes will be saved.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification: Added returns. Feijian Sun, 07/94.
---------------------------------------------------------------------------*/  
int OpSaveStatusAll(dacport)
u_short dacport;
{
	int stat;
				/* save all exit-status */
	if ((stat = save_all_status(dacport)) == -1) {
		printf("Error : Communication error with DAC.\n");
		return(0);
	}
				/* process errors */
	if (stat) {
		return(1);
	} 

	printf("Error : Unknown error, call system operator.\n");
	return(0);
}


/*---------------------------------------------------------------------------
  Prototype   : int OpVerifyApp(u_short dacport)
  Parameters  : -
  Returns     : If all components are executable, return 1. Otherwise, return 0. 
  Called by   : start
  Calls       : verify_app, printf
  Notes       : All processes/handlers are checked whether they are executable.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification: Added returns. Feijian Sun, 07/94
---------------------------------------------------------------------------*/  
int OpVerifyApp(dacport)
u_short dacport;
{
	char name[NAME_LEN];
	int stat;

				/* verify application */
	if ((stat = verify_app(dacport, name)) == -1) {
		printf("Error : Communication error with DAC.\n");
		return(0);
	}
				/* process errors */
	if (stat) {
		printf("****** (%s) verified, all components executable.\n",fname);
		return(1);
	} 
	if (dacerrno == DAC_ER_EXECHANDLER) {
		printf("Error : Unable to start object : %s.\n", name);
		return(0);
	}
	if (dacerrno == DAC_ER_EXECPROCESS) {
		printf("Error : Unable to execute process : %s.\n", name);
		return(0);
	} 

	printf("Error : Unknown error, call system operator.\n") ;
	return(0);
}


/*---------------------------------------------------------------------------
  Prototype   : void OpExit(u_short dacport)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : exit_dac, printf, exit
  Notes       : DAC is made to exit.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpExit(dacport)
u_short dacport;
{
   int stat ;
                                /* exit DAC */
   if ((stat = exit_dac(dacport)) == -1)
    {
       printf("Error : Communication error with DAC.\n") ;
       exit(1) ;
    }
                                /* process errors */
   if (stat)
      printf("+++++ DAC exitted.\n") ;
   else
      printf("Error : Unknown error, call system operator.\n") ;
}



