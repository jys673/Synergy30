/*.........................................................................*/
/*             DAC.C ------> Distributed Application Controller            */
/*.........................................................................*/

#include "dac.h"
#include <time.h>
/*---------------------------------------------------------------------------
  Prototype   : int main(int argc, char **argv)
  Parameters  : argv[1] -  .prcd file to be processed
		argv[2] -  SSH port to get DAC_exit_information
  Returns     : Never returns 
  Called by   : System
  Calls       : readFile, initDac, startExec, exit
  Notes       : The .prcd file is read and then control is passed to 
                startExec to process incoming op-codes. Send CID the
		app. exit status. 
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification: 07/94 Feijian Sun:
		Added a parameter: ssh_port as argv[2] to send SSH 
		DAC_exit_information.
	        02/13 Justin Y. Shi, updated for Ubuntu 10.4
---------------------------------------------------------------------------*/  

int main(argc, argv)
int argc; 
char **argv;
{
	term_flag = 0;

	if (argc != 6)
		exit(E_ARGCNT);

	ssh_port = atoi(argv[2]);		/* obtain SSH port */
	debug = atoi(argv[3]);
	parent_pid = atoi(argv[4]);		/* startDac caller's id */
	fdd_start = atoi(argv[5]);		/* FSUN 10/94. For FDD */
	r_debug = 0;
	total_cpu = 0;
	plist_cnt = 0;
	hlist_cnt = 0;
  	cpu_header = NULL;
	readFile(argv[1]);		
	initDac();
	OpSaveStatusAll();
	startExec();
}


/*---------------------------------------------------------------------------
  Prototype   : void readFile(char *fname)
  Parameters  : fname  - pointer to .prcd file name
  Returns     : -
  Called by   : main
  Calls       : fopen, fclose, fgets, exit, readProcess, readHandler, 
                readPrcd, updateLinks
  Notes       : The .prcd file is opened and appropriate functions are
                called to read portions of the file.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void readFile(fname)
char *fname;
{
   char app_name[80] ;

   strcpy(dacmapid,fname);
   if ((fp = fopen(fname, "r")) == NULL)
   {
      printf("++ DAC failed to open: (%s)\n",fname);
      exit(E_FILEOPEN) ;
   }
				/* obtain application name from .prcd file */
   if (fgets(app_name, sizeof(app_name), fp) == NULL)
      exit(E_APPNAME) ;
   app_name[strlen(app_name)-1] = '\0' ;
				/* read all process, handler and precedence*/
   readProcess() ;		/* information */
   readHandler() ;
   readPrcd() ;
   fclose(fp) ;
   updateLinks() ;		/* obtain host-address for all links */
}


/*---------------------------------------------------------------------------
  Prototype   : void readProcess(void)
  Parameters  : -
  Returns     : -
  Called by   : readFile
  Calls       : fgets, strcmp, malloc, fscanf, exit, readLocation, readParams,
                readArgs, readLinks
  Notes       : The information about a process are read and stored in a 
                linked list structure. The linked list is maintained in the
		order in which processes appear in the file, so that when the
		precedence information is read in the end, the list need not
		be searched to identify the process.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification: Added plist_cnt to record the length of process_list.
		Feijian Sun, 07/94.
---------------------------------------------------------------------------*/  

void readProcess()
{
   process_t **pp_p ;
   char dlim[5] ;

   plist_cnt = 0;
   process_list = NULL ;
   pp_p = &process_list ;
   while (fgets(dlim, sizeof(dlim), fp) != NULL)
    {				/* check if beginning of handlers */
       if (!strcmp(dlim, BO_HAND))
	{
	   *pp_p = NULL ;
	   return ;
	}			/* check if it's beginning of process */
       if (strcmp(dlim, BO_PROC))
	{	
	  if (!strcmp(dlim,"debu")) 
	  { /* Add automatic object generation for debugging sessions */
		r_debug = 1;
		if (fgets(dlim, sizeof(dlim), fp) == NULL)
			exit(E_EOF);
		if (fgets(dlim, sizeof(dlim), fp) == NULL)
			exit(E_EOF);
		if (!strcmp(dlim, BO_HAND))
		{
			*pp_p = NULL;
			return;
		}
	  }
	  else exit(E_DELIMITER) ;
	}
				/* get memory for process, store name */
       if ((*pp_p = (process_t *) malloc(sizeof(process_t))) == NULL)
	  exit(E_MALLOC) ;
       if (fscanf(fp, "%s", (*pp_p)->name) != 1)
	  exit(E_PROCNAME) ;
				/* read storage, execution locations,*/
       readLocation(*pp_p) ;	/* parameters, arguments and link infn */
       readParams(*pp_p) ;
       readArgs(*pp_p) ;
       readLinks(*pp_p) ;
       plist_cnt ++;	/* plist_cnt is the length of process_list. FSUN 07/94 */
       pp_p = &(*pp_p)->next ;
    }
   exit(E_EOF) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void readLocation(process_t *p_p)
  Parameters  : p_p - pointer to process
  Returns     : -
  Called by   : readProcess
  Calls       : fscanf, exit, inet_addr
  Notes       : The storage and execution locations for the process are read.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void readLocation(p_p)
process_t *p_p;
{
   char addr[20] ;
   
   if (fscanf(fp, "%s%s%s\n", addr, p_p->s_path, p_p->s_login) != 3)
      exit(E_STORAGELOC) ;
   p_p->S_host = inet_addr(addr) ;
				/* read execution location */
   if (fscanf(fp, "%s%s%s\n", addr, p_p->path, p_p->login) != 3)
      exit(E_EXECLOC) ;
   p_p->host = inet_addr(addr) ;
   push_host(p_p->host, p_p->proc_id, p_p->login);
}


/*---------------------------------------------------------------------------
  Prototype   : void readParams(process_t *p_p)
  Parameters  : p_p - pointer to process
  Returns     : - 
  Called by   : readProcess
  Calls       : fscanf, strcmp, exit
  Notes       : The process parameters : f, p, t, d, protocol are read.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void readParams(p_p)
process_t *p_p;
{
   char prot[20] ;
				/* read protocol, convert to code */
   if (fscanf(fp, "prot = %s\n", prot) != 1)
      exit(E_PROTOCOL) ;
   if (!strcmp(prot, TCP))
      p_p->protocol = DAC_PROT_TCP ;
   else
      exit(E_PROTOCOL) ;
				/* read process parameters */
   if (fscanf(fp, "f = %hu\np = %hu\nt = %hu\nd = %hu\n", &p_p->f, &p_p->p, 
	      &p_p->t, &p_p->d) != 4)
      exit(E_PARAMS) ;
}
   

/*---------------------------------------------------------------------------
  Prototype   : void readArgs(process_t *p_p)
  Parameters  : p_p - pointer to process
  Returns     : -
  Called by   : readProcess
  Calls       : fgets, strcmp, exit
  Notes       : The command line arguments for the process are read.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void readArgs(p_p)
process_t *p_p;
{
   char arg[ARG_LEN] ;

   p_p->arg_cnt = 0 ;
   while (fgets(arg, ARG_LEN, fp) != NULL)
    {				/* check if end of arguments is reached */
       if (!strcmp(arg, EO_ARGS))
	  return ;
				/* check if argument limit exceeded */
       if (p_p->arg_cnt >= ARGS_MAX)
	  exit(E_MOREARGS) ;
				/* store argument in argument list */
       arg[strlen(arg)-1] = '\0' ;
       strcpy(p_p->args[p_p->arg_cnt++], arg) ;
    }
   exit(E_EOF) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void readLinks(process_t *p_p)
  Parameters  : p_p - pointer to process
  Returns     : - 
  Called by   : readProcess
  Calls       : fgets, strcmp, sscanf, exit, malloc
  Notes       : The links associated with the process are read and stored in 
                a linked list. The links appear in the reverse order from how
		they are listed in the file.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification: The 'mode' for opening the file is made dummy. It's specified
                in the cnf_open call.
---------------------------------------------------------------------------*/  

