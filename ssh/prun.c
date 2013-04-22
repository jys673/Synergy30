/*.........................................................................*/
/*                  PRUN.C ------> Synergy Parallel Run Command            */
/*.........................................................................*/

#include "prun.h"
int status;

/*---------------------------------------------------------------------------
  Prototype   : int main(int argc, char **argv)
  Parameters  : argc   - argument count
                argv[1]- .prcd file name 
  Returns     : -
  Called by   : User.  
  Calls       : close, exit, printf, get_socket, bind_socket, start_dac, 
		OpSaveStatusAll, OpVerifyApp, OpExit, OpStart, waitdacexit  
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
	int ret, fdd_id;
	int oldsock;
	u_short dacport, prunport;
	char hostname[NAME_LEN], cmd[MAP_LEN];
	FILE *objs_file;
	int fdd_start = 0;

	if (argc < 2) {
		printf("Usage : %s <.csl file> [-ft | debug]\n", argv[0]);
		exit(1);
	}
	if (argc > 2)
		if (!strcmp(argv[2], "-ft")) {
			fdd_start = FDD_ALIVE;
			debug = 0;
		} else {
			fdd_start = 0;
			debug = 1;
		}
	else {
		fdd_start = 0;
		debug = 0;
	}
	
	gethostname(hostname,sizeof(hostname));
/*
	if (!ping_cid(hostname, getpwuid(getuid())->pw_name))
	{
		printf(" CID not ready. Auto-starting ... \n");
		status=system("cid &");
		sleep(3);
	}
*/
	/* checking live processors */
	printf("== Checking Processor Pool:\n");
	status=system("cds -f");
	printf("== Done.\n");

	/* start configuration */
/*
	sprintf(cmd,"%s.objs.prcd",argv[1]);
	if ((objs_file = fopen(cmd, "r")) != NULL)
	{
		printf("++ Debugging mode found. \n");
		debug = 1;
	}	
*/
	if (debug) /* debugging mode */
	{
		sprintf(cmd,"conf %s debug", argv[1]);
		status=system(cmd);
		printf("== Done.\n");
		/* create distributed objects using .objs.prcd file */
		printf("== Starting distributed objects.\n");
		sprintf(cmd,"sng_prun %s.objs 0",argv[1]);
		status=system(cmd);
		printf("== Done.\n");
		printf("++ Entering Synergy Debugging Session ... \n");
		printf("	Use .dbx to start debugging each program. \n");
		printf("	Please keep .ini file along with each program. \n");
		printf("++ Type %s.end to terminate the debugging session.\n",argv[1]);
		exit (1);
	}
	/* configuration first */

	sprintf(cmd,"conf %s", argv[1]);
	status=system(cmd);
	printf("== Done.\n");
	sprintf(cmd,"%s.prcd",argv[1]);
	if ((objs_file = fopen(cmd, "r")) == NULL)
	{
		printf("++ %s Aborted.\n",argv[1]);
		exit (1);
	}	
	/* startup dac now */
	printf("== Starting Distributed Application Controller ...\n");
        if ((oldsock = get_socket()) != -1)
                if (!(prunport = bind_socket(oldsock, 0)))
                        exit(E_NOPORT);
		/* start DAC, send ssh_port to it, and get it's port */
	sprintf(fname,"%s.prcd",argv[1]);
	ret = start_dac(&dacport, fname, prunport, debug, getpid(), fdd_start);
	switch (ret) {
		case 0 : 
			printf("++ DAC. Error : Unable to execute DAC\n");
			exit(1);
		case 1 : 
			printf("++ DAC. Error. Local PMD down\n");
			printf("++ DAC. Auto stating PMD ...\n");
			status=system("pmd &");
			exit(1);
		case 2 : 
			printf("++ DAC. Error : CID absent. \n");
			printf("++ DAC. Starting CID. Please try again ...\n");
			status=system("cid &");
			exit(1);
	}

	ret = 0;
        if (OpVerifyApp(dacport))
            if (OpStart(dacport)) {
		if (fdd_start) {
        	    ret = start_fdd(dacport, debug, &fdd_id);
        	    if (ret) {
                	sprintf(cmd, "kill -9 %d", fdd_id);
                	printf(" FDD: Started successfully.\n");
        	    } else
                	printf(" FDD: Failed to start.\n");
		}
                /* PRUN will be blocked here until DAC_exits */
                if (waitdacexit(oldsock)) {
		    stop_t = time((long *)0);
		    printf("== (%s) completed. Elapsed [%ld] Seconds. \n", 
				argv[1], stop_t - start_t);
                    close(oldsock);
		    if (ret) status=system(cmd);
		    sprintf(cmd, "/bin/rm %s.prcd",argv[1]);
		    status=system(cmd);
                    return;
                }

	    }

	printf("++ DAC. Could not start application (%s). \n", argv[1]);
        OpExit(dacport);
	close(oldsock);
	if (ret) status=system(cmd);
	sprintf(cmd, "/bin/rm %s.prcd",argv[1]);
	status=system(cmd);
}


