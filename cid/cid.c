/*.........................................................................*/
/*                     CID.C ------> Command Interpretter Daemon           */
/*.........................................................................*/

#include "cid.h"

int daemon(int, int);
void closeall(int);
int status;

/*---------------------------------------------------------------------------
  Prototype   : int main(void)
  Parameters  : -
  Returns     : Never returns
  Called by   : System
  Calls       : initFromfile, initSocket, initOther, start
  Notes       : The initialization routines and the main CID function are
                invoked.
  date        : April '93
	        February '13, updated by Justin Y. Shi
  Coded by    : N. Isaac Rajkumar
  Modification:
        1) CIDPORT is changed from static to dynamic. It is registered
                with PMD using "hostname+accountname". Yuan Shi
        2) PMD is merged with CID. The new daemons is named "cid".
                Yuan Shi
        3) CIDport registration "sng$cid$login".
        4) Automatic start pmd if it was down.
        5) ReportD is merged with CID. YS94
        6) Local TSH is built in CID. FEIJIAN SUN, 04/96
---------------------------------------------------------------------------*/

int main(argc,argv)
int argc;
char **argv;
{
   if (argc > 1) multiplier = atoi(argv[1]);
   else multiplier = 1;

   if (multiplier > 1) status=daemon(1, 0); // Only dive when %cid 2
   if (getenv("SNG_PATH") == NULL)
   {
	printf("SNG_PATH not set. Exiting...\n");
	exit(0);
   }
   sprintf(CID_PATH,"%s/bin",getenv("SNG_PATH")); 
   sprintf(PMD_PATH,"%s/pmd &",CID_PATH);
   sprintf(tsh_path,"%s/tsh",CID_PATH);
   sprintf(fah_path,"%s/fah",CID_PATH);

/* Path of Bcast Reciever. FSUN 04/96 
   sprintf(bcr_path,"%s/bcr",CID_PATH);
*/
/* If active_cid=1, sigHandler start BCR. FSUN 07/96 
   active_cid = 1 ;
*/

/* initFromfile() ;                 initialise from init file */
   initOther() ;                /* initialise the data structures */
   initSocket() ;               /* create socket, bind */
/*
   startBCR() ;                  Start BCast receiver. FSUN 04/96
*/ 
   start() ;                    /* accept connections/requests */
}


/*---------------------------------------------------------------------------
  Prototype   : void initFromfile(void)
  Parameters  : -
  Returns     : -
  Called by   : main
  Calls       : fopen, fprintf, fgets, strchr, exit, fclose
  Notes       : The initialization file contains the path for TSH and FAH.
                Line 1 contains TSH path and line 2 FAH path.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void initFromfile()
{
   FILE *fp ;
   char *tmp, init_file_name[PATH_LEN+LOGIN_LEN] ;
                                /* open CID inititialization file */
   sprintf(init_file_name,"%s/%s",CID_PATH,INIT_FILE);
   if ((fp = fopen(init_file_name, "r")) == NULL)
    {
       fprintf(stderr, "initFromfile::fopen\n") ;
       exit(1) ;
    }
                                /* read path for TSH */
   if (fgets(tsh_path, PATH_LEN, fp) == NULL)
    {
       fprintf(stderr, "initFromfile::fgets\n") ;
       exit(1) ;
    }
   if ((tmp = strchr(tsh_path, '\n')) != NULL)
      *tmp = '\0' ;
                                /* read path for FAH */
   if (fgets(fah_path, PATH_LEN, fp) == NULL)
    {
       fprintf(stderr, "initFromfile::fgets\n") ;
       exit(1) ;
    }
   if ((tmp = strchr(fah_path, '\n')) != NULL)
      *tmp = '\0' ;
   fclose(fp) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void initOther(void)
  Parameters  : -
  Returns     : -
  Called by   : main
  Calls       : initIpc, signal, fprintf, time, exit
  Notes       : Memory is allocated for CID table and handler for SIGTERM,
                SIGCHLD is installed. When CID has to be killed, it can be
                sent SIGTERM.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void initOther()
{
   int i ;
                                /* install signal handlers */
   signal(SIGTERM, sigtermHandler);
   signal(SIGINT, sigtermHandler);
   signal(SIGALRM, sigAlarmHandler);
   signal(SIGCHLD, sigchldHandler);
                                /* get start time */
   time(&start_time) ;
                                /* initialize application list */
   list_cnt = 0;
   list_p = end_p = NULL;
                                /* initialize CID table*/
   for (i = 0 ; i < TABLE_SIZE ; i++) {
      table[i].used = UNUSED ;  /* table is empty */
      table[i].term_stat = 1 ;  /* FSUN 10/94. For FDD */
   }
   if ((pmd_table =
        (pmd_table_t *)malloc(sizeof(pmd_table_t) * TABLE_SIZE)) == NULL)
    {
      fprintf(stderr, "initOther::malloc PMD_table\n") ;
      exit(1) ;
    }
   for (i = 0 ; i < TABLE_SIZE ; i++)
     pmd_table[i].used = UNUSED;

}


/*---------------------------------------------------------------------------
  Prototype   : void initSocket(void)
  Parameters  : -
  Returns     : -
  Called by   : main
  Calls       : get_socket, bind_socket, fprintf, exit
  Notes       : A well known port (CID_PORT) is bound to a socket, to accept
                connections. 'oldsock' is the global variable in which CID
                accepts connections.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void initSocket()
{                       /* bind socket to well known port*/
   sng_int16 cidport;
   int pt,killoldcid_pid, i;
   struct hostent *cidhost;
   FILE *fd;
   char filenm[128],ip[32],ipn[128],prt[32],os[32],login[32],fsys[128];
   int r_val=0;

   if ((oldsock = get_socket()) == -1)
    {
       fprintf(stderr, "initSocket::get_socket\n") ;
       exit(1) ;
    }
   if (!(cidport = bind_socket(oldsock, 0)))
    {
       fprintf(stderr, "initSocket::bind_socket\n") ;
       close(oldsock);
       exit(1) ;
    }

/* Get a global variable Tcidport for local TSH. FSUN 04/96 */
    Tcidport = cidport;

    if (gethostname(mapid,sizeof(mapid)) == -1)
    {
        printf(" Cannot get hostname?? \n");
        exit(1);
    }
 printf(" CID HOST NAME (%s)\n", mapid);
    /* Find the assigned IP address */

    sprintf(filenm,"%s/.sng_hosts",getenv("HOME"));
    if ((fd = fopen(filenm, "r")) != NULL) 
    {
      r_val = fscanf(fd, "%s %s %s %s %s %s\n",ip,ipn,prt,os,login,fsys);
      while (r_val > 0)
      {
	if (ip[0] != '#') {
		if (strcmp(mapid, ipn) == 0)
		{
			strcpy(mapid, ip);
			break;
		}
	}
        r_val = fscanf(fd, "%s %s %s %s %s %s\n",ip, ipn, prt, os, login, fsys);
      }
    } else {
	printf("Please creat a ~/.sng_hosts file first.\n");
	exit(-1);
    }
    fclose(fd);
   
printf(" Actual CID IP(%s)\n", mapid); 

    if ((cidhost = gethostbyname(mapid)) == NULL)
    {
        printf("cannot get local host?? gethostbyname\n") ;
        exit(1) ;
    }
    hostid = *((long *)cidhost->h_addr_list[0]) ;
    printf("\n");
    sprintf(mapid,"sng$cid$%s",getpwuid(getuid())->pw_name);
    if ((pt = pmd_getmap(mapid, hostid, (u_short)PMD_PROT_TCP))>0)
    {
        /* use the old port to kill the existing CID */
        kill_old_cid(pt);
        printf("Reusing cid entry. \n");
    }
    if (!(pmd_map(mapid, cidport, (u_short)PMD_PROT_TCP)))
    {
        printf("Unable to contact peer. Trying again ... \n");
        for (i=0; i < 3; i++)
        {
                if (pmd_map(mapid, cidport, (u_short)PMD_PROT_TCP))
                {
                        printf("CID ready.\n");
                        return;
                }
                printf("Re-trying (pass %d) ... \n",i);
                sleep(5);
        }
        status=system(PMD_PATH);
        printf("New peer leader ready.\n");
        sleep(3);
        if (!(pmd_map(mapid, cidport, (u_short)PMD_PROT_TCP)))
        {
                printf("New peer leader not usable. Net congestion suspected. Try later.\n");
                exit(0);
        }
    }
    printf("CID ready.\n");
}


/*---------------------------------------------------------------------------
  Prototype   : void startBCR(void)
  Parameters  : -
  Returns     : -
  Called by   : main
  Calls       :
  Notes       : Start BCast receiver, a independent monitoring process
                that is responsible for recieving the BCast info.
                sent by other CIDs.
  Date        : April '96
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void startBCR()
{
   int slot ;
   char *path ;
   char l_hostid[32], l_port[32];

   path = bcr_path ; 
   if (access(path, X_OK) == -1)
   {
     fprintf(stderr, "startBCR::access\n") ;
     exit(1) ;
   }

   sprintf(bcid, "sng$bc$%s", getpwuid(getuid())->pw_name);

   if ((slot = getSlot()) == -1)
   {
     fprintf(stderr, "startBCR::getSlot\n") ;
     exit(1) ;
   }
                                /* fill entry in CID table for this process */
   table[slot].appid[0] = 0; 
   table[slot].name[0] = 0; 
   table[slot].fdd_start = 0;
   table[slot].type = CID_TYPE_BCR ;
   table[slot].host = 0 ;
   table[slot].port = 0 ;
   table[slot].used = USED;

   if ((bcr_pid  = fork()) == 0 )
   {
     sprintf(l_hostid, "%lu", hostid); 
     sprintf(l_port, "%d", Tcidport); 
     execl(path, path, bcid, l_hostid, l_port, (char *)0) ;
                                /* exec failed, should never happen */
     ungetSlot(slot) ;
     exit(-1);
   }
   table[slot].pid = bcr_pid;   /* Get a global Bcast Reciever Process ID variable */

   signal(SIGCHLD, sigchldHandler);
}


/*---------------------------------------------------------------------------
  Prototype   : void start(void)
  Parameters  : -
  Returns     : -
  Called by   : main
  Calls       : get_connection, fprintf, exit, readn, close, ntohs, logOps,
                fork, toFork, appropriate Op-function
  Notes       : This is the controlling function of CID that invokes
                appropriate routine based on the operation requested.
                For operations that have to be executed in the context of a
                user, a new process is forked and the uid of the child is
                altered.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification: Added OpVerifyUser, OpVerifyHandler, OpVerifyProcess.
                Changed to private cid - one per user. Yuan Shi

                Feijian Sun  04/10/96 --
                Created LOCAL TSH
---------------------------------------------------------------------------*/

void start()
{
   static void (*op_func[])() = {
      OpNull, OpExecTsh, OpExecTsh, OpExecComponent, OpKill, OpKillApp,
      OpKillAll, OpGetApptable, OpGetFulltable, OpPing, OpPingApp,
      OpRemove, OpFilePut, OpFileGet, OpAddUser, OpDeleteUser, OpVerifyUser,
      OpVerifyProcess, OpVerifyHandler, OpExit, OpExitProcess, OpResourceCk,
      OpSaveAppMsg, OpChangeAppMsg, OpGetAppList, OpGetApp, OpDelAppList,
      OpDelApp, OpDetectProc, OpVTUpd
      } ;
  static void (*pmdop_func[])() = {
      pmd_OpNull, OpMap, OpUnmap, OpGetMap, OpGetTable, OpPurgeTable, OpExit
      } ;     /* OpExit is never used in CID */
  static void (*tshop_func[])() = {
      OpPut, OpGet, OpGet, OpExit, OpRetrieve, OpTid, OpZap
      } ;     /* OpExit,OpRetrieve&OpTid are never used in CID */
  int pid;

   tsh.space = NULL; 
   while (TRUE)
    {                           /* get a connection for request */
       if ((newsock = get_connection(oldsock, &peer)) == -1)
        {
           fprintf(stderr, "start::get_connection\n") ;
           pmd_unmap(mapid,getpid(),(u_short)PMD_PROT_TCP);

/*
           kill(bcr_pid, SIGTERM) ;     Kill Bcast receiver process. FSUN 04/96 
*/
           exit(1) ;
        }
                                /* read operation code */
       if (!readn(newsock, (char *)&this_op, sizeof(u_short)))
        {
           close(newsock) ;
           continue ;
        }

       this_op = ntohs(this_op) ;
                               /* validate operation & process */
       if (this_op >= TSH_OP_MIN && this_op <= TSH_OP_MAX)
          (*tshop_func[this_op - TSH_OP_MIN])() ;
       else if (this_op >= CID_OP_MIN && this_op <= CID_OP_MAX)
          (*op_func[this_op - CID_OP_MIN])();
       else if (this_op >= PMD_OP_MIN && this_op <= PMD_OP_MAX)
          (*pmdop_func[this_op - PMD_OP_MIN])() ;
       else this_op = CID_OP_UNKNOWN ;

       logOps(peer) ;
       pmd_logOps() ;
       close(newsock) ;
    }
}