void readLinks(p_p)
process_t *p_p;
{
   char s[300] ;
   link_t *p_l ;

   p_p->link = NULL ;
   p_p->link_cnt = 0 ;
   while (fgets(s, sizeof(s), fp) != NULL)
    {				/* check if end of link is reached */
       if (!strcmp(s, EO_LINK))
	  return ;		
				/* read link details */
       if ((p_l = (link_t *)malloc(sizeof(link_t))) == NULL)
	  exit(E_MALLOC) ;

       p_l->mode[0] = '\0' ;	/* mode, path only for FILE */
       p_l->path[0] = '\0' ;	/* mode not read from .prcd - dummy */

       if (sscanf(s, "%c %c %s%s%s", &p_l->type, &p_l->dirn, p_l->ref_name, 
		  p_l->obj_name, p_l->path) < 4)
	  exit(E_LINK) ;

       p_l->next = p_p->link ;
       p_p->link = p_l ;
       p_p->link_cnt++ ;
    }
   exit(E_EOF) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void readHandler(void)
  Parameters  : -
  Returns     : -
  Called by   : readFile
  Calls       : addHandlerList, strcmp, malloc, sscanf, inet_addr, exit
  Notes       : The hanlder information is read and stored in a linked list.
                The handlers appear in the reverse order from how they are
		listed in the fle.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void readHandler()
{
   char s[80], addr[20] ;
   handler_t *p_h ;

   hlist_cnt = 0;
   handler_list = NULL ;
   while (fgets(s, sizeof(s), fp) != NULL)
    {
				/* check if beginning of precedence infn. */
       if (!strcmp(s, BO_PRCD))
	  return ;

       if ((p_h = (handler_t *) malloc(sizeof(handler_t))) == NULL)
	  exit(E_MALLOC) ;
				/* store type, name, host and login */
       if (!r_debug) 
       {
		if (sscanf(s, "%c %s%s%s", &p_h->type, p_h->name, addr, 
		  p_h->login) != 4)
	  	exit(E_HANDLER) ;
		p_h->mapid[0] = 0;
       } else { /* Use mapid for debugging sessions. YS94 */
		if (sscanf(s, "%c %s%s%s%s", &p_h->type, p_h->name, addr, 
		  p_h->login, p_h->mapid) != 5)
		exit(E_HANDLER);
       }
       strcpy(p_h->inet_addr, addr);
       p_h->host = inet_addr(addr) ;
       p_h->state = READY ;
       p_h->next = handler_list ;
       handler_list = p_h ;
       hlist_cnt ++;
    }
   exit(E_EOF) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void readPrcd(void)
  Parameters  : -
  Returns     : -
  Called by   : readFile
  Calls       : getPrcdComponent, fgets, sprintf, sscanf, strcat, exit
  Notes       : The precedence information is stored in a linked list for
                each process. The precedence information is assumed to be 
		listed in the same order in which the processes are 
		described.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void readPrcd()
{
   process_t *p_p ;
   prcd_t *p_pr ;
   char s[300], name[NAME_LEN], fmt1[100], fmt2[100] ;

   for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
      p_p->retflag = NO ;

   for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
    {
       if (fgets(s, sizeof(s), fp) == NULL)
	  exit(E_EOF) ;
       p_p->prcd = NULL ;
       sprintf(fmt1, "%s: ", p_p->name) ;
				
       do {			/* read 1 precedent at a time */
	  sprintf(fmt2, "%s%s", fmt1, "%s") ;
	  if (sscanf(s, fmt2, name) != 1)
	     break ;
				/* add precedent to list */
	  if ((p_pr = (prcd_t *) malloc(sizeof(prcd_t))) == NULL)
	     exit(E_MALLOC) ;
	  if ((p_pr->process = getProcess(name)) == NULL)
	     exit(E_PRCD) ;
	  
	  p_pr->process->retflag = YES ;
	  p_pr->next = p_p->prcd ;
	  p_p->prcd = p_pr ;
	  strcat(fmt1, "%*s") ;	/* modify sscanf format */
       } while (1) ;
       p_p->state = p_p->prcd == NULL ? READY : WAITING ;
    }
}


/*---------------------------------------------------------------------------
  Prototype   : process_t *getProcess(char *name)
  Parameters  : name - name of process 
  Returns     : pointer to the process structure of 'name'
  Called by   : readPrcd
  Calls       : strcmp
  Notes       : Process-list is searched for 'name' and the component is
                returned.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

process_t *getProcess(name)
char *name;
{
   process_t *p_p ;
				/* Search process-list for process */
   for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
      if (!strcmp(p_p->name, name))
	 return p_p ;
   return NULL ;
}


/*---------------------------------------------------------------------------
  Prototype   : void updateLinks(void)
  Parameters  : -
  Returns     : -
  Called by   : readFile
  Calls       : exit, getPipeHost, getHandlerHost
  Notes       : The 'host' entries for the links are found out for PIPE-out
                and FILE, SPACE.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void updateLinks()
{
   process_t *p_p ;
   link_t *p_l ;

   for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
      for (p_l = p_p->link ; p_l != NULL ; p_l = p_l->next)
	 if (p_l->type == O_PIPE && p_l->dirn == OUT)
	  {
	     if (!(p_l->host = getPipeHost(p_l->obj_name)))
		exit(E_LINKHOST) ;
	     strcpy(p_l->login, getPipeLogin(p_l->obj_name));
	     if (debug) 
printf(" Pipe destination update: ref_name(%s) host (%lu) login (%s) \n",
		p_l->ref_name, p_l->host, p_l->login);
	  }
	 else if (p_l->type == O_FILE || p_l->type == O_SPACE)
	 {
	    if (!(p_l->host = getHandlerHost(p_l->obj_name)))
	       exit(E_LINKHOST) ;
	    strcpy(p_l->login, getHandlerLogin(p_l->obj_name));
	    if (debug) 
printf(" Handler desitnatino update: refnm (%s) (host(%lu) login (%s) \n",
		p_l->ref_name, p_l->host, p_l->login);
	 }
}


/*---------------------------------------------------------------------------
  Prototype   : u_long getPipeHost(char *name)
  Parameters  : name - object name of the pipe
  Returns     : address of process with 'name' as INPUT pipe [or] 0 on error
  Called by   : updateLinks
  Calls       : strcmp
  Notes       : The links of all the processes are searched to identify
                the one with 'name' as INPUT pipe.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

u_long getPipeHost(name)
char *name;
{
   process_t *p_p ;
   link_t *p_l ;
				/* search all processes for name */
   for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
      for (p_l = p_p->link ; p_l != NULL ; p_l = p_l->next)
	 if (p_l->type == O_PIPE && p_l->dirn == IN && 
	     (!strcmp(p_l->obj_name, name)))
	    return p_p->host ;
   return 0 ;
}

char *getPipeLogin(name)
char *name;
{
   process_t *p_p ;
   link_t *p_l ;
				/* search all processes for name */
   for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
      for (p_l = p_p->link ; p_l != NULL ; p_l = p_l->next)
	 if (p_l->type == O_PIPE && p_l->dirn == IN && 
	     (!strcmp(p_l->obj_name, name)))
	    return p_p->login ;
   return 0 ;
}

/*---------------------------------------------------------------------------
  Prototype   : u_long getHandlerHost(char *name)
  Parameters  : name - name of FAH [or] TSH
  Returns     : address of handler with 'name' [or] 0 on error
  Called by   : updateLinks
  Calls       : strcmp
  Notes       : The handler list is searched to locate the TSH/FAH with
                the 'name'.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

u_long getHandlerHost(name)
char *name;
{
   handler_t *p_h ;
				/* search handler-list for name */
   for (p_h = handler_list ; p_h != NULL ; p_h = p_h->next)
      if (!strcmp(p_h->name, name))
	 return p_h->host ;
   return 0 ;
}

char *getHandlerLogin(name)
char *name;
{
   handler_t *p_h ;
				/* search handler-list for name */
   for (p_h = handler_list ; p_h != NULL ; p_h = p_h->next)
      if (!strcmp(p_h->name, name))
	 return p_h->login ;
   return 0 ;
}

/*---------------------------------------------------------------------------
  Prototype   : void initDac(void)
  Parameters  : -
  Returns     : -
  Called by   : main
  Calls       : sprintf, getpwuid, exit, strcpy, mapDacport, signal
  Notes       : Initialisations required before DAC can accept op-codes
                are performed. The app_id of the application is created by
                host-address+pid of DAC. The distributed application is
		identified by this sequence obtained.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification: changed to inform CID the application status. Feijian Sun, 07/94.
---------------------------------------------------------------------------*/  