/*---------------------------------------------------------------------------
  Prototype   : int OpStart(void)
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
	start_t = time((long *)0);
        if ((stat = start_app(dacport)) == -1) {
                printf("++ DAC. Error : Communication error with DAC. \n");
                return(0);
        }
        if (stat) {
                printf("** (%s) started. \n",fname);
		return(1);
	}
        if (dacerrno == DAC_ER_EXECHANDLER) {
                printf("++ DAC. Error : Unable to start objects.\n");
                return(0);
	}        

        printf("++ DAC. Error : Unknown error, call system operator. stat(%d) dacerrno(%d)\n",
			stat, dacerrno);
        return(0);
}

/*---------------------------------------------------------------------------
  Prototype   : int OpVerifyApp(void)
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
	if ((stat = verify_app(dacport, name)) < 0) {
		printf("++ DAC. Error : Communication error with DAC.\n");
		exit(0);
	}
				/* process errors */
	if (stat) {
		printf("** (%s) verified, all components executable.\n",fname);
		return(1);
	} 
	if (dacerrno == DAC_ER_EXECHANDLER) {
		printf("++ DAC. Error : Unable to start object : %s.\n", name);
		exit(0);
	}
	if (dacerrno == DAC_ER_EXECPROCESS) {
		printf("++ DAC. Error : Unable to execute process : (%s). Please double check $HOME/bin.\n", name);
		exit(0);
	} 

	printf("++ DAC. Unknown error, call system operator.\n") ;
	exit(0);
}


/*---------------------------------------------------------------------------
  Prototype   : void OpExit(void)
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
       printf("++ DAC. Error : Communication error with DAC.\n") ;
       exit(1) ;
    }
                                /* process errors */
   if (stat)
      printf("== Aborting DAC ...\n") ;
   else
      printf("++ DAC. Error : Unknown error, call system operator.\n") ;
}


/*---------------------------------------------------------------------------
  Prototype   : int waitdacexit(int oldsock)
  Parameters  : oldsock - a socket bound to prun_port 
  Returns     : If receive the message, DAC_EXIT,  return 1. Otherwise, 
                return 0.
  Called by   : main 
  Calls       : get_connection, readn close, printf  
  Notes       : Wait for DAC exit message from DAC.
  Date        : July' 1994 
  Coded by    : Feijian Sun 
  Modification:
---------------------------------------------------------------------------*/

int waitdacexit(oldsock)
int oldsock;
{
        int newsock;
        char exit_info;

        if ((newsock = get_connection(oldsock, NULL)) != -1)
                if (readn(newsock, (char *)&exit_info, sizeof(exit_info)))
                        if (exit_info == APP_EXITTED) {
                                close(newsock);
                                return(1);
                        }
        printf("++ DAC. Error : Waitting for DAC failed.\n");
        close(newsock);
        return(0);
}