/*---------------------------------------------------------------------------
  Prototype   : void OpNull(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : writen, htons, htonl
  Notes       : This operation is available to test whether the CID is up.
                The # of entries in its table and also the time from when it
                is up are returned.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpNull()
{
   cid_null_ot out ;
   u_short temp;
                                /* return CID up-time, # entries */
   out.start_time = htonl(start_time) ;
   /* Get benchmark and send out */
   temp = bench();
   out.status = htons(temp);  out.error = htons(CID_ER_NOERROR) ;
   writen(newsock, (char *)&out, sizeof(cid_null_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpExecTsh(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, doLogin, getSlot, ungetSlot, access, strcpy,
                getpid, htons, execl, sprintf
  Notes       : This function is invoked for both CID_OP_EXEC_FAH and
                CID_OP_EXEC_TSH. Based on this_op it executes the right
                executable. Prior to that permissions for the user are
                checked i.e.
                - login must be a synergy user
                - rlogin, peer must be specified in the permissions file in
                  login's home directory in the format rlogin@host (host
                  can be either in . format or otherwise).
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification: table[].port = 0 so that exit-status will not be sent to DAC.
---------------------------------------------------------------------------*/

void OpExecTsh()
{
   cid_exectsh_it in ;
   cid_exectsh_ot out ;
   char bufsock[5] ;
   char *path , mapid[NAME_LEN];
   int slot ;
                                /* read TSH/FAH parameters */
   if (!readn(newsock, (char *)&in, sizeof(cid_exectsh_it)))
      return ;
                                /* check valid user, change uid of process */
   if (!doLogin(in.login, in.rlogin))
    {
       out.status = htons(FAILURE) ;  out.error = htons(CID_ER_NOLOGIN) ;
       writen(newsock, (char *)&out, sizeof(cid_exectsh_ot)) ;
       return ;
    }
                                /* check if could execute TSH/FAH */
   path = (this_op == CID_OP_EXEC_TSH) ? tsh_path : fah_path ;
   if (access(path, X_OK) == -1)
    {
       out.status = htons(FAILURE) ;  out.error = htons(CID_ER_NOACCESS) ;
       writen(newsock, (char *)&out, sizeof(cid_exectsh_ot)) ;
       return ;
    }
                                /* check if space available in CID table */
   if ((slot = getSlot()) == -1)
    {
       out.status = htons(FAILURE) ;  out.error = htons(CID_ER_NOSPACE) ;
       writen(newsock, (char *)&out, sizeof(cid_exectsh_ot)) ;
       return ;
    }
                                /* fill entry in CID table for this process */
   strcpy(table[slot].appid, in.appid) ;
   strcpy(table[slot].name, in.name) ;
   table[slot].fdd_start = ntohs(in.fdd_start);
   in.r_debug = ntohs(in.r_debug);
   if (in.r_debug) strcpy(mapid,in.appid);
   table[slot].type=(this_op == CID_OP_EXEC_TSH) ? CID_TYPE_TSH : CID_TYPE_FAH;
   table[slot].host = peer ;
   table[slot].port = 0 ;
                                /* execute TSH/FAH, pass socket desc. */
   if (in.r_debug) /* use mapid to start tsh YS94 */
   {
        if ((table[slot].pid = fork()) == 0 )
        {
        sprintf(bufsock, "%d", newsock) ;
        execl(path, path, "-a", mapid, in.name , bufsock, (char *)0) ;
                                /* exec failed, should never happen */
        ungetSlot(slot) ;
        exit(-1);
/*      out.status = htons(FAILURE) ;  out.error = htons(CID_ER_NOEXEC) ;
        writen(newsock, (char *)&out, sizeof(cid_exectsh_ot)) ;
*/
        }
   }
   else {
        printf("CID starting object (%s) \n",in.name);
        if ((table[slot].pid = fork()) == 0 )
        {
            sprintf(bufsock, "%d", newsock) ;
            execl(path, path, "-s", bufsock, (char *)0) ;
                                /* exec failed, should never happen */
            ungetSlot(slot) ;
            exit(-1);
/*          out.status = htons(FAILURE) ;  out.error = htons(CID_ER_NOEXEC) ;
            writen(newsock, (char *)&out, sizeof(cid_exectsh_ot)) ;
*/
        }
   }
                                /* return SUCCESS so that DAC can continue */
   out.proc_id = htonl(table[slot].pid);/* FSUN 10/94. For FDD to retrieve tuple. */
   signal(SIGCHLD, sigchldHandler);
   out.status = htons(SUCCESS) ;
   out.error = htons(CID_ER_NOERROR) ;
   if (!writen(newsock, (char *)&out, sizeof(cid_exectsh_ot)))
    {
       ungetSlot(slot);
       return ;
    }
}


/*---------------------------------------------------------------------------
  Prototype   : void OpExecComponent(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, doLogin, getSlot, ungetSlot, access, strcpy,
                getpid, htons, execv, sprintf
  Notes       : Prior to executing the component permissions for the user
                are checked i.e.
                - login must be a synergy user
                - rlogin, peer must be specified in the permissions file in
                  login's home directory in the format rlogin@host (host
                  can be either in . format or otherwise).
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification: Connection to DAC is converted from newsock to STDIN.
---------------------------------------------------------------------------*/

void OpExecComponent()
{
   cid_execcomponent_it in ;
   cid_execcomponent_ot out ;
   char *argv[ARGS_MAX+1] ;
   char buf[NAME_LEN];
   int i, slot ;
                                /* read component parameters */
   if (!readn(newsock, (char *)&in, sizeof(cid_execcomponent_it)))
      return ;
                                /* check valid user, change uid of process */
   if (!doLogin(in.login, in.rlogin))
    {
       out.status = htons(FAILURE) ;  out.error = htons(CID_ER_NOLOGIN) ;
       writen(newsock, (char *)&out, sizeof(cid_execcomponent_ot)) ;
       return ;
    }
                                /* check if could execute component */
   if (in.path[0] == '*')
   {
        for (i=0; i<strlen(in.path)-2; i++)
                in.path[i] = in.path[i+2];
        in.path[i] = '\0';
        in.path[i] = '\0';
        sprintf(buf, "bin/%s",in.path);
        strcpy(in.path,buf);
   }
   printf("CID starting program. path (%s)\n",in.path);
   if (access(in.path, X_OK) == -1)
   {
       out.status = htons(FAILURE) ;  out.error = htons(CID_ER_NOACCESS) ;
       writen(newsock, (char *)&out, sizeof(cid_execcomponent_ot)) ;
       return ;
   }
                                /* check if space available in CID table */
   if ((slot = getSlot()) == -1)
   {
       out.status = htons(FAILURE) ;  out.error = htons(CID_ER_NOSPACE) ;
       writen(newsock, (char *)&out, sizeof(cid_execcomponent_ot)) ;
       return ;
   }
                                /* fill up CID table for this process */
   strcpy(table[slot].appid, in.appid) ;
   strcpy(table[slot].name, in.name) ;
   table[slot].fdd_start = ntohs(in.fdd_start); /* FSUN 10/94. For FDD */
   table[slot].type = CID_TYPE_USR ;
   table[slot].host = peer ;
   table[slot].port = in.port ;
                                /* execute application component */
   if ((table[slot].pid = fork()) == 0)
   {
        argv[0] = in.path ;
        in.arg_cnt = ntohs(in.arg_cnt) ;
        for (i = 0 ; i < in.arg_cnt ; i++)
                argv[i+1] = in.args[i] ;
        argv[i+1] = (char *)0 ; /* make socket as stdin for component */

        if (newsock != STDIN)
        {
                dup2(newsock, STDIN) ;
                close(newsock) ;
        }

        execv(in.path, argv) ;
        ungetSlot(slot) ;               /* exec failed, should never happen */
        exit(-1);
   }
                                /* return SUCCESS so that DAC can continue */
   signal(SIGCHLD, sigchldHandler);
   out.proc_id = htonl(table[slot].pid);/* FSUN 10/94. FDD to retrieve tuple. */
   out.status = htons(SUCCESS) ;
   out.error = htons(CID_ER_NOERROR) ;
   if (!writen(newsock, (char *)&out, sizeof(cid_execcomponent_ot)))
    {
       ungetSlot(slot) ;
       return ;
    }
   writen(STDIN, (char *)&out, sizeof(cid_execcomponent_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpKill(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, strcmp, kill, htons
  Notes       : The component corresponding to application id and component
                name specified is sent SIGTERM. sigchldHandler, returns the
                exit status of the killed process.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpKill()
{
   cid_kill_it in ;
   cid_kill_ot out ;
   int i ;
                                /* read application id, component name */
   if (!readn(newsock, (char *)&in, sizeof(cid_kill_it)))
      return ;
                                /* search table for component */
   for (i = 0 ; i < TABLE_SIZE ; i++)
      if ((table[i].used == USED) && (!strcmp(in.appid, table[i].appid))
          && (!strcmp(in.name, table[i].name)))
       {                        /* exit-status sent by SIGCHLD handler */
          kill(table[i].pid, SIGTERM) ;
          ungetSlot(i) ;
          out.status = htons(SUCCESS) ;  out.error = htons(CID_ER_NOERROR) ;
          writen(newsock, (char *)&out, sizeof(cid_kill_ot)) ;
          return ;
       }
                                /* no entry in table, return error */
   out.status = htons(FAILURE) ;  out.error = htons(CID_ER_NOENTRY) ;
   writen(newsock, (char *)&out, sizeof(cid_kill_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpKillApp(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, strcmp, kill, htons
  Notes       : All components corresponding to application id are sent
                SIGTERM. sigchldHandler, returns the exit status of the
                killed processes.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification: While killing a app., remove the Tuple and Request Queues
		corrresponding to it in TS. FSUN 09/96
---------------------------------------------------------------------------*/

void OpKillApp()
{
   cid_killapp_it in ;
   cid_killapp_ot out ;
   int i ;
                                /* read application id. */
   if (!readn(newsock, (char *)&in, sizeof(cid_killapp_it)))
      return ;
                                /* kill processes of this application */
  for (i = 0, out.count = 0 ; i < TABLE_SIZE ; i++)
     if ((table[i].used == USED) && (!strcmp(in.appid, table[i].appid)))
      {                         /* exit-status sent by SIGCHLD handler */
         kill(table[i].pid, SIGTERM) ;
         ungetSlot(i) ;
         out.count++ ;
      }

  if (out.count > 0)
    {                           /* application present */
      out.status = htons(SUCCESS) ;
      out.error = htons(CID_ER_NOERROR) ;
    }
  else
    {                           /* application not present */
      out.status = htons(FAILURE) ;
      out.error = htons(CID_ER_NOENTRY) ;
    }
  out.count = htons(out.count) ;
  writen(newsock, (char *)&out, sizeof(cid_killapp_ot)) ;
  /* add app_list element removal */
  printf(" CID killed application. id (%s)\n",in.appid);
  DelTRQue(in.appid);
  delete_app(in.appid);
}


/*---------------------------------------------------------------------------
  Prototype   : void OpKillAll(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, kill, htons
  Notes       : All components are sent SIGTERM. SigchHandler, returns the
                exit status of the killed process.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpKillAll()
{
   cid_killall_ot out ;
   int i ;
                                /* kill all processes in table */
   for (i = 0, out.count = 0 ; i < TABLE_SIZE ; i++)
      if (table[i].used == USED)
       {                        /* exit-status sent by SIGCHLD handler */
          kill(table[i].pid, SIGTERM) ;
          ungetSlot(i) ;
          out.count++ ;
       }
                                /* components present */
  if (out.count > 0)
   {
      out.status = htons(SUCCESS) ;
      out.error = htons(CID_ER_NOERROR) ;
   }
  else                          /* no components present */
   {
      out.status = htons(FAILURE) ;
      out.error = htons(CID_ER_NOENTRY) ;
   }
   out.count = htons(out.count) ;
   writen(newsock, (char *)&out, sizeof(cid_killall_ot)) ;
   /* Clear app_list pointer to null */
}


/*---------------------------------------------------------------------------
  Prototype   : void OpGetApptable(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, htons, ntohs, getAppcount, sendApptable
  Notes       : All entries in CID table corresponding to the application
                id specified are sent.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpGetApptable()
{
   cid_getapptable_ot1 out ;
   cid_getapptable_it in ;
   int i ;
                                /* read appid for operation */
   if (!readn(newsock, (char *)&in, sizeof(cid_getapptable_it)))
      return ;

   if ((out.count = getAppcount(in.appid)) > 0)
    {                           /* processes present for application */
       out.status = htons(SUCCESS) ;
       out.error = htons(CID_ER_NOERROR) ;
    }
   else
    {                           /* processes not present for application */
       out.status = htons(FAILURE) ;
       out.error = htons(CID_ER_NOENTRY) ;
    }
                                /* send count for allocating resources */
   out.count = htons(out.count) ;
   if (!writen(newsock, (char *)&out, sizeof(cid_getapptable_ot1 )))
      return ;
   if (ntohs(out.count) > 0)
      sendApptable(in.appid) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpGetFulltable(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, htons, ntohs, sendFulltable
  Notes       : All entries in the CID table are sent.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpGetFulltable()
{
   cid_getfulltable_ot1 out ;
   int i ;
                                /* count # of entries in table */
   for (i = 0, out.count = 0 ; i < TABLE_SIZE ; i++)
      if (table[i].used == USED)
         out.count++ ;

   if (out.count > 0)
    {                           /* processes present */
       out.status = htons(SUCCESS) ;
       out.error = htons(CID_ER_NOERROR) ;
    }
   else
    {                           /* processes not present */
       out.status = htons(FAILURE) ;
       out.error = htons(CID_ER_NOENTRY) ;
    }
                                /* send count for allocating resources */
   out.count = htons(out.count) ;
   if (!writen(newsock, (char *)&out, sizeof(cid_getfulltable_ot1)))
      return ;
   if (ntohs(out.count) > 0)
      sendFulltable() ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpPing(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, htons, strcmp
  Notes       : Checks whether a component with specified application id
                and component name exists in the system.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpPing()
{
   cid_ping_it in ;
   cid_ping_ot out ;
   int i ;
                                /* read application id, component name */
   if (!readn(newsock, (char *)&in, sizeof(cid_ping_it)))
      return ;
                                /* search table for component */
   for (i = 0 ; i < TABLE_SIZE ; i++)
    {
       if ((table[i].used == USED) && (!strcmp(in.appid, table[i].appid))
           && (!strcmp(in.name, table[i].name)))
        {
           out.status = htons(SUCCESS) ;  out.error = htons(CID_ER_NOERROR) ;
           writen(newsock, (char *)&out, sizeof(cid_ping_ot)) ;
           return ;
        }
    }
                                /* no entry in table, return error */
   out.status = htons(FAILURE) ;  out.error = htons(CID_ER_NOENTRY) ;
   writen(newsock, (char *)&out, sizeof(cid_ping_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpPingApp(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, htons, strcmp
  Notes       : Checks whether a component with specified application id
                exists in the system.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpPingApp()
{
   cid_pingapp_it in ;
   cid_pingapp_ot out ;
   int i ;
                                /* read application id. */
   if (!readn(newsock, (char *)&in, sizeof(cid_pingapp_it)))
      return ;
                                /* search table for application */
  for (i = 0, out.count = 0 ; i < TABLE_SIZE ; i++)
     if ((table[i].used == USED) && (!strcmp(in.appid, table[i].appid)))
        out.count++ ;

  if (out.count > 0)
    {                           /* application present */
      out.status = htons(SUCCESS) ;
      out.error = htons(CID_ER_NOERROR) ;
    }
  else
    {                           /* application not present */
      out.status = htons(FAILURE) ;
      out.error = htons(CID_ER_NOENTRY) ;
    }
  out.count = htons(out.count) ;
  writen(newsock, (char *)&out, sizeof(cid_pingapp_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpRemove(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, doLogin, unlink, htons
  Notes       : The specified file is deleted. Prior to that permissions
                are verified.
                - login must be a synergy user
                - rlogin, peer must be specified in the permissions file in
                  login's home directory in the format rlogin@host (host
                  can be either in . format or otherwise).
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpRemove()
{
   cid_remove_it in ;
   cid_remove_ot out ;

   if (!readn(newsock, (char *)&in, sizeof(cid_remove_it)))
      return ;
                                /* check valid user, change uid of process */
   if (!doLogin(in.login, in.rlogin))
    {
       out.status = htons(FAILURE) ;
       out.error = htons(CID_ER_NOLOGIN) ;
    }
   else                         /* remove file */
      if (unlink(in.path) == -1)
       {
          out.status = htons(FAILURE) ;
          out.error = htons(CID_ER_NOREMOVE) ;
       }
      else
       {
          out.status = htons(SUCCESS) ;
          out.error = htons(CID_ER_NOERROR) ;
       }
   writen(newsock, (char *)&out, sizeof(cid_remove_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpFilePut(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, doLogin, receiveFile, unlink, htons
  Notes       : The specified file is created and data received from socket
                is stored. Prior to that permissions are verified.
                - login must be a synergy user
                - rlogin, peer must be specified in the permissions file in
                  login's home directory in the format rlogin@host (host
                  can be either in . format or otherwise).
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification: Modified OpTransfer to OpFilePut.
---------------------------------------------------------------------------*/

void OpFilePut()
{
   cid_fileput_it in ;
   cid_fileput_ot out ;
   FILE *fp ;

   if (!readn(newsock, (char *)&in, sizeof(cid_fileput_it)))
      return ;
                                /* check valid user, change uid of process */
   if (!doLogin(in.login, in.rlogin))
    {
       out.status = htons(FAILURE) ;
       out.error = htons(CID_ER_NOLOGIN) ;
    }
   else                         /* open file and store received data */
      if ((fp = fopen(in.path, "w")) == NULL)
       {
          out.status = htons(FAILURE) ;
          out.error = htons(CID_ER_NOTRANSFER) ;
       }
      else                      /* if error in receiving, remove file */
         if (!receiveFile(fp, ntohl(in.len)))
          {
             out.status = htons(FAILURE) ;
             out.error = htons(CID_ER_NOTRANSFER) ;
             unlink(in.path) ;
          }
         else
          {
             out.status = htons(SUCCESS) ;
             out.error = htons(CID_ER_NOERROR) ;
          }
   fclose(fp) ;
   writen(newsock, (char *)&out, sizeof(cid_fileput_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpFileGet(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, doLogin, sendFile, unlink, htons
  Notes       : The specified file is read and sent through the socket.
                Prior to that permissions are verified.
                - login must be a synergy user
                - rlogin, peer must be specified in the permissions file in
                  login's home directory in the format rlogin@host (host
                  can be either in . format or otherwise).
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification: Modified OpTransfer to OpFilePut.
---------------------------------------------------------------------------*/

void OpFileGet()
{
   cid_fileget_it in ;
   cid_fileget_ot out ;
   FILE *fp ;
   struct stat buf ;

   if (!readn(newsock, (char *)&in, sizeof(cid_fileget_it)))
      return ;
                                /* check valid user, change uid of process */
   if (!doLogin(in.login, in.rlogin))
    {
       out.status = htons(FAILURE) ;
       out.error = htons(CID_ER_NOLOGIN) ;
       writen(newsock, (char *)&out, sizeof(cid_fileget_ot)) ;
       return ;
    }
   else                         /* open file and store received data */
      if ((fp = fopen(in.path, "r")) == NULL)
       {
          out.status = htons(FAILURE) ;
          out.error = htons(CID_ER_NOTRANSFER) ;
          writen(newsock, (char *)&out, sizeof(cid_fileget_ot)) ;
          return ;
       }
   out.status = htons(SUCCESS) ;
   out.error = htons(CID_ER_NOERROR) ;
   stat(in.path, &buf) ;
   out.len = htonl(buf.st_size) ;
   writen(newsock, (char *)&out, sizeof(cid_fileget_ot)) ;
   sendFile(fp, buf.st_size) ;
   fclose(fp) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpAddUser(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, htons, validSynergyuser, addUser
  Notes       : This request is made when a user of this m/c must be made a
                synergy user. Manually a request is sent to synergy admin,
                from whom this request comes to update the list in the m/c.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpAddUser()
{
   cid_adduser_it in ;
   cid_adduser_ot out ;

   if (!readn(newsock, (char *)&in, sizeof(cid_adduser_it)))
      return ;
   out.status = htons(SUCCESS) ;
   out.error = htons(CID_ER_NOERROR) ;
   if (!validSynergyuser(in.login)) /* check if valid user */
      if (!addUser(in.login))
       {
          out.status = htons(FAILURE) ;
          out.error = htons(CID_ER_NOADD) ;
       }
   writen(newsock, (char *)&out, sizeof(cid_adduser_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpDeleteUser(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, htons, validSynergyuser, deleteUser
  Notes       : This request is made when a user of this m/c must be removed
                from being a synergy user. Manually a request is sent to
                synergy admin, from whom this request comes to update the
                list in the m/c.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpDeleteUser()
{
   cid_deleteuser_it in ;
   cid_deleteuser_ot out ;

   if (!readn(newsock, (char *)&in, sizeof(cid_deleteuser_it)))
      return ;
   out.status = htons(SUCCESS) ;
   out.error = htons(CID_ER_NOERROR) ;

   if (validSynergyuser(in.login)) /* check if valid user */
      if (!deleteUser(in.login))
       {
          out.status = htons(FAILURE) ;
          out.error = htons(CID_ER_NODELETE) ;
       }
   writen(newsock, (char *)&out, sizeof(cid_deleteuser_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpVerifyUser(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, htons, validSynergyuser
  Notes       : This request is made when a new synergy user is added by the
                synergy admin to verify if the user indeed exists in the m/c.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpVerifyUser()
{
   cid_verifyuser_it in ;
   cid_verifyuser_ot out ;

   if (!readn(newsock, (char *)&in, sizeof(cid_verifyuser_it)))
      return ;                  /* check if valid user */
   if (validSynergyuser(in.login))
    {
       out.status = htons(SUCCESS) ;
       out.error = htons(CID_ER_NOERROR) ;
    }
   else
    {
       out.status = htons(FAILURE) ;
       out.error = htons(CID_ER_NOLOGIN) ;
    }
   writen(newsock, (char *)&out, sizeof(cid_verifyuser_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpVerifyProcess(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, htons, doLogin, access
  Notes       : This request is made to verify whether a user has permissions
                to execute a program.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpVerifyProcess()
{
   cid_verifyprocess_it in ;
   cid_verifyprocess_ot out ;
   char buf[NAME_LEN];
   int i;

   if (!readn(newsock, (char *)&in, sizeof( cid_verifyprocess_it)))
      return ;
                                /* check valid user, change uid of process */
   if (!doLogin(in.login, in.rlogin))
    {
       out.status = htons(FAILURE) ;  out.error = htons(CID_ER_NOLOGIN) ;
       writen(newsock, (char *)&out, sizeof(cid_verifyprocess_ot)) ;
       return ;
    }
                                /* check if could execute component */
   if (in.path[0] == '*')
   {
        for (i=0; i<strlen(in.path)-2; i++)
                in.path[i] = in.path[i+2];
        in.path[i] = '\0';
        sprintf(buf,"bin/%s",in.path);
        strcpy(in.path, buf);
    }
printf("CID verify ****'d process (%s) \n",in.path);
   if (access(in.path, X_OK) == -1)
    {
       out.status = htons(FAILURE) ;  out.error = htons(CID_ER_NOACCESS) ;
       writen(newsock, (char *)&out, sizeof(cid_verifyprocess_ot)) ;
       return ;
    }
   out.status = htons(SUCCESS) ;
   out.error = htons(CID_ER_NOERROR) ;
   writen(newsock, (char *)&out, sizeof(cid_verifyprocess_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpVerifyHandler(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, htons, doLogin, access
  Notes       : This request is made to verify whether a user has permissions
                to execute a handler.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpVerifyHandler()
{
   cid_verifyhandler_it in ;
   cid_verifyhandler_ot out ;
   char *path ;

   if (!readn(newsock, (char *)&in, sizeof(cid_verifyhandler_it)))
      return ;
                                /* check valid user, change uid of process */
   if (!doLogin(in.login, in.rlogin))
    {
       out.status = htons(FAILURE) ;  out.error = htons(CID_ER_NOLOGIN) ;
       writen(newsock, (char *)&out, sizeof(cid_verifyhandler_ot)) ;
       return ;
    }
                                /* check if could execute TSH/FAH */
   path = (ntohs(in.type) == CID_TYPE_TSH) ? tsh_path : fah_path ;
   if (access(path, X_OK) == -1)
    {
       out.status = htons(FAILURE) ;  out.error = htons(CID_ER_NOACCESS) ;
       writen(newsock, (char *)&out, sizeof(cid_verifyhandler_ot)) ;
       return ;
    }
   out.status = htons(SUCCESS) ;
   out.error = htons(CID_ER_NOERROR) ;
   writen(newsock, (char *)&out, sizeof(cid_verifyhandler_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpExit(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, kill, sleep, htons, exit, exitIpc
  Notes       : This request can be made only from DAC to terminate
                the CID. Consequently, all processes in the CID table are
                terminated.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpExit()
{
   cid_exit_ot out ;
   int i ;
   char mapid[128];
                                /* exit-status sent by SIGCHLD handler */
/*   active_cid = 0; */
   for (i = 0 ; i < TABLE_SIZE ; i++)
      if (table[i].used == USED)
      {
         kill(table[i].pid, SIGTERM) ;
         ungetSlot(i) ;
      }
   out.status = htons(SUCCESS) ;  out.error = htons(CID_ER_NOERROR) ;
   writen(newsock, (char *)&out, sizeof(cid_exit_ot)) ;
   sleep(5) ;                   /* wait for all processes to be killed */
   free(pmd_table) ;
   sprintf(mapid,"sng$cid$%s",getpwuid(getuid())->pw_name);
   pmd_unmap(mapid,getpid(),(u_short)PMD_PROT_TCP);
   exit(0) ;
}

/* OpTid is never used in CID */
void OpTid()
{
	return;
}

/*---------------------------------------------------------------------------
  Prototype   : void OpZap(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, kill, sleep, htons, exit, exitIpc
  Notes       : This request is made by an application program to remove
		all CID/TSH internal tuples, virtual and real. This 
		prepares the DSPACE for the next iteration of the
		applications. 
  Date        : November '96
  Coded by    : Yuan Shi 
  Modification:
---------------------------------------------------------------------------*/
void OpZap()
{
   space1_t *t, *s, *m;
   apptq_t *qtr;
   tsh_start_it in;
   int first;

   if (!readn(newsock,(char *)&in, sizeof(tsh_start_it)))
   {
	perror("CID: dzap readn error");
	close(newsock);
	return;
   }

   for (qtr = tsh.space ; qtr != NULL ; qtr = qtr->next)
   {
       if (!strcmp(qtr->appid, in.appid)) break;
   }
   if (qtr == NULL) return;

   /* Remove all virtual tuples */
/*
   printf(" CID**ck: DTS before zap: \n");
   s = qtr->Tqueue ;
   while (s != NULL)
   {
	printf(" CID**ck: (%s) pri(%d)\n", s->name,
		s->priority);
	s = s->next;
   }
*/
   s = qtr->Tqueue;
   while (s != NULL)
   {
	m = s;
	s = s->next;
	free (m);
   }
   qtr->Tqueue = NULL; 
/**** delete only virtual tuples 
   s = qtr->Tqueue ;
   t = qtr->Tqueue ;
   first = 1;
   while (s != NULL)
   {
	m = s;
	s = s->next;
	if (m->priority == 0) 
	{
printf("CID?? deleted a virtual tuple (%s)\n",m->name);
		free (m);
		if (m == qtr->Tqueue)
		{
			qtr->Tqueue = s;
			t = s;
			first = 1;
		} else t->next = s;
	} else if (first) first = 0;
		else t = t->next;
   }
   printf(" CID**ck: DTS after zap: \n");
   s = qtr->Tqueue ;
   while (s != NULL)
   {
	printf(" CID**ck: (%s) pri(%d)\n", s->name,
		s->priority);
	s = s->next;
   }
******************/
   return;
}
   
/*---------------------------------------------------------------------------
  Prototype   : getAppcount(char *appid)
  Parameters  : appid - application id
  Returns     : # of entries in CID table for appid
  Called by   : OpGetApptable
  Calls       : stcmp
  Notes       :
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

int getAppcount(appid)
char *appid;
{
   int i, count ;
                                /* count # of entries for application */
   for (i = 0, count = 0 ; i < TABLE_SIZE ; i++)
      if ((table[i].used == USED) && (!strcmp(appid, table[i].appid)))
         count++ ;
   return count ;
}


/*---------------------------------------------------------------------------
  Prototype   : void sendApptable(char *appid)
  Parameters  : appid - application id
  Returns     : -
  Called by   : OpGetApptable
  Calls       : strcmp, strcpy, writen, hrons
  Notes       : All entries in CID table corresponding to appid are sent.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void sendApptable(appid)
char *appid;
{
   cid_getapptable_ot2 out ;
   int i ;

   for (i = 0 ; i < TABLE_SIZE ; i++)
    {                           /* send entries for appid */
       if ((table[i].used == USED) && (!strcmp(appid, table[i].appid)))
        {
           strcpy(out.name, table[i].name) ;
           out.type = htons(table[i].type) ;
           if (!writen(newsock, (char *)&out, sizeof(cid_getapptable_ot2)))
              return ;
        }
    }
}


/*---------------------------------------------------------------------------
  Prototype   : void sendFulltable(void)
  Parameters  : -
  Returns     : -
  Called by   : OpGetFulltable
  Calls       : strcpy, writen, htons
  Notes       : All entries in the CID table are sent.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void sendFulltable()
{
   cid_getfulltable_ot2 out ;
   int i ;

   for (i = 0 ; i < TABLE_SIZE ; i++)
    {                           /* send every entry in table */
       if (table[i].used == USED)
        {
           strcpy(out.appid, table[i].appid) ;
           strcpy(out.name, table[i].name) ;
           out.type = htons(table[i].type) ;
           if (!writen(newsock, (char *)&out, sizeof( cid_getfulltable_ot2)))
              return ;
        }
    }
}


/*---------------------------------------------------------------------------
  Prototype   : int receiveFile(FILE *fp, u_long len)
  Parameters  : fp  - specifies file in which data must be stored
                len - length of the file that has to be received
  Returns     : 1 - transfer successful
                0 - transfer failed
  Called by   : OpFilePut
  Calls       : malloc, readn, free, fwrite
  Notes       : Data is read from socket in blocks of SEGMENT bytes.
                *-- malloc error return is not checked
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

int receiveFile(fp, len)
FILE *fp;
long len;
{
   int size ;
   char *buff ;

   buff = (char *) malloc(SEGMENT) ;
   while (len > 0)              /* receive, store data in segments */
    {
       size = (len >= SEGMENT) ? SEGMENT : len ;
       if (readn(newsock, (char *)buff, size))
          if (fwrite(buff, 1, size, fp) == size)
           {
              len -= size ;
              continue ;
           }
       free(buff) ;
       return 0 ;
    }
   free(buff) ;
   return 1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : void sendFile(FILE *fp, long len)
  Parameters  : fp  - specifies file from where data has to be read
                len - length of the file that has to be sent
  Returns     : -
  Called by   : OpFileGet
  Calls       : malloc, writen, free, fread
  Notes       : Data is read from file and sent through the socket.
                *-- malloc error return is not checked
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void sendFile(fp, len)
FILE *fp;
long len;
{
   int size ;
   char *buff ;

   buff = (char *) malloc(SEGMENT) ;
   while (len > 0)              /* receive, store data in segments */
    {
       size = (len >= SEGMENT) ? SEGMENT : len ;
       if (fread(buff, 1, size, fp) == size)
          if (writen(newsock, (char *)buff, size))
           {
             len -= size ;
             continue ;
           }
       break ;
    }
   free(buff) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void sigtermHandler(void)
  Parameters  : -
  Returns     : -
  Called by   : System on SIGTERM
  Calls       : exit, exitIpc, kill, sleep
  Notes       : The CID can be terminated by sending SIGTERM signal. It waits
                until exit status of all children could be sent.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void sigtermHandler()
{
   int i ;
                                /* exit-status sent by SIGCHLD handler */
/*   active_cid = 0; */
   for (i = 0 ; i < TABLE_SIZE ; i++)
      if (table[i].used == USED)
      {

         kill(table[i].pid, SIGTERM) ;
         ungetSlot(i) ;
      }
   sleep(5) ;                   /* wait for all processes to be killed */
   printf(" Shut down CID ...\n");
   sprintf(mapid,"sng$cid$%s",getpwuid(getuid())->pw_name);
   pmd_unmap(mapid,getpid(),(u_short)PMD_PROT_TCP);
/*   system("KILL cid");           kill old zomby cid's */
   exit(0) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void sigchldHandler(void)
  Parameters  : -
  Returns     : -
  Called by   : System on SIGTERM
  Parameters  : -
  Calls       : wait3, signal, writen, strcpy, get_socket, do_connect, close
  Notes       : The exit status of the dead child is returned the system (DAC)
                that started it.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification: Only if the return-port is non-zero the exit status is sent
                back.
                cid_exitstatus_ot changed to dac_exitprocess_it.
                FSUN 04/96:
                For local TSH, if BCast reciever is dead, restart it again.
---------------------------------------------------------------------------*/

void sigchldHandler()
{
   static int bcr_cnt = 0;
   int sock, pid, i;
   sng_int16 cidport ;
   dac_exitprocess_it out ;
   int status;
/*
   union wait status ;
*/
   u_short op ;

   while((pid=waitpid(-1,&status,WNOHANG)) > 0)
   {
	printf("CID. subp(%d) terminated \n", pid);
	fflush(stdout);

   	if (WIFEXITED(status)) {
        	status = WEXITSTATUS(status);
        	if (status == NORMAL_EXIT)
                	status = EXITTED;
        	else
                	status = FAILED;
   	} else status = KILLED;
//        printf("CID. subp (%d) term status(%d)\n", pid, status);
/* ReStart BCast receiver. FSUN 04/96 
   	if (pid == bcr_pid && active_cid == 1)
   	{
       		++ bcr_cnt;
       		if (bcr_cnt == 3)
       		{
           		fprintf(stderr, "++ CID :: sigchldHandler cannot start BCR??\n") ;
           		exit(1) ;
       		}
       		for (i = 0 ; i < TABLE_SIZE ; i++)
         		if ((table[i].used == USED) && (table[i].pid == pid))
             			ungetSlot(i) ; 
       		startBCR() ; 
       		return ;
   	}
   	if (pid == fpid) 
   	{
		return; // check if it's a VTget fork 
   	}
*/
    	op = htons(DAC_OP_EXIT_PROCESS)  ;
   	for (i = 0 ; i < TABLE_SIZE ; i++)
        {  /*  Reset Process slot  */
           if ((table[i].used == USED) && (table[i].pid == pid))
           {
		printf("Setup exit status for (%d)\n", pid);
               if (status != EXITTED && table[i].fdd_start == FDD_ALIVE)
	       {
		  printf("Setup abnormal exit: term_stat == 0\n");
                  table[i].term_stat = 0;       /* Abnormal exit */
	       }
               if (table[i].port)
                  if ((sock = get_socket()) != -1)
                   {  // Inform DAC 
                      if (do_connect(sock, table[i].host, table[i].port))
                       {
                          writen(sock, (char *)&op, sizeof(u_short)) ;
                          out.retstatus = htons(status);
                          strcpy(out.name, table[i].name) ;
                          writen(sock, (char *)&out, sizeof(dac_exitprocess_it)) ;
                       }
                      close(sock) ;
                   }
  //          ungetSlot(i) ;  must wait for CID_OP_EXITALL 
               break ;
           }
    	}
#if HPUX | AIX | Linux | SunOs
	signal(SIGCHLD, sigchldHandler);
#endif
   } // of while
   return;
}

void sigAlarmHandler()
{
        return;
}

/*---------------------------------------------------------------------------
  Prototype   : int getSlot(void)
  Parameters  : -
  Returns     : index to empty slot in CID table
                -1 if  table full
  Called by   : OpExecTsh, OpExecComponent
  Calls       : -
  Notes       : If an empty slot exists it's marked USED and returned. The
                semaphore is used only for this operation since all the others
                are not as critical, though the cid table could be locked for
                them too.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

int getSlot()
{
   int i ;
                                /* look for slot in the table */
/*
   semP() ;
*/
   for (i = 0 ; i < TABLE_SIZE ; i++)
    {
       if (table[i].used == UNUSED && table[i].term_stat == 1)
        {
           table[i].used = USED ;
           break ;
        }
    }                           /* if no slot, send error to requestor */
/*
   semV() ;
*/
   return (i < TABLE_SIZE) ? i : -1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : void ungetSlot(int slot)
  Parameters  : slot - index into CID table
  Returns     : -
  Called by   : OpExecTsh, OpExecComponent, sigchldHandler
  Calls       : -
  Notes       : The slot in CIT table is marked UNUSED.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void ungetSlot(slot)
int slot;
{                               /* make slot free in CID table */
   if (slot >= 0 && slot < TABLE_SIZE)
   {
      table[slot].used = UNUSED ; 
      table[slot].term_stat = 1 ;
   }
}


/*---------------------------------------------------------------------------
  Prototype   : int doLogin(char *login, char *rlogin)
  Parameters  : login  - login name of the user in this m/c
                rlogin - login name of the user in remote m/c
  Returns     : 1 - permitted to continue with the operation
                0 - not permitted to continue with the operation
  Called by   : OpExecTsh, OpExecComponent, OpRemove, OpFilePut, OpFileGet
  Calls       : validSynergyuser, getpwnam, setuid, chdir, validRlogin
  Notes       : If permitted to continue the uid of the process is changed
                to login and the current dir. is set to home directory.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

int doLogin(login, rlogin)
char *login;
char *rlogin;
{
   struct passwd *pass ;
                                /* check if valid user of synergy */
/*
   if (!validSynergyuser(login))
      return 0 ;
*/
                                /* get details of the a/c - uid, home dir */
   if ((pass = getpwnam(login)) == NULL)
      return 0 ;
                                /* alter uid of process, working dir */
   if (setuid((uid_t)pass->pw_uid) == -1)
      return 0 ;

   if (chdir(pass->pw_dir) == -1)
      return 0 ;
                                /* check if access is allowed for rlogin */
/*
   if (!validRlogin(rlogin))
      return 0 ;
*/
   return 1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int validSynergyuser(char *login)
  Parameters  : login - login naame of the user in this m/c
  Returns     : 1 - valid Synergy user
                0 - not valid Synergy user
  Called by   : doLogin, OpVerifyUser
  Calls       : fopen, fgets, strcmp, strchr, fclose
  Notes       : The file containing Synergy user list is serched for the
                login specified.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

int validSynergyuser(login)
char *login;
{
   char user[LOGIN_LEN] ;
   FILE *fp ;
   char *tmp, user_file_name[PATH_LEN+LOGIN_LEN] ;
                                /* read file containing all synergy users */
   sprintf(user_file_name,"%s/%s",CID_PATH,USER_FILE);
   if ((fp = fopen(user_file_name, "r")) == NULL)
      return 0 ;
                                /* check if login is a valid synergy user */
   while (fgets(user, LOGIN_LEN, fp) != NULL)
    {
       if ((tmp = strchr(user, '\n')) != NULL)
          *tmp = '\0' ;

       if (!strcmp(user, login))
        {
           fclose(fp) ;
           return 1 ;
        }
    }
   fclose(fp) ;
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int validRlogin(char *rlogin)
  Parameters  : rlogin - login name of user in remote m/c
  Returns     : 1 - permitted for remote access
                0 - not permitted for remote access
  Called by   : doLogin
  Calls       : fopen, fclose, fgets, strcpy, strchr, validRhost
  Notes       : Verifies whether the rlogin is specified in the access file
                in the current dir (home dir of the user in this m/c).
                Entries in this file are in the format rlogin@host.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

int validRlogin(rlogin)
char *rlogin;
{
   char line[80] ;
   FILE *fp ;
   char *tmp, *user, *rhost ;
                                /* read file specifying remote access */
   if ((fp = fopen(RLOGIN_FILE, "r")) == NULL)
      return 0 ;
                                /* check if rlogin is allowed access */
   user = line ;
   while (fgets(line, sizeof(line), fp) != NULL)
    {
       if ((tmp = strchr(line, '@')) == NULL)
          continue ;
       *tmp = '\0' ;
       if (strcmp(user, rlogin)) /* check if valid remote login name */
          continue ;
       rhost = tmp + 1 ;
       if ((tmp = strchr(rhost, '\n')) != NULL)
          *tmp = '\0' ;
        {
       if (validRhost(rhost))   /* check if valid remote host */
           fclose(fp) ;
           return 1 ;
        }
    }
   fclose(fp) ;
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int validRhost(char *rhost)
  Parameters  : rhost - remote host in . format or alphabetic string
  Returns     : 1 - rhost matches with peer
                0 - rhost does not match with peer
  Called by   : validRlogin
  Calls       : isdigit, inet_addr, gethostbyname
  Notes       : If the first char of rhost is a digit it is assumed that
                the address is specified in . format. rhost is converted
                to numerical address and then compared with peer.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

int validRhost(rhost)
char *rhost;
{
   struct hostent *host ;
   int i ;

   if (isdigit(rhost[0]))       /* if 1st char is a digit, then . format */
      if (inet_addr(rhost) == peer)
         return 1 ;
      else
         return 0 ;
                                /* address given as character string */
   if ((host = gethostbyname(rhost)) == NULL)
      return 0 ;
                                /* check if any address matches peer */
   for (i = 0 ; host->h_addr_list[i] ; i++)
      if ((* (u_long *)(host->h_addr_list[i])) == peer)
         return 1 ;
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int addUser(char *login)
  Parameters  : login - login to be added to SYNERGY_USER file
  Returns     : 1 - successfully adder [or] 0 - on error
  Called by   : OpAddUser
  Calls       : fopen, fprintf, fclose, getpwnam
  Notes       : The login is added to the end of the SYNERGY_USER file. Prior
                to that it is verified that login is valid in the m/c.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

int addUser(login)
char *login;
{
   FILE *fp ;
   char user_file_name[PATH_LEN+LOGIN_LEN];
                                /* check if the login exists */
   if (getpwnam(login) == NULL)
      return 0 ;
                                /* open user-file and store login */
   sprintf(user_file_name,"%s/%s",CID_PATH,USER_FILE);
   if ((fp = fopen(user_file_name, "a")) == NULL)
      return 0 ;
   if (fprintf(fp, "%s\n", login) == EOF)
    {
       fclose(fp) ;
       return 0 ;
    }
   fclose(fp) ;
   return 1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int deleteUser(char *login)
  Parameters  : login - login of user to be deleted from USER_LIST
  Returns     : 1 - successfully deleted [or] 0 - on error
  Called by   : OpDeleteUser
  Calls       : fopen, fclose, strcmp, sprintf, fputs, rename
  Notes       : The user-file is stored line by line skipping the 'login'
                in a temporary file. And then the temporary file is moved
                to the user-file.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

int deleteUser(login)
char *login;
{
   FILE *fpin, *fpout ;
   char line[LOGIN_LEN+1], del_line[LOGIN_LEN+1] ;
   char user_file_name[PATH_LEN+LOGIN_LEN],
        user_file_tmp_name[PATH_LEN+LOGIN_LEN], *tmp;
                                /* open user-file & temporary file */
   sprintf(user_file_name,"%s/%s",CID_PATH,USER_FILE);
   if ((fpin = fopen(user_file_name, "r")) == NULL)
      return 0 ;
   sprintf(user_file_tmp_name,"%s/%s",CID_PATH,USER_FILE_TMP);
   if ((fpout = fopen(user_file_tmp_name, "w")) == NULL)
    {
       fclose(fpin) ;
       return 0 ;
    }
   sprintf(del_line, "%s\n", login) ;
                                /* read user-file until login is reached */
   while (fgets(line, sizeof(line), fpin) != NULL)
    {
       if (!strcmp(line, del_line))
          break ;
       fputs(line, fpout) ;
    }
                                /* store rest of the lines */
   while (fgets(line, sizeof(line), fpin) != NULL)
      fputs(line, fpout) ;
   fclose(fpin) ;
   fclose(fpout) ;
   if (rename(user_file_tmp_name, user_file_name) == -1)
      return 0 ;
   return 1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int toFork(void)
  Parameters  : -
  Returns     : 1 - CID has to fork
                0 - CID need not fork
  Called by   : start
  Calls       : -
  Notes       : Based on the operation requested, determines wheteher the
                operation has to be carried out in a separate child process
                or not.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification: Added CID_OP_VERIFY_PROCESS, CID_OP_VERIFY_HANDLER,
                CID_OP_FILE_GET.
---------------------------------------------------------------------------*/

int toFork()
{
   if (this_op == CID_OP_EXEC_TSH || this_op == CID_OP_EXEC_FAH ||
       this_op == CID_OP_EXEC_COMPONENT || this_op == CID_OP_REMOVE ||
       this_op == CID_OP_FILE_PUT || this_op == CID_OP_FILE_GET ||
       this_op == CID_OP_VERIFY_PROCESS || this_op == CID_OP_VERIFY_HANDLER)
      return 1 ;                /* must fork for these operations */
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : void logOps(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : fopen, fclose, fwrite, rename
  Notes       : The requested operation is logged in a file. When the file
                is full it is moved to a backup file and again entries start
                from the beginning.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void logOps()
{
   struct {
      u_short op ;
      u_long peer ;
      long tloc ;
   } rec ;
   static int ops_cnt = OPS_MAX ;
   FILE *fp ;
   char ops_file_name[PATH_LEN+LOGIN_LEN],
        ops_old_file_name[PATH_LEN+LOGIN_LEN];

   rec.op = this_op ;
   rec.peer = peer ;
   time(&rec.tloc) ;

   sprintf(ops_file_name,"%s/%s",CID_PATH,OPS_FILE);
   sprintf(ops_old_file_name,"%s/%s",CID_PATH,OPS_FILE_OLD);
   if (ops_cnt == OPS_MAX)
    {                           /* save old entries */
       rename(ops_file_name, ops_old_file_name) ;
       if ((fp = fopen(ops_file_name, "w")) == NULL)
          return ;
       ops_cnt = 0 ;
    }
   else                         /* if cannot append, over-write file */
      if ((fp = fopen(ops_file_name, "a")) == NULL)
         if ((fp = fopen(ops_file_name, "w")) == NULL)
            return ;
         else
            ops_cnt = 0 ;
                                /* store record in file */
   fwrite(&rec, sizeof(rec), 1, fp ) ;
   fclose(fp) ;
   ops_cnt++ ;
}

void OpExitProcess()
{
   int sock, pid, i, stats ;
   dac_exitprocess_it out ;
   u_short op ;
   cid_exit_process_ot out2 ;

   op = htons(DAC_OP_EXIT_PROCESS)  ;
   readn(newsock, (char *)&out2, sizeof(cid_exit_process_ot));
   pid = out2.error;
   stats = out2.status;

   for (i = 0 ; i < TABLE_SIZE ; i++)
   {
                                        /* FSUN 10/94. For FDD */
           if ((table[i].used == USED) && (table[i].pid == pid))
            {
               if (stats != EXITTED && table[i].fdd_start == FDD_ALIVE)
                  table[i].term_stat = 0;       /* Abnormal exit */
               if (table[i].port)
                  if ((sock = get_socket()) != -1)
                   {
                      if (do_connect(sock, table[i].host, table[i].port))
                       {
                          writen(sock, (char *)&op, sizeof(u_short)) ;
                          out.retstatus = htons(stats);
                          strcpy(out.name, table[i].name) ;
                          writen(sock, (char *)&out, sizeof(dac_exitprocess_it)) ;
                       }
                      close(sock) ;
                   }
               ungetSlot(i) ;
               break ;
            }
    }
}

/* Check local resources: cpu, mem, disk and network. Send status back. */
void OpResourceCk()
{
    char packet[100];
    int i,pid;

    if ((pid=fork()) == 0)
    {
        for (i=0; i<100; i++) packet[i] = 0;
        return_value(packet);
        if (!writen(newsock, packet, 100 )) {
                perror("CID resource checker write socket error\n");
                exit(1);
        }
    }
}

/* Returns the value as specified by command */
void return_value( packet )
    char *packet;
{
  ParseCPU(packet);
  ParseMEM(packet);
  ParseDisk(packet);
  ParseNet(packet);
}

ParseNet(packet)
char *packet;
{
FILE *uu;
char buf[255], *st;
int connections=0;

/* parse received */
if ( (uu=popen("netstat | wc","r")) == NULL )
  perror("popen netstat error");
st=fgets(buf, 255, uu);
sscanf(buf,"%d", &connections);

sprintf(buf, "NET: connections %d ", connections);
strcat(packet, buf);
#ifdef TEST
puts(packet);
#endif
}

ParseMEM(packet)
char *packet;
{
FILE *uu;
char buf[255],*st;
int active=0, free=0;
int position;

if ( (uu=popen("vmstat","r")) == NULL )
  perror("error popen vmstat");

while (st=fgets(buf, 255, uu))
  {
    position=member("avm", buf);
    if (position <= 0)
      position=member("act", buf);
    if (position >0)
      {
        st=fgets(buf, 255, uu);
        sscanf(buf+position-1, "%d%d", &active, &free);
        if (active!=0)
          break;
      }
  }/* end of while */

sprintf(buf, "MEM: %d/%d  ",active, free+active);
strcat(packet, buf);
#ifdef TEST
puts(packet);
#endif
}

ParseDisk(packet)
char *packet;
{
FILE *uu;
char tmp[255], buf[255], *st;
int volume=0, used=0, avail=0, capacity=0;
int sum=0, total_used=0;

if ( (uu=popen("df","r")) == NULL )
  perror("popen df error");

st=fgets(buf, 255, uu);

while (st=fgets(buf, 255, uu))
  {
    sscanf(buf, "%s%d%d%d%d", tmp, &volume, &used, &avail, &capacity);
    if (used !=0 )
      {
        sum=sum+volume;
        total_used=total_used+used;
      }
  } /* end of while */
pclose(uu);

/* doesn't work on zoro */

sprintf(tmp,"DISK: %d/%d  ", total_used, sum);
strcat(packet,tmp);
#ifdef TEST
puts(packet);
#endif
}

ParseCPU(packet)
char *packet;
{
FILE *uu;
char buf[255], *st;
int position, us=0, sy=0, id=0;

if ( (uu=popen("vmstat", "r")) == NULL )
  perror("error popen vmstat");

while (st=fgets(buf, 255, uu))
  {
    /* this takes some brain power. 'coz cpu field is not at constant place*/
    position=member("us",buf);
    if (position >0 )
      {
        st=fgets(buf, 255, uu);
        sscanf(buf+position, "%d%d%d", &us, &sy, &id);
        if (us != 0)
          break;
      }
  }/* end of while */
sprintf(buf,"CPU: %d/%d  ", us+sy, us+sy+id);
strcat(packet, buf);
#ifdef TEST
puts(packet);
#endif
}

member(seed, string)
char *seed, *string;
{
int i, j;

for (i=0; i<strlen(string); i++)
  if ( (seed[0]==string[i]) && (seed[1]==string[i+1]) )
    return(i);
return(-1);
}


/*---------------------------------------------------------------------------
  Prototype   : void pmd_initOther(void)
  Parameters  : -
  Returns     : -
  Called by   : main
  Calls       : signal, fprintf, malloc, time, exit
  Notes       : Memory is allocated for PMD table and handler for SIGTERM is
                installed. When PMD has to be killed, it can be sent SIGTERM.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void pmd_initOther()
{
  int i ;
                                /* to handle exit of PMD */
  if ((pmd_table =
        (pmd_table_t *)malloc(sizeof(pmd_table_t) * TABLE_SIZE)) == NULL)
    {
      fprintf(stderr, "initOther::malloc\n") ;
      exit(1) ;
    }
  for (i = 0 ; i < TABLE_SIZE ; i++)
    pmd_table[i].used = UNUSED ;        /* initialize table entries */
}

/*---------------------------------------------------------------------------
  Prototype   : void pmd_OpNull(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : writen, htons, htonl
  Notes       : This operation is available to test whether the PMD is up.
                The # of entries in its table and also the time from when it
                is up are returned.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void pmd_OpNull()
{
   pmd_null_ot out ;
   int i ;

   for (i = 0, out.count = 0 ; i < TABLE_SIZE ; i++)
      if (pmd_table[i].used == USED)
         out.count++ ;          /* count the number of entries */

   out.status = htons(SUCCESS) ;  out.error = htons(PMD_ER_NOERROR) ;
   out.count = htons(out.count) ;
   out.start_time = htonl(start_time) ;
                                /* return PMD started-time also */
   writen(newsock, (char *)&out, sizeof(pmd_null_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpMap(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : pmd_getSlot, readn, writen, ntohs, htons, ntohl, strcpy
  Notes       : The mapid, protocol, port, pid are stored in PMD table.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpMap()
{
  pmd_map_it in ;
  pmd_map_ot out ;
  int index ;

  if (!readn(newsock, (char *)&in, sizeof(pmd_map_it)))
     return ;                   /* read input for operation */

  if ((index = pmd_getSlot(in.mapid, ntohs(in.protocol))) != -1)
   {
      pmd_table[index].used = USED ;
      strcpy(pmd_table[index].mapid, in.mapid) ;
      pmd_table[index].protocol = ntohs(in.protocol) ;
      pmd_table[index].port = in.port ; /* retain in NW order */
      pmd_table[index].pid = ntohl(in.pid) ;

      out.status = htons(SUCCESS) ; out.error = htons(PMD_ER_NOERROR) ;
      writen(newsock, (char *)&out, sizeof(pmd_map_ot)) ;
      return ;
   }                            /* store mapping in the table */

  out.status = htons(FAILURE) ; out.error = htons(PMD_ER_NOSPACE) ;
  writen(newsock, (char *)&out, sizeof(pmd_map_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpUnmap(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : unmapPid, unmapMapid, readn, writen, ntohl, ntohs, htons
  Notes       : If a pid > 0 is provided, then all entries corresponding
                to that pid is removed else entries for mapid are removed.
                The # of entries removed from PMD table is returned.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpUnmap()
{
  pmd_unmap_it in ;
  pmd_unmap_ot out ;

  if (!readn(newsock, (char *)&in, sizeof( pmd_unmap_it)))
     return ;                   /* read input for operation */

  if (ntohl(in.pid) > 0)        /* remove all entries for pid */
     out.count = unmapPid(ntohl(in.pid)) ;
  else                          /* remove entry for mapid */
     out.count = unmapMapid(in.mapid, ntohs(in.protocol)) ;

  if (out.count > 0)
   {
      out.status = htons(SUCCESS) ;
      out.error = htons(PMD_ER_NOERROR) ;
   }
  else
   {
      out.status = htons(FAILURE) ;
      out.error = htons(PMD_ER_NOENTRY) ;
   }
                                /* return the # of entries unmapped */
  out.count = htons(out.count) ;
  writen(newsock, (char *)&out, sizeof(pmd_unmap_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpGetMap(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, htons, ntohs, strcmp
  Notes       : The port # corresponding to mapid, protocol is returned.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpGetMap()
{
  pmd_getmap_it in ;
  pmd_getmap_ot out ;
  int i ;

  if (!readn(newsock, (char *)&in, sizeof(pmd_getmap_it)))
     return ;                   /* read input for operation */

  for (i = 0 ; i < TABLE_SIZE ; i++)
   {
      if ((pmd_table[i].used == USED) && (!strcmp(pmd_table[i].mapid, in.mapid))
          && (pmd_table[i].protocol == ntohs(in.protocol)))
       {
          out.port = pmd_table[i].port ;
          out.status = htons(SUCCESS) ; out.error = htons(PMD_ER_NOERROR) ;
          writen(newsock, (char *)&out, sizeof(pmd_getmap_ot)) ;
          return ;
       }                        /* look for requested mapid+protocol */
   }
  out.port = 0 ;                /* no match found in table */
  out.status = htons(FAILURE) ; out.error = htons(PMD_ER_NOENTRY) ;
  writen(newsock, (char *)&out, sizeof(pmd_getmap_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpGetTable(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : sendTable, writen, htons, ntohs
  Notes       : The # of entries in the table is first sent to the requestor
                so that resources can be allocated for the table.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpGetTable()
{
   pmd_gettable_ot1 out ;
   int i ;

   for (i = 0, out.count = 0 ; i < TABLE_SIZE ; i++)
      if (pmd_table[i].used == USED)
         out.count++ ;          /* count number of entries in table */

   if (out.count > 0)
    {                           /* entries present */
       out.status = htons(SUCCESS) ;
       out.error = htons(PMD_ER_NOERROR) ;
    }
   else
    {                           /* entries not present - table empty */
       out.status = htons(FAILURE) ;
       out.error = htons(PMD_ER_NOENTRY) ;
    }
   out.count = htons(out.count) ;
   if (!writen(newsock, (char *)&out, sizeof(pmd_gettable_ot1)))
      return ;                  /* send initial data - # of entries */

   if (ntohs(out.count) > 0)    /* send the entries in the table */
      sendTable() ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpPurgeTable(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : writen, htons
  Notes       : All entries in the table are deleted. The count of the
                entries deleted is returned.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpPurgeTable()
{
   pmd_purgetable_ot out ;
   int i ;

   for (i = 0, out.count = 0 ; i < TABLE_SIZE ; i++)
      if (pmd_table[i].used == USED)
       {
          out.count++ ;
          pmd_table[i].used = UNUSED ;
       }                        /* make all entries unused */

   out.status = htons(SUCCESS) ;  out.error = htons(PMD_ER_NOERROR) ;
   out.count = htons(out.count) ;
   writen(newsock, (char *)&out, sizeof(pmd_purgetable_ot)) ;
}

/*---------------------------------------------------------------------------
  Prototype   : int pmd_getSlot(char *mapid, u_short protocol)
  Parameters  : mapid    - logical name for the porto
                protocol - tcp/udp used for communicating with the port
  Returns     : index into PMD table to store the mapping [or]
                -1 if mapping cannot be stored
  Called by   : OpMap
  Calls       : strcmp
  Notes       : If an entry exists already for the mapping it will be over-
                written.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

int pmd_getSlot(mapid, protocol)
char *mapid;
u_short protocol;
{
   int i, index ;

   for (i = 0, index = -1 ; i < TABLE_SIZE ; i++)
    {
       if ((pmd_table[i].used == UNUSED) && (index == -1))
          index = i ;           /* look for an empty slot */

       if (!strcmp(pmd_table[i].mapid, mapid) && (pmd_table[i].protocol == protocol))
        {
           index = i ;
           break ;              /* check whether entry already present */
        }                       /* to be over-written */
    }
   return index ;
}


/*---------------------------------------------------------------------------
  Prototype   : int unmapPid(u_long pid)
  Parameters  : pid - pid for which all entries must be deleted
  Returns     : # of entries deleted from PMD table
  Called by   : OpUnmap
  Calls       : -
  Notes       : This is available to cleanup PMD table when a process
                terminates.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

int unmapPid(pid)
u_long pid;
{
   int i, count ;

   for (i = 0, count = 0 ; i < TABLE_SIZE ; i++)
    {
       if ((pmd_table[i].used == USED) && (pmd_table[i].pid == pid))
        {
           pmd_table[i].used = UNUSED ;
           count++ ;            /* delete all entries matching pid */
        }
    }
   return count ;
}


/*---------------------------------------------------------------------------
  Prototype   : int unmapMapid(char *mapid, u_short protocol)
  Parameters  : mapid    - mapid of entry to be deleted
                protocol - tcp/udp
  Returns     : 1 - entry present and deleted
                0 - entry not present
  Called by   : OpUnmap
  Calls       : strcpy
  Notes       : An entry in the PMD table is identified by the 2- tuple
                <mapid, protocol>
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

int unmapMapid(mapid, protocol)
char *mapid;
u_short protocol;
{
   int i, count ;

   for (i = 0, count = 0 ; i < TABLE_SIZE ; i++)
    {
       if ((pmd_table[i].used == USED) && (!strcmp(pmd_table[i].mapid, mapid))
           && (protocol == pmd_table[i].protocol))
        {
           pmd_table[i].used = UNUSED ;
           count = 1 ;
           break ;              /* delete entry matching parameters */
        }
    }
   return count ;
}


/*---------------------------------------------------------------------------
  Prototype   : void sendTable(void)
  Parameters  : -
  Returns     : -
  Called by   : OpGetTable
  Calls       : writen, htons
  Notes       : All the entries <mapid, protocol, port> in the table are sent
                to the requestor.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void sendTable()
{
   pmd_gettable_ot2 out ;
   int i ;

   for (i = 0 ; i < TABLE_SIZE ; i++)
    {                           /* send valid entries in the table */
       if (pmd_table[i].used == USED)
        {
           strcpy(out.mapid, pmd_table[i].mapid) ;
           out.protocol = htons(pmd_table[i].protocol) ;
           out.port = pmd_table[i].port ;
           if (!writen(newsock, (char *)&out, sizeof(pmd_gettable_ot2)))
              return ;
        }
    }
}



/*---------------------------------------------------------------------------
  Prototype   : void pmd_logOps(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : fopen, fclose, fwrite, rename
  Notes       : The requested operation is logged in a file. When the file
                is full it is moved to a backup file and again entries start
                from the beginning.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void pmd_logOps()
{
   struct {
      u_short op ;
      u_long peer ;
      long tloc ;
   } rec ;
   static int ops_cnt = OPS_MAX ;
   FILE *fp ;

   rec.op = this_op ;
   rec.peer = peer ;
   time(&rec.tloc) ;

   if (ops_cnt == OPS_MAX)
    {                           /* save old entries */
       rename(OPS_FILE, OPS_FILE_OLD) ;
       if ((fp = fopen(OPS_FILE, "w")) == NULL)
          return ;
       ops_cnt = 0 ;
    }
   else                         /* if cannot append, over-write file */
      if ((fp = fopen(OPS_FILE, "a")) == NULL)
         if ((fp = fopen(OPS_FILE, "w")) == NULL)
            return ;
         else
            ops_cnt = 0 ;

   fwrite(&rec, sizeof(rec), 1, fp ) ;
   fclose(fp) ;
   ops_cnt++ ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpSaveAppMsg(void)
  Parameters  : -
  Returns     : Never returns
  Called by   : start
  Calls       : malloc, readn, free
  Notes       :
  Date        : July' 94
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void OpSaveAppMsg()
{
        cid_saveappmsg_it in;
        appmsg_t *p_q, *cutp;

        if ((p_q = (appmsg_t *) malloc(sizeof(appmsg_t))) == NULL)
                exit(E_MALLOC);
        if (readn(newsock, (char *)&in, sizeof(cid_saveappmsg_it))) {
                strcpy(p_q->app_id, in.app_id);
                p_q->dacport = in.dacport;
                p_q->app_status = in.app_status;
                p_q->num_process = ntohs(in.num_process);
                strcpy(p_q->start_time, in.start_time);
                p_q->flag = 0;
                p_q->next = NULL;
                if (p_q->app_status != APP_RUNNING) p_q->elapsed_time = 0;
                else p_q->elapsed_time = time((long *)0);
                strcpy(p_q->complete_time,"0 Sec.");

                list_cnt++;     /* increase the length of exit_process_list */
                if (list_cnt == 1)
                        list_p = end_p = p_q;
                else {
                        list_p->next = p_q;
                        list_p = p_q;
                }
        } else {
                printf("CID saving app message failure.\n");
                free(p_q);
        }
}


/*---------------------------------------------------------------------------
  Prototype   : void OpChangeAppMsg(void)
  Parameters  : -
  Returns     : Never returns
  Called by   : start
  Calls       : readn
  Notes       :
  Date        : July' 94
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/
void OpChangeAppMsg()
{
        appmsg_t *p_q;
        cid_changeappmsg_it in;
        int t;

        if (readn(newsock, (char *)&in, sizeof(cid_changeappmsg_it))) {
                p_q = end_p;
                while (p_q != NULL) {
                        if (!strcmp(p_q->app_id, in.app_id)) {
                                if (in.app_status == APP_RUNNING)
                                        p_q->elapsed_time = time((long *)0);
                                if (in.app_status == APP_READY)
                                        p_q->elapsed_time = 0;
                                if (in.app_status == APP_EXITTED ||
                                                in.app_status == APP_KILLED) {
                                        if (p_q->elapsed_time == 0)
                                        strcpy(p_q->complete_time,"0 Sec.");
                                        else {
                                        sprintf(p_q->complete_time,"%ld Sec.",
                                                time((long *)0) -
                                                p_q->elapsed_time);
                                        p_q->elapsed_time = time((long *)0) -
                                                p_q->elapsed_time;
                                        }
                                }
                                p_q->app_status = in.app_status;
                                break;
                        }
                        p_q = p_q->next;
                }
                return;
        }
        printf("CID changing app message failure.\n");
}

/*---------------------------------------------------------------------------
  Prototype   : void OpGetAppList(void)
  Parameters  : -
  Returns     : Never returns
  Called by   : start
  Calls       : writen
  Notes       :
  Date        : July' 94
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void OpGetAppList()
{
        cid_getapplist_ot out;
        appmsg_t *p_q;
        int i, j, k;

        out.list_cnt = htons(list_cnt);
        p_q = end_p;
        if (list_cnt > 0) {
            i = j = k = 0;
            while (list_cnt > BUFFE_SIZE * j) {
                j++;
                for ( ; i < BUFFE_SIZE * j && i < list_cnt; i ++) {
                        k = i - BUFFE_SIZE * (j - 1);
                        strcpy((out.buffer[k]).app_id, p_q->app_id);
                        (out.buffer[k]).dacport = p_q->dacport;
                        (out.buffer[k]).app_status = p_q->app_status;
                        (out.buffer[k]).num_process = htons(p_q->num_process);
                        strcpy((out.buffer[k]).start_time, p_q->start_time);
                        if (p_q->app_status == APP_RUNNING)
                                sprintf((out.buffer[k]).complete_time,"%ld Sec.",
                                time((long *)0) - p_q->elapsed_time);
                        else
                                sprintf((out.buffer[k]).complete_time,"%ld Sec.",
                                 p_q->elapsed_time);
                        p_q = p_q->next;
                }
                if (!writen(newsock, (char *)&out, sizeof(cid_getapplist_ot))) {
                        printf("CID sending app list failure.\n");
                        break;
                }
            }
        } else
            if (!writen(newsock, (char *)&out, sizeof(cid_getapplist_ot)))
                printf("CID sending app list failure.\n");
}

/*---------------------------------------------------------------------------
  Prototype   : void OpGetApp(void)
  Parameters  : -
  Returns     : Never returns
  Called by   : start
  Calls       : readn, strcmp, writen
  Notes       : Special for MP program. Appname = Filename(.prcd).
  Date        : July' 94
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void OpGetApp()
{
        cid_getapp_ot out;
        char appname_it[NAME_LEN], appname_lt[NAME_LEN];
        appmsg_t *p_q;
        int i, findout;

        findout = i = 0;
        if (readn(newsock, appname_it, sizeof(appname_it))) {
            p_q = end_p;
            while (p_q != NULL) {
                if (strcmp(p_q->app_id, appname_it)) {
                    if (!p_q->flag) {
                        for(i = 0; (p_q->app_id)[i] != '$'; i ++)
                            appname_lt[i] = (p_q->app_id)[i];
                        appname_lt[i] = '\0';
                        if (!strcmp(appname_lt, appname_it)) {
                            p_q->flag = findout = 1;
                            break;
                        }
                    }
                } else {
                    findout = 1;
                    break;
                }
                p_q = p_q->next;
            }
            if (findout) {
                strcpy(out.app_id, p_q->app_id);
                out.dacport = p_q->dacport;
                out.app_status = p_q->app_status;
                out.num_process = htons(p_q->num_process);
                strcpy(out.start_time, p_q->start_time);
                if (p_q->app_status == APP_RUNNING)
                        sprintf(out.complete_time,"%ld Sec.", time((long *)0) -
                        p_q->elapsed_time);
                else sprintf(out.complete_time,"%ld Sec.",p_q->elapsed_time);
                if (writen(newsock, (char *)&out, sizeof(cid_getapp_ot )))
                    return;
            } else {
                out.dacport = 0;
                if (writen(newsock, (char *)&out, sizeof(cid_getapp_ot )))
                    return;
            }
        }
        printf("CID sending app message failure.\n");
}


/*---------------------------------------------------------------------------
  Prototype   : void OpDelApp(void)
  Parameters  : -
  Returns     : Never returns
  Called by   : start
  Calls       : readn, strcmp, printf, free
  Notes       :
  Date        : July' 94
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void OpDelApp()
{
        char appname_it[NAME_LEN];
        appmsg_t *p_q1, *p_q2;
        int i, findout;

        findout = 0;
        if (readn(newsock, appname_it, sizeof(appname_it))) {
            p_q1 = p_q2 = end_p;
            while (p_q1 != NULL) {

/*

                for(i = 0; (p_q1->app_id)[i] != '$'; i ++)
                    appname_lt[i] = (p_q1->app_id)[i];
                appname_lt[i] = '\0';

*/

                if (!strcmp(p_q1->app_id, appname_it)) {
                    findout = 1;
                    break;
                }
                p_q2 = p_q1;
                p_q1 = p_q1->next;
            }
            if (findout)
                if (p_q1->app_status == APP_RUNNING)
                    printf("(%s) is ready or running, it cannot be deleted.\n",
                                appname_it);
                else {
                    if (p_q1 == p_q2)
                        end_p = p_q1->next;
                    else if (p_q1->next == NULL)
                        list_p = p_q2;
                    else
                        p_q2->next = p_q1->next;
                    free(p_q1);
                    list_cnt --;
                }
            else
                printf("(%s) not present in CID_APP_LIST\n", appname_it);
        } else
            printf("CID delete app message failure.\n");
}


/*---------------------------------------------------------------------------
  Prototype   : void OpDelAppList(void)
  Parameters  : -
  Returns     : Never returns
  Called by   : start
  Calls       : printf, free
  Notes       :
  Date        : July' 94
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void OpDelAppList()
{
        appmsg_t *p_q1, *p_q2;

        p_q1 = p_q2 = end_p;
        while (p_q2 != NULL) {
            if (p_q2->app_status != APP_RUNNING) {
                if (p_q2 == p_q1) {
                        end_p = p_q2->next;
                        free(p_q2);
                        p_q1 = p_q2 = end_p;
                        list_cnt --;
                } else {
                        p_q1->next = p_q2->next;
                        free(p_q2);
                        p_q2 = p_q1->next;
                        list_cnt --;
                }
            } else
                if (p_q1 == p_q2)
                        p_q2 = p_q2->next;
                else {
                        p_q1 = p_q1->next;
                        p_q2 = p_q2->next;
                }
        }
        list_p = p_q1;
}


void kill_old_cid(cidport)
sng_int16 cidport;
{
   cid_exit_ot in ;
   int sock ;
   u_long cid_host ;
   struct hostent *host ;
   char host_name[MAP_LEN];
   u_short this_op = CID_OP_EXIT;

   alarm(15);
                                /* determine address of CID */
   gethostname(host_name,sizeof(host_name));
   if (isdigit(host_name[0]))
      cid_host = inet_addr(host_name) ;
   else
    {
       if ((host = gethostbyname(host_name)) == NULL)
        {
           printf("Error: gethostbyname.\n") ;
           exit(1) ;
        }
       cid_host = *((long *)host->h_addr_list[0]) ;
    }
                                /* get socket and connect to CID */
   if ((sock = get_socket()) == -1)
    {
       printf("Error: Cannot get socket.\n") ;
       exit(1) ;
    }
   if (!do_connect(sock, cid_host, cidport))
    {
/*        printf("\n No old CID found. \n") ;
*/
        return;
    }
   printf("Found an old CID. \n");

   this_op = htons(this_op);
   if (!writen(sock, (char *)&this_op, sizeof(u_short)))
   {
        printf("Found a malfunctioning CID ...\n");
        return;
   }
                                /* read result from CID */
   if (!readn(sock, (char *)&in, sizeof(cid_exit_ot)))
    {
       printf("Found a malfunctioning CID ...\n") ;
       return ;
    }
   printf("Removed an old CID \n");
   return;
}

delete_app(appname_it)
char *appname_it;
{
        appmsg_t *p_q1, *p_q2;
        int i, findout;

            p_q1 = p_q2 = end_p;
            while (p_q1 != NULL) {
                if (!strcmp(p_q1->app_id, appname_it)) {
                    findout = 1;
                    break;
                }
                p_q2 = p_q1;
                p_q1 = p_q1->next;
            }
            if (findout)
            {
                    if (p_q1 == p_q2)
                        end_p = p_q1->next;
                    else if (p_q1->next == NULL)
                        list_p = p_q2;
                    else
                        p_q2->next = p_q1->next;
                    free(p_q1);
                    list_cnt --;
            }
            printf("CID deleted app (%s).\n",appname_it);
}

u_short bench()
{
        int i;
        double d, a=1.1,b=123.0,c=0.12334;
        double t0;

        t0 = second();
        for (i = 0; i<100000*multiplier; i++)
                d = a*a*i - 4*b*c*i + b*i*b - c*i*i*c;
        return (u_short)(second() - t0);
}


/*---------------------------------------------------------------------------
  Prototype   : void OpDetectProc(void)
  Parameters  : -
  Returns     : -
  Called by   :
  Calls       :
  Notes       :
  Date        : October'94
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void OpDetectProc()
{
   cid_detectproc_it in ;
   cid_detectproc_ot out ;
   int i ;
                                /* read application id, component name */
   if (!readn(newsock, (char *)&in, sizeof(cid_detectproc_it)))
      return ;
   in.proc_id = ntohl(in.proc_id);
                                /* search table for component */
   for (i = 0 ; i < TABLE_SIZE ; i++)
    {
/*
       if (!strcmp(in.appid, table[i].appid)) 
	printf("    CID. OpDetectProc.  pid(%d) tbpid(%d) st(%d)\n",
	in.proc_id, table[i].pid, table[i].term_stat);
*/
	// Return SUCC if already finished. 
       if ((table[i].term_stat == 1) && (in.proc_id == table[i].pid)
                && (!strcmp(in.appid, table[i].appid)))
        {
           out.status = htons(SUCCESS) ;  out.error = htons(CID_ER_NOERROR) ;
           writen(newsock, (char *)&out, sizeof(cid_detectproc_ot)) ;
           return ;
        }
	// Return FAILURE if pid was dead. 
       if ((table[i].term_stat == 0) && (in.proc_id == table[i].pid)
                && (!strcmp(in.appid, table[i].appid)))
        {
           table[i].term_stat = 1;
           out.status = htons(FAILURE) ;  out.error = htons(CID_ER_NOERROR) ;
           writen(newsock, (char *)&out, sizeof(cid_detectproc_ot)) ;
           return;
        }
    }
   out.status = htons(SUCCESS) ;  out.error = htons(CID_ER_NOENTRY) ;
   writen(newsock, (char *)&out, sizeof(cid_detectproc_ot)) ;
   return;
}

/*---- The following is the Local TSH Part ----*/

/*---------------------------------------------------------------------------
  Prototype   : void OpPut(void)
  Parameters  : -
  Returns     : - 
  Called by   : start
  Calls       : createTuple, consumeTuple, storeTuple, readn, writen,
                ntohs, ntohl, malloc, free
  Notes       : Copied and modified from TSH.C (N. Isaac Rajkumar)
  Date        : April '96
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void OpPut()
{
   tsh_put_it in ;
   tsh_put_ot out ;
   space1_t *s ;
   char *t ;

   out.error =  htons((short int)TSH_ER_NOERROR);
   out.status = htons((short int)SUCCESS);
                                /* read tuple length, priority, name */
   if (!readn(newsock, (char *)&in, sizeof(tsh_put_it)))
      return ;
   in.proc_id = ntohs(in.proc_id);
   if (guardf(in.host, in.proc_id)) return;
                                /* allocate memory for the tuple */

   if ((t = (char *)malloc(ntohl(in.length))) == NULL)
    {
       perror("CID/TSH. No memory\n");
       free(t) ;
       out.status = htons((short int)FAILURE) ;
       out.error = htons((short int)TSH_ER_NOMEM) ;
       writen(newsock, (char *)&out, sizeof(tsh_put_ot)) ;
       return ;
    }                           /* read the tuple */
   if (!readn(newsock, t, ntohl(in.length)))
    {
       perror(" CID. OpPut. Readn error??? \n");
       free(t) ;
       return ;
    }                           /* create and store tuple in space */
   s = createTuple(in.appid, in.name, t, ntohl(in.length), ntohs(in.priority)) ;
/*
   printf("***CID: OpPut. pri(%d) name(%s) val(%s)\n",
		ntohs(in.priority), in.name, t);
*/
   if (s == NULL)
    {
/*
printf(" CID. OpPut. Cannot create tuple??? \n");
*/
       free(t) ;
       out.status = htons((short int)FAILURE) ;
       out.error = htons((short int)TSH_ER_NOMEM) ;
    }
   else
    {                   /* satisfy pending requests, if possible */
       if (!consumeTuple(s)) {
          out.error = htons(storeTuple(s, 0)) ;
      } else {
          out.error = htons((short int)TSH_ER_NOERROR) ;
       }
       out.status = htons((short int)SUCCESS);
    }
   writen(newsock, (char *)&out, sizeof(tsh_put_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpGet(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : findTuple, deleteTuple, storeRequest, readn, writen,
                strcpy, htons
  Notes       : Copied and modified from TSH.C (N. Isaac Rajkumar)
  Date        : April '96
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void OpGet()
{
   tsh_get_it in ;
   tsh_get_ot1 out1, in1 ;
   tsh_get_ot2 out2 ;
   space1_t *s ;
   int request_len ;
   u_long r_hostid;
   int r_Tcidport, u, subsock;
                                /* read tuple name */
   if (!readn(newsock, (char *)&in, sizeof(tsh_get_it)))
      return ;
/*
printf(" CID: Got a request:(%s) appid(%s)\n", in.expr, in.appid);
*/

   in.proc_id = ntohs(in.proc_id);
   if (guardf(in.host, in.proc_id)) return;
   request_len = ntohl(in.len); /* get user requested length */
                                /* locate tuple in tuple space */
   if ((s = findTuple(in.appid, in.expr, 1)) == NULL)
    {
/*
printf(" CID. Cannot find requested tuple??? (%s)\n", in.expr);
*/
       out1.status = htons(FAILURE) ;
       if (request_len != -1)   /* -1: async read/get. Do not queue */
       {
	  if ((ntohl(in.host) !=0) && (ntohs(in.port) != 0))
          if (!storeRequest(in.appid, in.expr, in.host, in.port, in.proc_id))
             out1.error = htons(TSH_ER_NOMEM) ;
          else
             out1.error = htons(TSH_ER_NOTUPLE) ;
/*
printf(" CID: TsGet. Cannot find local tuple (%s). request stored\n", in.expr);
*/
       }
       writen(newsock, (char *)&out1, sizeof(tsh_get_ot1)) ;
       return ;
    }

   out1.status = htons(SUCCESS) ; out1.error = htons(TSH_ER_NOERROR) ;
   if (!writen(newsock, (char *)&out1, sizeof(tsh_get_ot1)))
      return ;
                                /* send tuple name, length and priority */
   strcpy(out2.appid, s->appid) ;
   strcpy(out2.name, s->name) ;
   if ((s->length > request_len) && (request_len != 0) &&
       (htons(s->priority) != 0))
        out2.length = in.len;
   else out2.length = htonl(s->length) ;
   out2.priority = htons(s->priority) ;
   if (!writen(newsock, (char *)&out2, sizeof(tsh_get_ot2)))
      return ;
                                /* send the tuple */
/*
printf("*CID. sending tuple. size(%d)\n", ntohl(out2.length));
*/
   if (!writen(newsock, s->tuple, ntohl(out2.length)/*s->length*/))
      return ;
                                /* read the result */
   if (!readn(newsock, (char *)&in1, sizeof(tsh_get_ot1)))
      return ;

   if ((ntohs(in1.status) == SUCCESS) && (ntohs(in1.error) == TSH_ER_NOERROR) &&
       (this_op == TSH_OP_GET))
      deleteTuple(s);
}


/*---------------------------------------------------------------------------
  Prototype   : int consumeTuple(space1_t *s)
  Parameters  : s - pointer to tuple that has to be consumed
  Returns     : 1 - tuple consumed
                0 - tuple not consumed
  Called by   : OpPut
  Calls       : findRequest, sendTuple, deleteRequest
  Notes       : Copied and modified from TSH.C (N. Isaac Rajkumar)
                If there is a pending request that matches this tuple, it
                is sent to the requestor (served FIFO). If there were only
                pending TSH_OP_READs (or no pending requests) then the tuple
                is considered not consumed. If a TSH_OP_GET was encountered
                then the tuple is considered consumed.
  Date        : April '96
  Coded by    : Feijian Sun
  Modification: The Stru.of TS has been changed to TWO levels for both Tuples
		and Requests. FSUN 09/96
---------------------------------------------------------------------------*/

int consumeTuple(s)
space1_t *s;
{
   queue1_t *q ;
                                /* check whether request pending */
   if ((q = findRequest(s->appid, s->name)) != NULL)
    {
       do
        {               /* send tuple to requestor, delete request */
           if (sendTuple(q, s) > 0)
              if (q->request == TSH_OP_GET)
              {
                  deleteRequest(q) ;
                  free(s);
                  return 1 ;    /* tuple consumed */
              }
              deleteRequest(q) ;
                                /* check for another pending request */
        } while ((q = findRequest(s->appid, s->name)) != NULL) ;
    }
   return 0 ;                   /* tuple not consumed */
}


/*---------------------------------------------------------------------------
  Prototype   : space1_t *createTuple(char *appid, char *name, u_long length,
                                                           u_short priority)
  Parameters  : appid    - application id
                name     - tuple name
                length   - length of tuple
                priority - priority of the tuple
  Returns     : pointer to a tuple made of the input [or] NULL if no memory
  Called by   : OpPut
  Calls       : malloc, strcpy
  Notes       : Copied and modified from TSH.C (N. Isaac Rajkumar)
                This function creates a tuple and fills up the attributes.
  Date        : April '96
  Coded by    : Feijian Sun
  Modification: The Stru.of TS has been changed to TWO levels for both Tuples
		and Requests. FSUN 09/96
---------------------------------------------------------------------------*/

space1_t *createTuple(appid, name, tuple, length, priority)
char *appid;
char *name;
char *tuple;
u_long length;
u_short priority;
{
   space1_t *s ;
                                /* create a new node and store tuple */
   if ((s = (space1_t *) malloc(sizeof(space1_t))) == NULL)
      return NULL ;
   strcpy(s->appid, appid) ;
   strcpy(s->name, name) ;
   s->length = length ;
   s->tuple = tuple ;
   s->priority = priority ;

   return s ;                   /* return new tuple */
}


/*---------------------------------------------------------------------------
  Prototype   : short int storeTuple(space1_t *s)
  Parameters  : s - pointer to tuple that has to be stored
  Returns     : -
  Called by   : OpPut
  Calls       : strcmp, free
  Notes       : Copied and modified from TSH.C (N. Isaac Rajkumar)
                The tuple is stored in the tuple space. If another tuple
                exists with the same name it is replaced.
  Date        : April '96
  Coded by    : Feijian Sun
  Modification: The Stru.of TS has been changed to TWO levels for both Tuples
		and Requests. FSUN 09/96
---------------------------------------------------------------------------*/

short int storeTuple(s, f)
space1_t *s;
int f;
{
   space1_t *ptr;
   apptq_t *qtr;
/*
printf("CID::storeTuple::appid:(%s)name(%s)\n", s->appid, s->name);
*/

   for (qtr = tsh.space ; qtr != NULL ; qtr = qtr->next)
       if (!strcmp(qtr->appid, s->appid)) break;
   if (qtr == NULL)
   {				 /* create a new node and store tuple */
       if ((qtr = (apptq_t *) malloc(sizeof(apptq_t))) == NULL)
          exit(E_MALLOC);
       strcpy(qtr->appid, s->appid) ;
       qtr->Tqueue = NULL;
       qtr->next = tsh.space ;
       qtr->prev = NULL ;
       tsh.space = qtr ;
   }
                                /* check if tuple already there */
   for (ptr = qtr->Tqueue ; ptr != NULL ; ptr = ptr->next)
    {
       if (!strcmp(ptr->name, s->name))
        {                       /* overwrite existing tuple */
           free(ptr->tuple) ;
           ptr->tuple = s->tuple ;
           ptr->length = s->length ;
           ptr->priority = s->priority ;
           free(s) ;
           return ((short int)TSH_ER_OVERRT) ;
        }
       if (ptr->next == NULL)
          break ;
    }
        if (f == 0) {           /* add tuple to end of space */
                s->next = NULL ;
                s->prev = ptr ;
                if (ptr == NULL)
                        qtr->Tqueue= s ;
                else
                        ptr->next = s ;
        } else {        /* add tuple retrieved to header of space */
                s->next = qtr->Tqueue;
                s->prev = NULL;
                qtr->Tqueue = s;
        }
   return((short int)TSH_ER_NOERROR);
}


/*---------------------------------------------------------------------------
  Prototype   : space1_t *findTuple(char *appid, char *expr, int *d)
  Parameters  : appid - application id
                expr - wildcard expression (*, ?)
                dire - find the highest/lowest priority
  Returns     : pointer to tuple in the tuple space
  Called by   : OpGet
  Calls       : match
  Notes       : Copied and modified from TSH.C (N. Isaac Rajkumar)
                The tuple matching the wildcard expression & with the
                highest priority of all the matches is determined and
                returned.
  Date        : April '96
  Coded by    : Feijian Sun
  Modification: The Stru.of TS has been changed to TWO levels for both Tuples
		and Requests. FSUN 09/96
---------------------------------------------------------------------------*/

space1_t *findTuple(appid, expr, d)
char *appid;
char *expr;
int d;
{
   space1_t *tp = NULL, *s ;
   apptq_t *qtr;

/*
printf(" CID. OpGet. Finding tuple (%s) appid(%s)\n", expr, appid);
*/
   for (qtr = tsh.space ; qtr != NULL ; qtr = qtr->next)
   {
       if (!strcmp(qtr->appid, appid)) break;
   }
   if (qtr == NULL) return NULL;

                                /* search tuple space for highest priority */
   for (s = qtr->Tqueue ; s != NULL ; s = s->next)
      if (d)
        if (match(expr, s->name) &&
             ((tp == NULL) || (s->priority > tp->priority)))
        tp = s ;
      else
        if (match(expr, s->name) &&
             ((tp == NULL) || (s->priority < tp->priority)))
        tp = s ;



   return tp ;                  /* return tuple or NULL if no match */
}


/*---------------------------------------------------------------------------
  Prototype   : void deleteTuple(space1_t *s)
  Parameters  : s - pointer to tuple to be deleted from tuple space
  Returns     : -
  Called by   : OpGet
  Calls       : free
  Notes       : Copied and modified from TSH.C (N. Isaac Rajkumar)
                The tuple is removed from the tuple space.
  Date        : April '96
  Coded by    : Feijian Sun
  Modification: The Stru.of TS has been changed to TWO levels for both Tuples
		and Requests. FSUN 09/96
---------------------------------------------------------------------------*/
void deleteTuple(s)
space1_t *s;
{
	apptq_t *qtr;

	for (qtr = tsh.space ; qtr != NULL ; qtr = qtr->next)
		if (!strcmp(qtr->appid, s->appid)) break;
	if (qtr == NULL) return;

        if (s == qtr->Tqueue)             /* remove tuple from space */
                qtr->Tqueue = s->next ;
        else
                s->prev->next = s->next ;
        if (s->next != NULL) s->next->prev = s->prev  ;
/*
printf("CID::deleteTuple::appid:(%s)name(%s)\n", s->appid, s->name);
*/
        free(s);
}


/*---------------------------------------------------------------------------
  Prototype   : int sendTuple(queue1_t *q, space1_t *s)
  Parameters  : q - pointer to pending request entry in the queue
                s - pointer to the tuple to be sent
  Returns     : 1 - tuple successfully sent
                0 - tuple not sent
  Called by   : consumeTuple
  Calls       : get_socket, do_connect, writen, close
  Notes       : Copied and modified from TSH.C (N. Isaac Rajkumar)
                The tuple is sent to the host/port specified in the request.
  Date        : April '96
  Coded by    : Feijian Sun
  Modification: The Stru.of TS has been changed to TWO levels for both Tuples
		and Requests. FSUN 09/96
---------------------------------------------------------------------------*/

int sendTuple(q, s)
queue1_t *q;
space1_t *s;
{
   tsh_get_ot2 out ;
   int sd ;
                                /* connect to the requestor */
   if ((sd = get_socket()) == -1)
   {
        printf(" Cannot getsocket for tuple object. Call system operator. \n");
        exit (E_SOCKET);
   }

   if (!do_connect(sd, q->host, q->port))
    {
       close(sd) ;
       return (E_CONNECT);
    }
                                /* send tuple name, length, priority */
   strcpy(out.appid, s->appid) ;
   strcpy(out.name, s->name) ;
   out.priority = htons(s->priority) ;
   out.length = htonl(s->length) ;
   if (!writen(sd, (char *)&out, sizeof(tsh_get_ot2)))
    {
       close(sd) ;
       return (E_CONNECT);
    }                           /* send tuple data */
   if (!writen(sd, s->tuple, s->length))
    {
       close(sd) ;
       return (E_CONNECT);
    }
   close(sd);
   return 1 ;                   /* tuple successfully sent */
}


/*---------------------------------------------------------------------------
  Prototype   : queue_t *findRequest(char *appid, char *name)
  Parameters  : appid - application id
                name  - tuple name
  Returns     : pointer to a request for this tuple
                NULL if no request for this tuple
  Called by   : consumeTuple
  Calls       : match
  Notes       : Copied and modified from TSH.C (N. Isaac Rajkumar)
                If there is a pending request for the specified tuple, it is
                returned.
  Date        : April '96
  Coded by    : Feijian Sun
  Modification: The Stru.of TS has been changed to TWO levels for both Tuples
		and Requests. FSUN 09/96
---------------------------------------------------------------------------*/

queue1_t *findRequest(appid, name)
char *appid;
char *name;
{
   queue1_t *q ;
   apprq_t *qtr;

   for (qtr = tsh.request ; qtr != NULL ; qtr = qtr->next)
       if (!strcmp(qtr->appid, appid)) break;
   if (qtr == NULL) return NULL;
                                /* search pending request queue */
   for (q = qtr->queue_hd ; q != NULL ; q = q->next)
   {
      if (match(q->expr, name))
      {
         return q ;             /* determine request that matches tuple */
      }
   }

   return NULL ;
}


/*---------------------------------------------------------------------------
  Prototype   : void deleteRequest(queue1_t *q)
  Parameters  : q - pointer to request in queue
  Returns     : -
  Called by   : OpPut
  Calls       : free
  Notes       : Copied and modified from TSH.C (N. Isaac Rajkumar)
                The specified request is removed from the pending requests
                queue.
  Date        : April '96
  Coded by    : Feijian Sun
  Modification: The Stru.of TS has been changed to TWO levels for both Tuples
		and Requests. FSUN 09/96
---------------------------------------------------------------------------*/

void deleteRequest(q)
queue1_t *q;
{
   apprq_t *qtr;

   for (qtr = tsh.request ; qtr != NULL ; qtr = qtr->next)
       if (!strcmp(qtr->appid, q->appid)) break;
   if (qtr == NULL) return;
                               /* remove request from queue */
   if (q == qtr->queue_hd)
      qtr->queue_hd = q->next ;
   else
      q->prev->next = q->next ;

   if (q == qtr->queue_tl)
      qtr->queue_tl = q->prev ;
   else
      q->next->prev = q->prev ;

   free(q) ;                    /* free request */
}


/*---------------------------------------------------------------------------
  Prototype   : int storeRequest(char *appid, char *expr, u_long host,
                                 sng_int16 port, int proc_id)
  Parameters  : appid - application id
                expr  - name (wildcard expression) of the requested tuple
                host  - address of the reque*stor
                port  - port at which the tuple has to be delivered
  Returns     : 1 - request stored
                0 - no space to store request
  Called by   : OpGet
  Calls       : malloc, strcpy
  Notes       : Copied and modified from TSH.C (N. Isaac Rajkumar)
                A request based on the parameters is placed in the pending
                request queue.
  Date        : April '96
  Coded by    : Feijian Sun
  Modification: The Stru.of TS has been changed to TWO levels for both Tuples
		and Requests. FSUN 09/96
---------------------------------------------------------------------------*/

int storeRequest(appid, expr, host, port, procid)
char *appid;
char *expr;
u_long host;
sng_int16 port;
int procid;
{
   queue1_t *q ;
   apprq_t *qtr;

   for (qtr = tsh.request ; qtr != NULL ; qtr = qtr->next)
       if (!strcmp(qtr->appid, appid)) break;
   if (qtr == NULL)
   {                             /* create a new node and store request */
       if ((qtr = (apprq_t *) malloc(sizeof(apprq_t))) == NULL)
          return 0 ;
       qtr->queue_hd = NULL;
       qtr->queue_tl = NULL;
       strcpy(qtr->appid, appid) ;
       qtr->next = tsh.request ;
       qtr->prev = NULL ;
       tsh.request = qtr ;
   }
                                /* create node for request */
   if ((q = (queue1_t *)malloc(sizeof(queue1_t))) == NULL)
      return 0 ;
   strcpy(q->appid, appid) ;
   strcpy(q->expr, expr) ;
   q->port = port ;
   q->host = host ;
   q->request = this_op ;
   q->next = NULL ;
   q->prev = qtr->queue_tl ;
                                /* store request in queue */
   if (qtr->queue_tl == NULL)
      qtr->queue_hd = q ;
   else
      qtr->queue_tl->next = q ;
   qtr->queue_tl = q ;
   return 1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int match(char *expr, char *name)
  Parameters  : expr - wildcard expression
                name - string to be matched with the wildcard
  Returns     : 1 - expr & name match
                0 - expr & name do not match
  Called by   : match, findRequest, findTuple
  Calls       : match
  Notes       : Copied and modified from TSH.C (N. Isaac Rajkumar)
                The expression can contain wild card characters '*' and '?'.
                I like this function.
  Date        : April '96
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

int match(expr, name)
char *expr;
char *name;
{
   /* Need to add app_id match checking */
   while (*expr == *name)       /* skip as long as the strings match */
    {
       if (*name++ == '\0')
          return 1 ;
       expr++ ;
    }
   if (*expr == '?')
    {                           /* '?' - skip one character */
       if (*name != '\0')
          return (match(++expr, ++name)) ;
    }
   else if (*expr == '*')
    {
       expr++ ;
       do                       /* '*' - skip 0 or more characters */
        {                       /* try for each of the cases */
           if (match(expr, name))
              return 1 ;
        } while (*name++ != '\0') ;
    }
   return 0 ;                   /* no match found */
}


/*---------------------------------------------------------------------------
  Prototype     : short int BCast_out(space1_t *s, u_short VTop)
  Parameters    : -
  Called by     : OpPut, OpGet
  Calls         :
  Notes         : Each time there is a tuple to be put into or cut from Tuple
                  Space, the CID will BROADCAST all others.
  Date          : April '96
  Coded by      : Feijian Sun
  Modification  :
---------------------------------------------------------------------------*/

short int BCast_out(s, VTop)
space1_t *s ;
u_short VTop;
{
       int                     sockfd;
       struct sockaddr_in      cli_addr, serv_addr;
       cid_send_bc             out_bc;
       int optval, optsize;

       /* Creat BCast message. FSUN 04/96 */
       strcpy(out_bc.bckey, bcid);
       strcpy(out_bc.appid, s->appid);
       strcpy(out_bc.name, s->name);
       out_bc.priority = htons(s->priority);
       out_bc.host = hostid;
       out_bc.port = Tcidport;
       out_bc.opflag = htons(VTop);

       bzero((char *) &serv_addr, sizeof(serv_addr));
       serv_addr.sin_family      =AF_INET;
/*       serv_addr.sin_addr.s_addr = inet_addr("192.168.171.255");*/
       serv_addr.sin_addr.s_addr = inet_addr("128.84.21.255");
       serv_addr.sin_port        = htons(SERV_UDP_PORT);

       if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
       {

        optsize = sizeof(optval);
        if (getsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char *) &optval, &optsize) >=0)
        {
          optval = 1;
          if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char *) &optval, optsize) >=0)
            if (getsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char *) &optval, &optsize) >=0)
            {
		 bzero((char *) &cli_addr, sizeof(cli_addr));
		 cli_addr.sin_family      = AF_INET;
		 cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		 cli_addr.sin_port        = htons(0);

		 if (bind(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) >= 0)
		 {
		   if (sendto(sockfd, (char *) &out_bc, sizeof(out_bc), 0,
	                 (struct sockaddr *) &serv_addr, sizeof(serv_addr))	
			 == sizeof(out_bc))
		   {
		       close(sockfd);
		       return ((short int) CID_ER_NOERROR);
		   }
		 }
            }
        }
        close(sockfd);
       }
       return ((short int) TSH_ER_NOBCAST);
}


/*---------------------------------------------------------------------------
  Prototype   : void OpVTUpd(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn
  Notes       :
  Date        : August '96
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void OpVTUpd()
{
   cid_recieve_bc in_bc;
   int  bcq_i, i;
                                /* read the Length of VT Update Package. */
/*
   if (!readn(newsock, (char *)&bcq_i, sizeof(int)))
      return ;
   bcq_i = ntohs(bcq_i);
printf(" CID: Vtuple Op. size(%d) \n", bcq_i);
*/
                                /* read the VT Update Package. */
   if (!readn(newsock, (char *)&in_bc, sizeof(in_bc)))
      return ;
                                /* search table for component */
   in_bc.opflag = ntohs(in_bc.opflag);
   in_bc.host = (in_bc.host);
   in_bc.port = (in_bc.port);
   if (in_bc.opflag == VTPUT_FLAG) VTPut(in_bc);
   else
   if (in_bc.opflag == VTGET_FLAG) VTGet(&in_bc); 
}


/*---------------------------------------------------------------------------
  Prototype   : void VTPut(void)
  Parameters  : -
  Returns     : -
  Called by   : OpVTUpd 
  Calls       : readn, writen
  Notes       :
  Date        : April '96
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void VTPut(in_bc)
cid_recieve_bc in_bc;
{
   char *r_tpvalue;
   queue1_t *q ;
   space1_t *s ;

/*
printf(" CID received V_Tput (%s) host(%ul) port(%d) \n", 
	in_bc.name, in_bc.host, in_bc.port); 
*/


   if ((r_tpvalue = (char *)malloc(128)) == NULL)
    {
       free(r_tpvalue) ;
       return ;
    }
   sprintf(r_tpvalue, "sng$bc$%d$%ul\0", (int)in_bc.port, in_bc.host);

   /* The priority of virtual tuple = 0 while the real one's >=1 */
   s = createTuple(in_bc.appid, in_bc.name, r_tpvalue, strlen(r_tpvalue), 0) ;

/*
   printf("***CID: VTPut. name(%s) val(%s)\n", in_bc.name, r_tpvalue );
*/
   if (s != NULL)
   {
	if (!consumeTuple(s)) 
	  storeTuple(s,0); 
   }
   else
   {
       free(r_tpvalue);
       exit(0);
   }
}


/*---------------------------------------------------------------------------
  Prototype   : void VTGet(void)
  Parameters  : -
  Returns     : -
  Called by   : OpVTUpd 
  Calls       : readn, writen
  Notes       :
  Date        : April '96
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void VTGet(in_bc)
cid_recieve_bc in_bc;
{
   space1_t *s ;

/*
printf(" CID: vt_get (%s)\n", in_bc.name); 
*/
                                /* search tuple space for highest priority */
   if ((s = findTuple(in_bc.appid, in_bc.name, 0)) != NULL)
      deleteTuple(s) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpRetrieve()
  Parameters  : -
  Returns     : -
  Called by   :
  Calls       :
  Notes       :
  Date        : October '94
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void OpRetrieve()
{
        tsh_retrieve_it in;
        space1_t *s;
        space2_t *p_q, *p_p;
	int find = 0;

        if (!readn(newsock, (char *)&in, sizeof(tsh_retrieve_it)))
                return ;
	in.proc_id = ntohl(in.proc_id);
        p_q = tsh.retrieve;
        while (p_q != NULL) {
                if (p_q->host == in.host && p_q->proc_id == in.proc_id) {
                        s = createTuple(p_q->name, p_q->tuple, p_q->length, 
						p_q->priority);
                        if (!consumeTuple(s)) storeTuple(s, 1);
                        p_q->fault = 1;
			p_q->tuple = NULL;
			find = 1;
			return;
                }
                p_q = p_q->next;
        }
        p_p = (space2_t *)malloc(sizeof(space2_t));
        p_p->host = in.host;
	p_p->proc_id = in.proc_id;
        p_p->fault = 1;
        p_p->next = tsh.retrieve;
	tsh.retrieve = p_p;
}


/*---------------------------------------------------------------------------
  Prototype   : void guardf(u_long hostid, int procid)
  Parameters  : -
  Returns     : -
  Called by   :
  Calls       :
  Notes       :
  Date        : October '94
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

int guardf(hostid, procid)
u_long hostid;
int procid;
{
        space2_t *p_q;

        p_q = tsh.retrieve;
        while (p_q != NULL) {
                if (p_q->fault == 1 && p_q->host == hostid
				&& p_q->proc_id == procid)
                        return(1);
		p_q = p_q->next;
	}
        return(0);
}


/*---------------------------------------------------------------------------
  Prototype   :  DelTRQue(char *appid)
  Parameters  : -
  Returns     : -
  Called by   : OpKillApp
  Calls       :
  Notes       : While CID kills an app., it will remove the Tuple and Request 
		Queues corresponding to this app. in TS at the same time by 
		calling this func. 
  Date        : September '96
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

DelTRQue(appid)
char *appid;
{
   apptq_t *ptr;
   apprq_t *qtr;
   space1_t *s ;
   queue1_t *q ;

   for (ptr = tsh.space ; ptr != NULL ; ptr = ptr->next)
       if (!strcmp(ptr->appid, appid)) break;
   if (ptr != NULL)
   {
       for (s=ptr->Tqueue; s != NULL; s=ptr->Tqueue)
	{
	    ptr->Tqueue = s->next;
	    free(s) ;
	}
       ptr->prev->next = ptr->next;
       ptr->next->prev = ptr->prev;
       free(ptr) ;
   }

   for (qtr = tsh.request ; qtr != NULL ; qtr = qtr->next)
       if (!strcmp(qtr->appid, appid)) break;
   if (qtr != NULL)
   {
       for (q=qtr->queue_hd; q != NULL; q=qtr->queue_hd)
        {
            qtr->queue_hd = q->next;
            free(q) ;
        }
       qtr->prev->next = qtr->next;
       qtr->next->prev = qtr->prev;
       free(qtr) ;
   }
}

/* daemon() - detach process from user and disappear into the background  */
/* returns -1 on failure, but you can't do much except exit in that case, */
/* since we may already have forked. This is based on the BSD version,    */
/* so the caller is responsible for things like the umask, etc.           */

/* believed to work on all Posix systems */

int
daemon (int nochdir, int noclose)
{
  int status;
  switch (fork ())
    {
    case 0:
      break;
    case -1:
      return -1;
    default:
      _exit (0);                /* exit the original process */
    }

  if (setsid () < 0)            /* shoudn't fail */
    return -1;

  /* dyke out this switch if you want to acquire a control tty in the */
  /* future - not normally advisable for daemons */

  switch (fork ())
    {
    case 0:
      break;
    case -1:
      return -1;
    default:
      _exit (0);
    }

  if (!nochdir)
    status=chdir ("/");

  if (!noclose)
    {
      closeall (0);
      open ("/dev/null", O_RDWR);
      status=dup (0);
      status=dup (0);
    }

  return 0;
}

/* closeall() - close all FDs >= a specified value */

void
closeall (int fd)
{
  int fdlimit = sysconf (_SC_OPEN_MAX);

  while (fd < fdlimit)
    close (fd++);
}