void initDac()
{
   struct passwd *p_passwd ;
   char pid[10] ;
   cid_saveappmsg_it out;
   int t; /* socket to send CID the app. status. FSUN 07/94 */
   time_t ttime;

   signal(SIGTERM, sigtermHandler);
				/* create app_id = hostid-pid */
   sprintf(app_id, "%lu-%d", sng_gethostid(), getpid()) ;

// printf(" DAC896 appid(%s) \n",app_id);

				/* obtain login of user */
   if ((p_passwd = getpwuid(getuid())) == NULL)
      exit(E_LOGIN) ;
   strcpy(login, p_passwd->pw_name) ;
   mapDacport() ;
   start_flag = NO ;

				/* Send the application message in CID */
   strcpy(out.app_id, dacmapid);
   gethostname(hostname, sizeof(hostname)); 
   out.dacport = ret_port;
   out.app_status = APP_NOT_READY;
   out.num_process = htons(total_cpu);
   if (debug) printf("appid(%s) dacport(%d) p(%d)\n",out.app_id, out.dacport, out.num_process);
   time(&ttime);
   strcpy(out.start_time, ctime(&ttime));
   elapsed_time = time((long *)0); /* define starting time */


   if (!saveappmsg(hostname, getpwuid(getuid())->pw_name, out)) 
	printf("DAC saving app failure. Call system operator.\n");
}


/*---------------------------------------------------------------------------
  Prototype   : void sigtermHandler(void)
  Parameters  : - 
  Returns     : -
  Called by   : initDac   
  Calls       : unmapDacport, killAllHandlers, killAllHandlers
  Notes       : DAC port is unregistered from PMD and all processes, handlers
                associated with the application are killed.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void sigtermHandler()
{
printf(" DAC caught SINTERM\n");
   unmapDacport() ;
printf(" DAC umapped port \n");
   killAllProcesses() ;
   cleanAllProcesses() ; 
printf(" DAC killed all processes\n");
   killAllHandlers() ;
printf(" DAC killed all objects\n");
   send_cid_appstatus(hostname, APP_KILLED);
printf(" DAC sent in KILLED status\n");
   exit(0) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void mapDacport(void)
  Parameters  : - 
  Returns     : -
  Called by   : initDac
  Calls       : sprintf, get_socket, bind_socket, exit, sngd_map
  Notes       : The pid of DAC along with it's port # is registered with the
                PMD. The interface program can now contact the PMD to obtain
		the port # of DAC.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification: Changed to register the pid of DAC and it's port # with the 
		SNGD. Feijian Sun, 07/94.
---------------------------------------------------------------------------*/  

void mapDacport()
{
   /* obtain DAC return port */
   if ((oldsock = get_socket()) != -1)
      if (!(ret_port = bind_socket(oldsock, 0)))
	 exit(E_NOPORT) ;
   sprintf(dacmapid,"%s+%d$%d",dacmapid,parent_pid,getpid());
	/* register with PMD */
   if (!sngd_map(dacmapid, ret_port, login, (u_short)PMD_PROT_TCP))
      exit(E_NOMAP) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void unmapDacport(void)
  Parameters  : -
  Returns     : -
  Called by   : startExec 
  Calls       : sprintf, sngd_unmap
  Notes       : The <"pid", port> is unregistered from the PMD.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification: Changed to unregistered the <"pid", port> from SNGD.
		Feijian Sun, 07/94.
---------------------------------------------------------------------------*/  

void unmapDacport()
{
	/* unregister from PMD */
   sngd_unmap(dacmapid, 0, login, (u_short)PMD_PROT_TCP) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void startExec(void)
  Parameters  : -
  Returns     : - 
  Called by   : main
  Calls       : ntohs, exit, close, get_connection, o-functions, unmapDacport 
  Notes       : If a valid op-code is received, then the corresponding
                function is called. DAC_OP_EXIT_COMPONENT is alone
		received from CID. All other op's are from the user-
		interface.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification: Added a terminate_flag to make DAC end by itself when all
		processes exit.
		Added OpGetProclist to send process list to PCHECK.
		Feijian Sun, 07/94.
---------------------------------------------------------------------------*/  

void startExec()
{
	static void (*op_func[])() = {
		OpStart, OpSaveStatus, OpSaveStatusAll, OpVerifyApp, OpExitProcess, 
		OpGetStatus, OpGetState, OpGetAppstate, OpKill, OpKillAll, 
		OpFddChangeProc, OpGetProclist, OpGetHandlerlist, OpGetCpulist, 
		OpExit
	};
	u_short this_op;
	int sockssh;
   	char exit_info;


	if (debug) printf("in DAC_startExec term_flag: (%d)\n", term_flag);

	while (!term_flag) {	/* DAC itself will terminate when all processes exit */ 
					/* get a connection for request */
		if ((newsock = get_connection(oldsock, NULL)) == -1)
		{
			printf(" Network problem. Try it later. \n");
   			send_cid_appstatus(hostname, APP_KILLED);
			exit(E_CONNECT);
		}
					/* read operation code */
		if (!readn(newsock, (char *)&this_op, sizeof(u_short))) {
			close(newsock);
			continue;
		}
		this_op = ntohs(this_op);
					/* validate operation & process */
		if (this_op >= DAC_OP_MIN && this_op <= DAC_OP_MAX)
			(*op_func[this_op - DAC_OP_MIN])();
		close(newsock);
	}

        killAllProcesses();	/* Before exit, kill all processes, handlers */
        cleanAllProcesses();
        killAllHandlers();
        unmapDacport();
				/* Inform PRUN DAC exited */
        if ((sockssh = get_socket()) != -1)
                if (do_connect(sockssh, sng_gethostid(), ssh_port)) {

			if (debug) printf("DAC sending exit_flag\n");

			exit_info = APP_EXITTED;
                        writen(sockssh, (char *)&exit_info, sizeof(char));
		}

	if (debug) printf("DAC exit\n");

}


/*---------------------------------------------------------------------------
  Prototype   : void OpStart(void)
  Parameters  : -
  Returns     : -
  Called by   : StartExec
  Calls       : execAllHandlers, execReadyProcesses, writen, send_cid_appstatus
  Notes       : This op must be received for DAC to start executing the
                handlers and processes. If a handler could not be executed, 
		it's considered a critical error and DAC does not proceed.
		But, if a process could not be executed, it's ignored.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification: Added send_cid_appstatus to inform CID the application
		status. Feijian Sun, 07/94.
---------------------------------------------------------------------------*/  

void OpStart()
{
   dac_start_ot out ;
				/* check if DAC already started */
   if (start_flag == YES)
    {
       out.status = htons(FAILURE) ;
       out.error = htons(DAC_ER_STARTED) ;
    }				/* execute handlers - error is critical */
   else if (!execAllHandlers())
    {
       out.status = htons(FAILURE) ;
       out.error = htons(DAC_ER_EXECHANDLER) ;
    }
   else
    {				/* execute processes - error is non-critical */
       start_flag = YES ;
       execReadyProcesses() ;
       out.status = htons(SUCCESS) ;
       out.error = htons(DAC_ER_NOERROR) ;
    }				/* send status to user-interface */
   if (debug) printf("DAC startup status (%d) \n",out.error);
   writen(newsock, (char *)&out, sizeof(dac_start_ot)) ;
   if (process_list == NULL)
   {
   	send_cid_appstatus(hostname, APP_EXITTED);
	if (debug) printf(" Process list empty. Exiting ...\n");
	exit(1);
   }
   send_cid_appstatus(hostname, APP_RUNNING);
}
  

/*---------------------------------------------------------------------------
  Prototype   : void OpSaveStatus(void)
  Parameters  : -
  Returns     : -
  Called by   : startExec
  Calls       : readn, writen, strcmp
  Notes       : As a result of this op-code, the retstatus of all the
                processes will be saved. This op-code could only be sent 
		before sending DAC_OP_START
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpSaveStatus()
{
   dac_savestatus_it in ;
   dac_savestatus_ot out ;
   process_t *p_p ;

   if (!readn(newsock, (char *)&in, sizeof(dac_savestatus_it)))
      return ;
   if (start_flag == YES)	/* check if DAC already started */
    {
       out.status = htons(FAILURE) ;
       out.error = htons(DAC_ER_STARTED) ;
    }
   else
    {
       out.status = htons(FAILURE) ;	/* assume error */
       out.error = htons(DAC_ER_NOPROCESS) ;
				/* update retflag for the process */
       for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
	  if (!strcmp(p_p->name, in.name))
	   {
	      p_p->retflag = YES ;
	      out.status = htons(SUCCESS) ;
	      out.error = htons(DAC_ER_NOERROR) ;
	      break ;
	   }
    }				/* returns status to interface */
   writen(newsock, (char *)&out, sizeof(dac_savestatus_ot)) ;
}

	    
/*---------------------------------------------------------------------------
  Prototype   : void OpSaveStatusAll(void)
  Parameters  : -
  Returns     : -
  Called by   : main 
  Calls       : writen
  Notes       : Return status for all processes will be obtained by DAC. 
                This op-code could be sent only if the DAC_OP_START is
		already not sent.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification: This func. will be called only by main.
		Feijian Sun, 08/94.
---------------------------------------------------------------------------*/  

void OpSaveStatusAll()
{
   dac_savestatusall_ot out ;
   process_t *p_p ;

   for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
	  p_p->retflag = YES ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpVerifyApp(void)
  Parameters  : -
  Returns     : -
  Called by   : startExec
  Calls       : writen, readn, verifyProcess, verifyHandler, , strcpy
  Notes       : It's checked whether permissions exist to execute all the 
                components of the application.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification: The process/handler name is also sent back.
---------------------------------------------------------------------------*/  

void OpVerifyApp()
{
   dac_verifyapp_ot out ;
   process_t *p_p ;
   handler_t *p_h ;
				/* verify all processes */
   for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
      if (!verifyProcess(p_p))
       {
	  out.status = htons(FAILURE) ;
	  out.error = htons(DAC_ER_EXECPROCESS) ;
	  strcpy(out.name, p_p->name) ;
	  writen(newsock, (char *)&out, sizeof(dac_verifyapp_ot)) ;
	  send_cid_appstatus(hostname, APP_PROC_NOT_READY);
	  return ;
       }			/* verify all handlers */
   for (p_h = handler_list ; p_h != NULL ; p_h = p_h->next)
      if (!verifyHandler(p_h))
       {
	  out.status = htons(FAILURE) ;
	  out.error = htons(DAC_ER_EXECHANDLER) ;
	  strcpy(out.name, p_h->name) ;
	  writen(newsock, (char *)&out, sizeof(dac_verifyapp_ot)) ;
	  send_cid_appstatus(hostname, APP_OBJ_NOT_READY);
	  return ;
       }
   out.status = htons(SUCCESS) ;
   out.error = htons(DAC_ER_NOERROR) ;
   strcpy(out.name, "") ;
   writen(newsock, (char *)&out, sizeof(dac_verifyapp_ot)) ;
   send_cid_appstatus(hostname, APP_READY);
}


/*---------------------------------------------------------------------------
  Prototype   : void OpExitProcess(void)
  Parameters  : -
  Returns     : -
  Called by   : startExec
  Calls       : readn, strcmp, modifyPrcd, execReadyProcesses, send_cid_appstatus
  Notes       : This op-code is sent by CID when a process exits. If the
                process had been a precedent, all the currently ready 
		processes are executed.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification: Changed to set a term_flag and inform CID the application
		status when all processes exit.
		Added send_cid_appstatus to inform CID the application
		status.
		Feijian Sun, 07/94.
---------------------------------------------------------------------------*/  

void OpExitProcess()
{
	dac_exitprocess_it in;
	process_t *p_p, *p_q;
	int ret_st;
				/* get connection from CID & get status */
	if (!readn(newsock, (char *)&in, sizeof(dac_exitprocess_it)))
		return;
 	ret_st = ntohs(in.retstatus);
				/* update state of the process */
	for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
	    if (!strcmp(in.name, p_p->name)) {
		p_p->elapsed_time = time((long *)0) - p_p->elapsed_time;
		if (p_p->state != KILLED) {
		    switch (ret_st) {
			case EXITTED: p_p->state = EXITTED;
		    		      p_p->retstatus = ntohs(in.retstatus);
				      break;
			case KILLED:  p_p->state = KILLED;
		    		      p_p->retstatus = 0;
				      break;
			case FAILED:  p_p->state = FAILED;
		    		      p_p->retstatus = 0;
				      break;
			default:      p_p->state = EXITTED;
		    		      p_p->retstatus = 0;
		    }
		}
				/* execute processes that have become ready */
		if (modifyPrcd(p_p->name)) // per application
	    	    execReadyProcesses();
		/* Set the term_flag to 0 when no active processes */
		else {
		    term_flag = 1;
		    for (p_q = process_list ; p_q != NULL ; p_q = p_q->next)
	                if (p_q->state == WAITING || p_q->state == READY || 
			        p_q->state== RUNNING ||
				(p_q->state == STARTED && pingProcess(p_q))) 
			{
			    term_flag = 0;
			    break;
		        } 
	    	}
		break;	
	    }
	if (term_flag == 1)
	{
		elapsed_time = time((long *)0) - elapsed_time;
		send_cid_appstatus(hostname, APP_EXITTED);
	}
}


/*---------------------------------------------------------------------------
  Prototype   : void OpGetStatus(void)
  Parameters  : -
  Returns     : -
  Called by   : startExec
  Calls       : readn, writen, strcmp
  Notes       : The exit status of the process is returned.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpGetStatus()
{
   dac_getstatus_it in ;
   dac_getstatus_ot out ;
   process_t *p_p ;
				/* read process name */
   if (!readn(newsock, (char *)&in, sizeof(dac_getstatus_it)))
      return ;
   if (start_flag == NO)	/* DAC not started */
    {
       out.status = htons(FAILURE) ;
       out.error = htons(DAC_ER_NOTSTARTED) ;
    }
   else
    {
       out.status = htons(FAILURE) ;	/* assume error */
       out.error = htons(DAC_ER_NOPROCESS) ;
				/* locate process and return status */
       for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
	  if (!strcmp(in.name, p_p->name))
	   {
	      out.status = htons(SUCCESS) ;
	      out.error = htons(DAC_ER_NOERROR) ;
	      out.state = htons(p_p->state) ;
	      out.retstatus = htons(p_p->retstatus) ;
	      break ;
	   }
    }				/* send exit status to interface */
   writen(newsock, (char *)&out, sizeof(dac_getstatus_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpGetState(void)
  Parameters  : -
  Returns     : -
  Called by   : startExec
  Calls       : readn, writen, strcmp
  Notes       : The state of the process is returned.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpGetState()
{
   dac_getstate_it in ;
   dac_getstate_ot out ;
   process_t *p_p ;
				/* obtain process name */
   if (!readn(newsock, (char *)&in, sizeof(dac_getstate_it)))
      return ;

   out.status = htons(FAILURE) ;	/* assume error */
   out.error = htons(DAC_ER_NOPROCESS) ;
				/* locate process and return state */
   for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
      if (!strcmp(in.name, p_p->name))
       {
	  out.status = htons(SUCCESS) ;
	  out.error = htons(DAC_ER_NOERROR) ;
	  if ((p_p->state == STARTED) && (!pingProcess(p_p)))
	     out.state = htons(EXITTED) ;
	  else
	     out.state = htons(p_p->state) ;
	  break ;
       }
   writen(newsock, (char *)&out, sizeof(dac_getstate_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpGetAppState(void)
  Parameters  : -
  Returns     : -
  Called by   : startExec
  Calls       : writen
  Notes       : If no process is WAITING or READY or RUNNING and if a STARTED
                process is still running then application is not completed.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpGetAppstate()
{
   dac_getappstate_ot out ;
   process_t *p_p ;

   out.status = htons(SUCCESS) ;
   out.error = htons(DAC_ER_NOERROR) ;
   out.state = htons(COMPLETED) ;	/* search process list */
   for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
      if (p_p->state == WAITING || p_p->state == READY || p_p->state== RUNNING)
       {
	  out.state = htons(NOT_COMPLETED) ;
	  break ;
       }
      else if (p_p->state == STARTED)
	 if (pingProcess(p_p))
	  {
	     out.state = htons(NOT_COMPLETED) ;
	     break ;
	  }
   writen(newsock, (char *)&out, sizeof(dac_getappstate_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpKill(void)
  Parameters  : -
  Returns     : -
  Called by   : startExec
  Calls       : readn, writen, strcmp, killProcess
  Notes       : The CID corresponding to the component is informed to kill
                the process. The processes that depend on this are not 
		executed now, but only when the CID comes back to 
		OpExitProcess.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpKill()
{
   dac_kill_it in ;
   dac_kill_ot out ;
   process_t *p_p ;
				/* read process name */
   if (!readn(newsock, (char *)&in, sizeof(dac_kill_it)))
      return ;
				/* search process in process list */
   for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
      if (!strcmp(in.name, p_p->name))
       {
	  if (p_p->state == RUNNING || p_p->state == STARTED)
	     killProcess(p_p) ; /* kill and update state */
	  if (p_p->state != EXITTED)
	     p_p->state = KILLED ;
	  out.status = htons(SUCCESS) ;
	  out.error = htons(DAC_ER_NOERROR) ;
	  writen(newsock, (char *)&out, sizeof(dac_kill_ot)) ;
	  return ;
       }
   out.status = htons(FAILURE) ;	/* invalid process name */
   out.error = htons(DAC_ER_NOPROCESS) ;
   writen(newsock, (char *)&out, sizeof(dac_kill_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpKillAll(void)
  Parameters  : -
  Returns     : -
  Called by   : startExec
  Calls       : killAllComponents, killAllHandlers, writen
  Notes       : All processes and all handlers are killed.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification: Set term_flag and inform CID the application status,
                then return. Feijan Sun, 07/94.
---------------------------------------------------------------------------*/  

void OpKillAll()
{
   	dac_killall_ot out ;
 
	killAllProcesses() ;
        cleanAllProcesses() ; // Added to protect premature cleaning in CID.
   	killAllHandlers() ;
   	out.status = htons(SUCCESS) ;
   	out.error = htons(DAC_ER_NOERROR) ;
   	writen(newsock, (char *)&out, sizeof(dac_killall_ot)) ;

        /* DAC exits after sending all process exit_information */
        term_flag = 1;
	elapsed_time = time((long *)0) - elapsed_time;
        send_cid_appstatus(hostname, APP_KILLED);

}


/*---------------------------------------------------------------------------
  Prototype   : void OpExit(void)
  Parameters  : -
  Returns     : -
  Called by   : startExec
  Calls       : sprintf, getpid, killAllHandlers, killAllProcesses, writen,
		send_cid_appstatus
  Notes       : On receiving this op-code, all processes, handlers are killed.
                Set term_flag to make DAC exit. 
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification: Set term_flag and inform CID the application status, 
		then return. Feijan Sun, 07/94. 
---------------------------------------------------------------------------*/  

void OpExit()
{
	dac_exit_ot out ;

	killAllProcesses() ;
	cleanAllProcesses() ;  // Added for FDD. JYS04 
	killAllHandlers() ;
	out.status = htons(SUCCESS) ;
	out.error = htons(DAC_ER_NOERROR) ;
	writen(newsock, (char *)&out, sizeof(dac_exit_ot)) ;

	/* DAC exits after sending all process exit_information */
	term_flag = 1;
	send_cid_appstatus(hostname, APP_KILLED);
}


/*---------------------------------------------------------------------------
  Prototype   : int verifyProcess(process_t *p_p)
  Parameters  : p_p - pointer to process
  Returns     : 1 - process can be executed [or] 
                0 - process cannot be executed
  Called by   : OpVerifyApp
  Calls       : get_socket, do_connect, readn, writen, ntohs, strcpy
  Notes       : Connection is made to the appropriate CID and execution 
                permissions are verified for the process.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int verifyProcess(p_p)
process_t *p_p;
{
   cid_verifyprocess_it out ;
   cid_verifyprocess_ot in ;
   u_short op ;
   int sd ;

   p_p->elapsed_time = 0; /* initialize */
   op = htons(CID_OP_VERIFY_PROCESS) ;
   if ((sd = get_socket()) == -1)
      return 0 ;		/* fill operation parameters */
   strcpy(out.login, p_p->login) ;
   strcpy(out.rlogin, login) ;
   strcpy(out.path, p_p->path) ;
				/* send request and verify process */
   printf("Verifying process [|(%s)|%s\n", out.login,out.path);
   sprintf(mapid,"sng$cid$%s",p_p->login);
   if (!(cidport=
        pmd_getmap(mapid,p_p->host,(u_short)PMD_PROT_TCP)))
   {
        printf("Remote host lookup failure. \n");
        return 0;
   }
   if (do_connect(sd, p_p->host, cidport))
      if (writen(sd, (char *)&op, sizeof(u_short)))
	 if (writen(sd, (char *)&out, sizeof(cid_verifyprocess_it)))
	    if (readn(sd, (char *)&in, sizeof(cid_verifyprocess_ot)))
	       if (ntohs(in.status) == SUCCESS)
		{
		   close(sd) ;
		   return 1 ;
		}
		else printf(" status error \n");
	    else printf(" readn error \n");
         else printf(" writen error \n");
      else printf(" writen op error \n");
   else printf(" do_connect error \n");
   close(sd) ;
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int verifyHandler(handler_t *p_h)
  Parameters  : p_h - pointer to handler
  Returns     : 1 - handler can be executed [or] 
                0 - handler cannot be executed
  Called by   : OpVerifyApp
  Calls       : get_socket, do_connect, readn, writen, ntohs, strcpy, htons
  Notes       : Connection is made to the appropriate CID and execution 
                permissions are verified for the handler.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int verifyHandler(p_h)
handler_t *p_h;
{
   cid_verifyhandler_it out ;
   cid_verifyhandler_ot in ;
   u_short op ;
   int sd ;

   op = htons(CID_OP_VERIFY_HANDLER) ;
   if ((sd = get_socket()) == -1)
      return 0 ;		/* fill operation parameters */
   strcpy(out.login, p_h->login) ;
   strcpy(out.rlogin, login) ;
   out.type = p_h->type == FAH ? htons(CID_TYPE_FAH) : htons(CID_TYPE_TSH) ;
				/* send request and verify handler */
   sprintf(mapid,"sng$cid$%s",p_h->login);
   if (!(cidport=pmd_getmap(mapid,p_h->host,(u_short)PMD_PROT_TCP)))
   {
        printf("Remote host lookup failure. \n");
        return 0;
   }
   if (do_connect(sd, p_h->host, cidport))
      if (writen(sd, (char *)&op, sizeof(u_short)))
	 if (writen(sd, (char *)&out, sizeof(cid_verifyhandler_it)))
	    if (readn(sd, (char *)&in, sizeof(cid_verifyhandler_ot)))
	       if (ntohs(in.status) == SUCCESS)
		{
		   close(sd) ;
		   return 1 ;
		}
   close(sd) ;
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : void execAllHandlers(void)
  Parameters  : -
  Returns     : -
  Called by   : OpStart
  Calls       : get_socket, do_connect, sendHandlerToCID, sendToHandler,
                killAllHandlers, exit
  Notes       : Appropriate CID's are contacted and all the handlers
                are executed. If even one handler could not be executed,
		all the previously started ones are killed.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification: 
---------------------------------------------------------------------------*/  
int execAllHandlers()
{
   handler_t *p_h ;
   int sd ;

   for (p_h = handler_list ; p_h != NULL ; p_h = p_h->next)
    {				/* connect to CID, execute handler */
       if ((sd = get_socket()) != -1)
       {
          sprintf(mapid,"sng$cid$%s",p_h->login);

          if (!(cidport =
                pmd_getmap(mapid,p_h->host,(u_short)PMD_PROT_TCP)))
          {
              printf("Remote host lookup failure.\n");
              return 0;
          }
          if (do_connect(sd, p_h->host, cidport))
	     if (sendHandlerToCID(sd, p_h))
	{
		if (sendToHandler(sd, p_h))
		   {
		      p_h->state = STARTED ;
			/* modify mapid for non-debugging case */
	 	      sprintf(p_h->mapid , "%s$%s",app_id,p_h->name);
		      close(sd) ;
		      continue ;
		   }
	}
       }
       killAllHandlers() ;	/* exec failed, so kill all other handlers */
       return 0 ;
    }
   return 1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int sendHandlerToCID(int sd, handler_t *p_h)
  Parameters  : sd   - socket descriptor of connection to CID
                p_h  - pointer to handler
  Returns     : 1 - handler successfully sent to CID
                0 - error in communication with CID
  Called by   : execAllHandlers
  Calls       : htons, strcpy, writen, readn, ntohs
  Notes       : Details about the handler required by CID are sent.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int sendHandlerToCID(sd, p_h)
int sd; 
handler_t *p_h;
{
   cid_exectsh_ot in ;
   cid_exectsh_it out ;
   u_short op ;
				/* prepare handler infn. to be sent to CID */
   op = htons(p_h->type == FAH ? CID_OP_EXEC_FAH : CID_OP_EXEC_TSH) ;
   strcpy(out.name, p_h->name) ;
   out.fdd_start = htons(fdd_start);	/* FSUN 10/94. For FDD */
   if (r_debug) 
   {
	strcpy(out.appid, p_h->mapid);
	out.r_debug = htons(r_debug);
   } else {
   	strcpy(out.appid, app_id) ;
   	out.r_debug = 0;
   }
   strcpy(out.rlogin, login) ;
   strcpy(out.login, p_h->login) ;
   strcpy(out.name, p_h->name) ;
/*
printf(" 896DAC sending to CID. mapid(%s) name(%s)\n", out.appid, out.name);
*/

				/* send handler infn. & check status */
   if (writen(sd, (char *)&op, sizeof(u_short)))
      if (writen(sd, (char *)&out, sizeof(cid_exectsh_it)))
	 if (readn(sd, (char *)&in, sizeof(cid_exectsh_ot)))
	    if (ntohs(in.status) == SUCCESS) {
		p_h->proc_id = ntohl(in.proc_id);
	        return 1 ;
	    }
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int sendToHandler(int sd, handler_t *p_h)
  Parameters  : sd   - socket descriptor of connection to handler
                p_h  - pointer to handler
  Returns     : 1 - handler details successfully sent to handler
                0 - error in communication with handler
  Called by   : execAllHandlers
  Calls       : strcpy, writen, readn, ntohs, exit
  Notes       : Details required by handler are sent to it.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int sendToHandler(sd, p_h)
int sd; 
handler_t *p_h;
{
   tsh_start_ot in ;
   tsh_start_it out ;
				/* prepare infn. to be sent to handler */
   strcpy(out.appid, app_id) ;
   strcpy(out.name, p_h->name) ;
				/* send infn to handler & check status */
   if (writen(sd, (char *)&out, sizeof(tsh_start_it)))
      if (readn(sd, (char *)&in, sizeof(tsh_start_ot)))
	 if (ntohs(in.status) == SUCCESS)
	  {
	     p_h->port = in.port ;
	     return 1 ;
	  }
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : void killAllHandlers(void)
  Parameters  : -
  Returns     : -
  Called by   : execAllHandlers
  Calls       : htons, get_socket, do_connect, readn, writen, close, strcpy 
  Notes       : All the handlers that have been started are killed.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void killAllHandlers()
{
   tsh_exit_ot in ;
   handler_t *p_h ;
   int sd ;
   u_short op ;

   for (p_h = handler_list ; p_h != NULL ; p_h = p_h->next)
    {
       if (p_h->state != STARTED) /* check if handler is running */
	  break ;
       op = htons(p_h->type == FAH ? FAH_OP_EXIT : TSH_OP_EXIT) ;
       if ((sd = get_socket()) != -1)
	  if (do_connect(sd, p_h->host, p_h->port))
	     if (writen(sd, (char *)&op, sizeof(u_short)))
		readn(sd, (char *)&in, sizeof(tsh_exit_ot)) ;
	     else printf("++ DAC: Cannot kill handler (%s)[writen]\n",p_h->name);
	  else printf("++ DAC: Cannot kill handler (%s)[connect]\n",p_h->name);
       else printf("++ DAC: Cannot kill handler (%s)[socket]\n",p_h->name);

       p_h->state = KILLED ;	/* modify state of handler */
       close(sd) ;
    }
}


/*---------------------------------------------------------------------------
  Prototype   : void execReadyProcesses(void)
  Parameters  : - 
  Returns     : - 
  Called by   : OpStart, OpExitProcess
  Calls       : get_socket, do_connect, sendProcessToCID, sendToProcess,
                close
  Notes       : Connection is made to the appropriate CID and then functions
                are called to communicate with CID and then the process.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void execReadyProcesses()
{
   process_t *p_p ;
   int sd ;

   for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
    {
       if (p_p->state != READY)	/* check if process is ready */
	  continue ;
       p_p->state = FAILED ;
				/* connect to CID */
       if ((sd = get_socket()) != -1)
       {
          sprintf(mapid,"sng$cid$%s",p_p->login);
          if (!(cidport =
              pmd_getmap(mapid,p_p->host,(u_short)PMD_PROT_TCP)))
          {
              printf("Remote host lookup failure for processes. \n");
              continue;
          }

          if (do_connect(sd, p_p->host, cidport))
	     if (sendProcessToCID(sd, p_p))
		if (sendToProcess(sd, p_p))
		{
		   p_p->state = p_p->retflag == YES ? RUNNING : STARTED ;
		   p_p->elapsed_time = time((long *)0); /* start timer */
		}
       } 
       close(sd) ;
    }
}

 
/*---------------------------------------------------------------------------
  Prototype   : int sendProcessToCID(int sd, process_t *p_p)
  Parameters  : sd   - socket descriptor of connection to CID
                p_p  - pointer to process
  Returns     : 1 - process successfully sent to CID
                0 - error in communication with CID
  Called by   : execReadyProcesses
  Calls       : htons, strcpy, writen, readn, ntohs
  Notes       : Details about the process required by CID are sent.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int sendProcessToCID(sd, p_p)
int sd; 
process_t *p_p;
{
   cid_execcomponent_it out ;
   cid_execcomponent_ot in ;
   u_short op ;
   int i ;
				/* fill process infn. to be sent */
   op = htons(CID_OP_EXEC_COMPONENT) ;
   strcpy(out.login, p_p->login) ;
   strcpy(out.rlogin, login) ;
   strcpy(out.appid, app_id) ;
   strcpy(out.name, p_p->name) ;
   strcpy(out.path, p_p->path) ;
   out.arg_cnt = htons(p_p->arg_cnt) ;
   out.fdd_start = htons(fdd_start);

   out.port = p_p->retflag == YES ? ret_port : 0 ;
   for (i = 0 ; i < p_p->arg_cnt ; i++)
      strcpy(out.args[i], p_p->args[i]) ;
				/* send infn to CID, check status */
   if (writen(sd, (char *)&op, sizeof(u_short)))
      if (writen(sd, (char *)&out, sizeof(cid_execcomponent_it)))
	 if (readn(sd, (char *)&in, sizeof(cid_execcomponent_ot)))
	    if (ntohs(in.status) == SUCCESS) {
		p_p->proc_id = ntohl(in.proc_id);
	        return 1 ;
	    }
   return 0 ;
}
	   

/*---------------------------------------------------------------------------
  Prototype   : int sendToProcess(int sd, process_t *p_p)
  Parameters  : sd   - socket descriptor of connection to process
                p_p  - pointer to process
  Returns     : 1 - process infn. successfully sent to process
                0 - error in communication with process
  Called by   : execReadyProcesses
  Calls       : htons, strcpy, writen, readn, ntohs, sendLink
  Notes       : The infn that has to reach the process are sent. This infn
                must be read by the process through Synergy library.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int sendToProcess(sd, p_p)
int sd; 
process_t *p_p;
{
   dac_data_ot out ;
   dac_data_it in ;
   link_t *p_l ;
   dac_getcpulist_ot out2;
   process_t *p_q;
   int i, j, k;
				/* prepare infn. to be sent to process */
   out.host = p_p->host;
   strcpy(out.name, p_p->name) ;
   strcpy(out.appid, app_id) ;
/*
printf(" DAC896 sendToProcess app_id(%s)\n",app_id);
*/
   out.protocol = htons(p_p->protocol) ;
   out.f = htons(p_p->f) ;
   out.p = htons(p_p->p) ;
   out.t = htons(p_p->t) ;
   out.d = htons(p_p->d) ;
   out.link_cnt = htons(p_p->link_cnt) ;
				/* send infn. to process */
   if (writen(sd, (char *)&out, sizeof(dac_data_ot)))
      for (p_l = p_p->link ; p_l != NULL ; p_l = p_l->next)
	 if (!sendLink(sd, p_l))
	 {
            printf(" DAC send link error \n");
	    return 0 ;
	 }
   /* Now add cpu_list to process. YS f96 */

   out2.hlist_cnt = htons(plist_cnt);
   p_q = process_list;
/*
printf(" Dac: plist_cnt (%d) \n", plist_cnt);
*/
   if (plist_cnt > 0) {
       i = j = k = 0;
      while (plist_cnt > BUFFE_SIZE * j) {
           j++;
           for ( ; i < BUFFE_SIZE * j && i < plist_cnt; i ++) {
                        k = i - BUFFE_SIZE * (j - 1);
			strcpy((out2.buffer[k]).app_id, app_id);
                        (out2.buffer[k]).hostid = (p_q->host);
                        (out2.buffer[k]).proc_id = (p_q->proc_id);
                        strcpy((out2.buffer[k]).login, p_q->login);
/*
printf(" Dac: appid(%s) hostid(%ul) pid(%d) login(%s)\n",
	(out2.buffer[k]).app_id,
	(out2.buffer[k]).hostid, 
	(out2.buffer[k]).proc_id,
	(out2.buffer[k]).login);
*/
                        p_q = p_q->next;
           }
           if (!writen(sd, (char *)&out2, sizeof(dac_getcpulist_ot))) {
                   printf("Send cpu list failure.\n");
                   break;
           }
      }
   } else {
       if (!writen(sd, (char *)&out2, sizeof(dac_getcpulist_ot)))
                printf("Send cpu list failure.\n");
   }
   /* Now do a final check to see if the process is still live */
   if (readn(sd, (char *)&in, sizeof(dac_data_it)))
      if (ntohs(in.status) == SUCCESS)
	 return 1 ;
	else printf("++ DAC: Communication error with (%s)\n",p_p->name);
   else printf("++ DAC: Unable to contact (%s). Check available resources. \n",
		p_p->name);
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int sendLink(int sd, link_t *p_l)
  Parameters  : sd   - socket descriptor of connection to process
                p_l  - pointer to link
  Returns     : 1 - link infn. successfully sent to process
                0 - error in communication with process
  Called by   : sendToProcess
  Calls       : strcpy, writen, readn, ntohs
  Notes       : Details about each link is sent to the process. Path, mode
                are specific to FILE and 'host' is applicable to input PIPE.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int sendLink(sd, p_l)
int sd; 
link_t *p_l;
{
   dac_link_ot out ;

   out.type = p_l->type ;	/* prepare link infn. to be sent to process */
   out.dirn = p_l->dirn ;
   out.host = p_l->host ;
   strcpy(out.ref_name, p_l->ref_name) ;
   strcpy(out.obj_name, p_l->obj_name) ;
   strcpy(out.path, p_l->path) ;
   strcpy(out.mode, p_l->mode) ;
   strcpy(out.login, p_l->login);
				/* send link infn. to process */
   if (writen(sd, (char *)&out, sizeof(dac_link_ot)))
      return 1 ;
   return 0 ;
}

/*---------------------------------------------------------------------------
  Prototype   : int modifyPrcd(char *name)
  Parameters  : name - pointer to name of the process
  Returns     : 0 - no new process has become READY
                1 - one or more processes have become READY
  Called by   : OpExitProcess
  Calls       : free, strcmp
  Notes       : 'name' is removed from the precedence list of those processes
                that depend on it to complete. If any new process is READY
		to execute as a result it's indicated as return value.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int modifyPrcd(name)
char *name;
{
   int ready_flag ;
   process_t *p_p ;
   prcd_t *p_pr, **pp_pr ;

   ready_flag = 0 ;		/* update all processes' precedent list */
   for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
    {
       pp_pr = &p_p->prcd ;
       p_pr = p_p->prcd ;
				/* scan the precedence list */
       while (p_pr != NULL)
	{			/* if match, remove process */
	   if (!strcmp(p_pr->process->name, name))
	    {
	       *pp_pr = p_pr->next ;
	       free(p_pr) ;
	       p_pr = *pp_pr ;
	    }
	   else
	    {			/* no match, skip to next process */
	       pp_pr = &p_pr->next ;
	       p_pr = p_pr->next ;
	    }
	}			/* update ready-flag, if process is READY */
       if ((p_p->prcd == NULL) && (p_p->state == WAITING))
	{
	   ready_flag = 1 ;
	   p_p->state = READY ;
	}
    }
   return ready_flag ;
}


/*---------------------------------------------------------------------------
  Prototype   : void killAllProcesses(void)
  Parameters  : -
  Returns     : -
  Called by   : OpExit, OpKillAll
  Calls       : killProcess
  Notes       : All the processes of this application are killed.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void killAllProcesses()
{
   process_t *p_p ;

   for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
    {
       if (p_p->state == RUNNING || p_p->state == STARTED)
	  killProcess(p_p) ;
       if (p_p->state != EXITTED)
	  p_p->state = KILLED ;
    }
}


/*---------------------------------------------------------------------------
  Prototype   : void killProcess(process_t *p_p)
  Parameters  : p_p - pointer to process
  Returns     : -
  Called by   : killAllProcesses, OpKill
  Calls       : strcpy, get_socket, do_connect, writen, readn, close, htons
  Notes       : Corresponding CID is contacted and the process is killed.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void killProcess(p_p)
process_t *p_p;
{
   cid_kill_it out ;
   cid_kill_ot in ;
   u_short op ;
   int sd ;

   op = htons(CID_OP_KILL) ;
   strcpy(out.appid, app_id) ;
   strcpy(out.name, p_p->name) ;
				/* connect to CID & kill */
   if ((sd = get_socket()) != -1)
   {
      sprintf(mapid,"sng$cid$%s",p_p->login);
      if (!(cidport=
        pmd_getmap(mapid,p_p->host,(u_short)PMD_PROT_TCP)))
      {
        printf("Remote host lookup failure for proc. kill. \n");
      } else {
      if (do_connect(sd, p_p->host, cidport))
	 if (writen(sd, (char *)&op, sizeof(u_short)))
	    if (writen(sd, (char *)&out, sizeof(cid_kill_it)))
	    {
	       readn(sd, (char *)&in, sizeof(cid_kill_ot)) ;
	       p_p->elapsed_time = time((long *)0) - p_p->elapsed_time;
	    }
      }
   }
   close(sd) ;
}

/*---------------------------------------------------------------------------
  Prototype   : void cleanAllProcesses(void)
  Parameters  : -
  Returns     : -
  Called by   : OpExit, OpKillAll
  Calls       : cleanProcess
  Notes       : All the process slots of this application are cleaned.
  Date        : Mar 04' 
  Coded by    : JYS (revised from killAllProcesses) 
  Modification:
---------------------------------------------------------------------------*/  

void cleanAllProcesses()
{
   process_t *p_p ;

   for (p_p = process_list ; p_p != NULL ; p_p = p_p->next)
    {
       if (p_p->state == RUNNING || p_p->state == STARTED)
	  cleanProcess(p_p) ;
       if (p_p->state != EXITTED)
	  p_p->state = KILLED ;
    }
}

/*---------------------------------------------------------------------------
  Prototype   : void cleanProcess(process_t *p_p)
  Parameters  : p_p - pointer to process
  Returns     : -
  Called by   : killAllProcesses, OpKill
  Calls       : strcpy, get_socket, do_connect, writen, readn, close, htons
  Notes       : Corresponding CID is contacted and the process is killed.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void cleanProcess(p_p)
process_t *p_p;
{
   cid_kill_it out ;
   cid_kill_ot in ;
   u_short op ;
   int sd ;

   op = htons(CID_OP_KILL_APP) ;
   strcpy(out.appid, app_id) ;
   strcpy(out.name, p_p->name) ;
				/* connect to CID & clean all slots */
   if ((sd = get_socket()) != -1)
   {
      sprintf(mapid,"sng$cid$%s",p_p->login);
      if (!(cidport=
        pmd_getmap(mapid,p_p->host,(u_short)PMD_PROT_TCP)))
      {
        printf("Remote host lookup failure for proc. kill. \n");
      } else {
      if (do_connect(sd, p_p->host, cidport))
	 if (writen(sd, (char *)&op, sizeof(u_short)))
	    if (writen(sd, (char *)&out, sizeof(cid_kill_it)))
	    {
	       readn(sd, (char *)&in, sizeof(cid_kill_ot)) ;
	       p_p->elapsed_time = time((long *)0) - p_p->elapsed_time;
	    }
      }
   }
   close(sd) ;
}

/*---------------------------------------------------------------------------
  Prototype   : int pingProcess(process_t *p_p)
  Parameters  : p_p - pointer to process
  Returns     : 1 - process exists [or] 
                0 - process doesnot exist
  Called by   : OpGetAppstate
  Calls       : strcpy, get_socket, do_connect, writen, readn, close, htons, 
                ntohs
  Notes       : Corresponding CID is contacted and checked for the process.
  Date        : July '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int pingProcess(p_p)
process_t *p_p;
{
   cid_ping_it out ;
   cid_ping_ot in ;
   u_short op ;
   int sd ;

   op = htons(CID_OP_PING) ;
   strcpy(out.appid, app_id) ;
   strcpy(out.name, p_p->name) ;
				/* connect to CID & ping */
   if ((sd = get_socket()) != -1)
   {
      sprintf(mapid,"sng$cid$%s",p_p->login);
      if (!(cidport=
        pmd_getmap(mapid,p_p->host,(u_short)PMD_PROT_TCP)))
      {
        printf("Remote host lookup failture for ping. \n");
        return 0;
      }
      if (do_connect(sd, p_p->host, cidport))
	 if (writen(sd, (char *)&op, sizeof(u_short)))
	    if (writen(sd, (char *)&out, sizeof(cid_ping_it)))
	       if (readn(sd, (char *)&in, sizeof(cid_ping_ot)))
		  if (ntohs(in.status) == SUCCESS)
		   {
		      close(sd) ;
		      return 1 ;
		   }
   }
   close(sd) ;
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : void send_cid_appstatus(u_char status)
  Parameters  : status - application status
  Returns     : -
  Called by   : initDac, OpStart, OpExitProcess, OpExit 
  Calls       : get_socket, printf, pmd_getmap, do_connect, htons,
		writen, close
  Notes       :
  Date        : July' 94
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void send_cid_appstatus(hostname, status)
char *hostname;
u_char status;
{
	cid_changeappmsg_it out;

	strcpy(out.app_id, dacmapid);
	out.app_status = status;
	if (status == APP_RUNNING) 
	out.elapsed_time = htonl(time((long *)0) - elapsed_time);
	else out.elapsed_time = elapsed_time;

	if (!changeappmsg(hostname, getpwuid(getuid())->pw_name, out))
		printf("Send app status (%u) failure.\n", status);
}


/*---------------------------------------------------------------------------
  Prototype   : void OpGetProclist(void)
  Parameters  : -
  Returns     : - 
  Called by   : startExec
  Calls       : writen
  Notes       :
  Date        : July' 94
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void OpGetProclist()
{
	dac_getproclist_ot out;
	process_t *p_q;
	int i, j, k;

	out.plist_cnt = htons(plist_cnt);
	p_q = process_list;
	if (plist_cnt > 0) {
            i = j = k = 0;
            while (plist_cnt > BUFFE_SIZE * j) {
                j++;
                for ( ; i < BUFFE_SIZE * j && i < plist_cnt; i ++) {
                        k = i - BUFFE_SIZE * (j - 1);
                        strcpy((out.buffer[k]).name, p_q->name);
                        strcpy((out.buffer[k]).login, p_q->login);
                        strcpy((out.buffer[k]).path, p_q->path);
                        (out.buffer[k]).host = p_q->host;
			(out.buffer[k]).state = p_q->state;
			if (p_q->state == RUNNING)
			sprintf((out.buffer[k]).elapsed_time, "%ld Sec.",
                                time((long *)0) -  p_q->elapsed_time);
			else if (p_q->elapsed_time > 900000)
			{
				p_q->elapsed_time = time((long *)0) -
					p_q->elapsed_time; 
				sprintf((out.buffer[k]).elapsed_time, "%ld Sec.",
				p_q->elapsed_time);
			}
			else sprintf((out.buffer[k]).elapsed_time,"%ld Sec.",
				p_q->elapsed_time);
                        p_q = p_q->next;
                }
                if (!writen(newsock, (char *)&out, sizeof(dac_getproclist_ot))) {
		 	printf("Send process list failure.\n");
                        break;
                }
            }
	} else
	    if (!writen(newsock, (char *)&out, sizeof(dac_getproclist_ot)))
		printf("Send process list failure.\n");
}


/*---------------------------------------------------------------------------
  Prototype   : void OpGethandlerlist(void)
  Parameters  : -
  Returns     : -
  Called by   : startExec
  Calls       : writen
  Notes       :
  Date        : October' 94
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void OpGetHandlerlist()
{
        dac_gethandlerlist_ot out;
        handler_t *p_q;
        int i, j, k;

        out.hlist_cnt = htons(hlist_cnt);
        p_q = handler_list;
        if (hlist_cnt > 0) {
            i = j = k = 0;
            while (hlist_cnt > BUFFE_SIZE * j) {
                j++;
                for ( ; i < BUFFE_SIZE * j && i < hlist_cnt; i ++) {
                        k = i - BUFFE_SIZE * (j - 1);
                        strcpy((out.buffer[k]).name, p_q->name);
                        strcpy((out.buffer[k]).login, p_q->login);
                        strcpy((out.buffer[k]).mapid, p_q->mapid);
                        strcpy((out.buffer[k]).inet_addr, p_q->inet_addr);
                        (out.buffer[k]).host = p_q->host;
                        (out.buffer[k]).port = p_q->port;
                        (out.buffer[k]).type = p_q->type;
                        p_q = p_q->next;
                }
                if (!writen(newsock, (char *)&out, sizeof(dac_gethandlerlist_ot))) {
                        printf("Send handler list failure.\n");
                        break;
                }
            }
        } else
            if (!writen(newsock, (char *)&out, sizeof(dac_gethandlerlist_ot)))
                printf("Send handler list failure.\n");
}



void push_host(hostid, procid, login)
u_long hostid;
int procid;
char *login;
{
   cpu_t *t1;

   if (cpu_header == NULL)
   {
	t1 = (cpu_t *)malloc(sizeof(cpu_t));
	t1->hostid = hostid;
        t1->proc_id = procid;
        strcpy(t1->login, login);
	t1->next = cpu_header;
	cpu_header = t1;
	total_cpu = 1;
	return;
   }
   t1 = cpu_header;
   while (t1 != NULL)
   {
	if (hostid == t1->hostid) return;
	t1 = t1->next;
   }
   t1 = (cpu_t *) malloc(sizeof(cpu_t));
   t1->hostid = hostid;
   t1->proc_id = procid;
   strcpy(t1->login, login);
   t1->next = cpu_header;
   cpu_header = t1;
   total_cpu ++;
   return;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpGetCpulist(void)
  Parameters  : -
  Returns     : -
  Called by   : startExec
  Calls       : writen
  Notes       :
  Date        :
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void OpGetCpulist()
{
        dac_getcpulist_ot out;
        process_t *p_q;
        int i, j, k;

        out.hlist_cnt = plist_cnt;
        p_q = process_list;
        if (plist_cnt > 0) {
            i = j = k = 0;
            while (plist_cnt > BUFFE_SIZE * j) {
                j++;
                for ( ; i < BUFFE_SIZE * j && i < plist_cnt; i ++) {
                        k = i - BUFFE_SIZE * (j - 1);
			strcpy((out.buffer[k]).app_id, app_id);
                        (out.buffer[k]).hostid = p_q->host;
                        (out.buffer[k]).proc_id = htonl(p_q->proc_id);
                        strcpy((out.buffer[k]).login, p_q->login);
                        p_q = p_q->next;
                }
                if (!writen(newsock, (char *)&out, sizeof(dac_getcpulist_ot))) {
                        printf("Send cpu list failure.\n");
                        break;
                }
            }
        } else {
            if (!writen(newsock, (char *)&out, sizeof(dac_getcpulist_ot)))
                printf("Send cpu list failure.\n");
	}
}


/*---------------------------------------------------------------------------
  Prototype   : void OpFddChangeProc()
  Parameters  : -
  Returns     : -
  Called by   :
  Calls       :
  Notes       :
  Date        : October '94
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void OpFddChangeProc()
{
        dac_fddchangeproc_it in;
        process_t *p_p;

        if (!readn(newsock, (char *)&in, sizeof(dac_fddchangeproc_it)))
                return;
	in.proc_id = ntohl(in.proc_id);

        p_p = process_list;
        while (p_p != NULL) {
                if (p_p->host == in.hostid && p_p->proc_id == in.proc_id)
                        if (p_p->state != EXITTED && p_p->state != FAILED)
                                p_p->state = KILLED;
                p_p = p_p->next;
        }
}
