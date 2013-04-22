/*.........................................................................*/
/*               DACLIB.C ------> DAC library functions                    */
/*                 February '13, updated by Justin Y. Shi                  */
/*.........................................................................*/

#include "synergy.h"
/*
#if (SunOs == 0) && (Ultrix == 0) && (EPIX == 0) && (Linux == 0)
*/
union sng_wait
{
	int	w_status;		/* used in syscall		*/

	struct				/* terminated process status	*/
	{
#if     BYTE_ORDER == BIG_ENDIAN
		unsigned short  w_PAD16;
                unsigned        w_Retcode:8;    /* exit code if w_termsig==0 */
                unsigned        w_Coredump:1;   /* core dump indicator */
                unsigned        w_Termsig:7;    /* termination signal */
#else
                unsigned 	w_Termsig:7;    /* termination signal */
                unsigned 	w_Coredump:1;   /* core dump indicator */
                unsigned 	w_Retcode:8;    /* exit code if w_termsig==0 */
                unsigned short  w_PAD16;
#endif
	} w_T;
	/*
         * Stopped process status.  Returned
         * only for traced children unless requested
         * with the WUNTRACED option bit.
         */

        struct {
#if     BYTE_ORDER == BIG_ENDIAN
                unsigned short  w_PAD16;
                unsigned        w_Stopsig:8;    /* signal that stopped us */
                unsigned        w_Stopval:8;    /* == _WSTOPPED if stopped */
#else
                unsigned	w_Stopval:8;    /* == _WSTOPPED if stopped */
                unsigned	w_Stopsig:8;    /* signal that stopped us */
                unsigned short  w_PAD16;
#endif
	} w_S;
};
/*
#endif
*/

u_short dacerrno ;		/* error # available to caller of daclib */

int connect_dac(/*u_short*/) ;
int r_connect_dac();

/*---------------------------------------------------------------------------
  Prototype   : int start_dac(u_short *dacport, char *fname, u_short prunport, 
			int debug, int parent_pid, int fdd_start)
  Parameters  : dacport  - location to store the port # of DAC
                fname    - filename of .prcd file
		prunport - PRUN port
  Returns     : 0 - error in DAC exec
                1 - error in PMD
		2 - error in .prcd file
		3 - DAC started
  Calls       : fork, setpgrp, exit, wait3, sprintf, gethostid, pmd_getmap
  Notes       : The DAC is executed in the child process. If there is an
                exec error/if prcd file error the child exits immediately.
		The parent process waits for 2 secs before proceding (to
		provide time for DAC to read .prcd file). Subsequently, 
		DAC port is obtained from PMD by specifying it's pid.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification: Added prunport as a parameter to send PRUN port to DAC.
		Feijian Sun, 07/94. 
---------------------------------------------------------------------------*/  

int start_dac(dacport, fname, prunport, debug, parent_pid, fdd_start)
u_short *dacport; 
char *fname; 
u_short prunport;
int debug; 
int parent_pid;
int fdd_start;
{
   int pid ;
   char spid[2*MAP_LEN] ;

#if (SunOs == 0) && (Ultrix == 0) && (EPIX == 0) && (Linux == 0)
   union sng_wait status ;
#else			
// union wait status ;
   int status;
#endif


   int noexec ;
   char DAC_PATH[128] ;	/* Remove path dependency. YS 1994 */ 
   char sshport[16], buf[5], buf2[16], buf3[16];

   sprintf(sshport, "%u", prunport);
   if (getenv("SNG_PATH") == NULL)
   {
	printf("SNG_PATH undefined. Exiting ... \n");
	exit(0);
   }
   sprintf(DAC_PATH,"%s/bin/dac",getenv("SNG_PATH")); 
   sprintf(buf,"%d",debug);
   sprintf(buf2,"%d",parent_pid);
   sprintf(buf3,"%d",fdd_start);
// printf("Starting DAC path(%s) fname(%s) sshport(%s)\n", DAC_PATH, fname, sshport);
   noexec = 0 ;
   if (!(pid = fork()))		/* exec DAC */
    {
/*
#ifdef IRIX
       BSD_setpgrp(0, getpid()) ; 
#endif
#ifdef Linux
*/
       setpgrp();
/*
#else
       setpgrp(0, getpid()) ; 
#endif
*/
       execl(DAC_PATH, "dac", fname, sshport, buf, buf2, buf3, (char *)0) ;
       noexec = 1 ;
       exit(1) ;
    }
   sleep(2) ;			/* check to see if child died */
   if (wait3(&status, WNOHANG, (struct rusage *)0) > 0)
     if (noexec)
       return 0 ;		/* error in PMD */
     else if (!ping_pmd(sng_gethostid()))
{
       return 1 ;
}
     else			/* error in .prcd file */
       return 2 ;
				/* obtain DAC port from PMD */
   sprintf(spid, "%s+%d$%d", fname, parent_pid, pid) ;
   if ((short)(*dacport = sngd_getmap(spid, sng_gethostid(), 
	getpwuid(getuid())->pw_name, PMD_PROT_TCP)) == -1)
     return 1 ;
   return 3 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int start_app(u_short dacport)
  Parameters  : dacport - DAC port #
  Returns     :  1 - application started
                 0 - application not started [error in dacerrno]
		-1 - commn. error
  Errors      : DAC_ER_NOERROR
                DAC_ER_STARTED
		DAC_ER_EXECHANDLER
  Calls       : connect_dac, htons, writen, readn, close
  Notes       : Application is started through DAC.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int start_app(dacport)
u_short dacport;
{
   dac_start_ot in ;
   u_short this_op ;
   int sd ;
				/* connect to DAC */
   if ((sd = connect_dac(dacport)) == -1)
      return -1 ;
				/* send op-code and read response */
   this_op = htons(DAC_OP_START) ;
   if (writen(sd, (char *)&this_op, sizeof(u_short)))
      if (readn(sd, (char *)&in, sizeof(dac_start_ot)))
       {
	  close(sd) ;
	  dacerrno = ntohs(in.error) ;
	  in.status = ntohs(in.status);
	  if (in.status == SUCCESS)
	     return 1 ;
	  return 0 ;
       }
   close(sd) ;
   return -1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int save_process_status(u_short dacport, char *name)
  Parameters  : dacport - DAC port #
                name    - name of process
  Returns     :  1 - process exit-status will be saved
                 0 - process exit-status will not be saved [error in dacerrno]
		-1 - commn. error
  Errors      : DAC_ER_NOERROR
                DAC_ER_STARTED
		DAC_ER_NOPROCESS
  Calls       : connect_dac, htons, readn, writen, strcpy, close
  Notes       : The exit-status of the process will be saved by DAC.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int save_process_status(dacport, name)
u_short dacport; 
char *name;
{
   dac_savestatus_it out ;
   dac_savestatus_ot in ;
   u_short this_op ;
   int sd ;
				/* connect to DAC */
   if ((sd = connect_dac(dacport)) == -1)
      return -1 ;
				/* send op-code, process name & read response*/
   this_op = htons(DAC_OP_SAVE_STATUS) ;
   strcpy(out.name, name) ;
   if (writen(sd, (char *)&this_op, sizeof(u_short)))
      if (writen(sd, (char *)&out, sizeof(dac_savestatus_it)))
	 if (readn(sd, (char *)&in, sizeof(dac_savestatus_ot)))
	  {
	     close(sd) ;
	     dacerrno = ntohs(in.error) ;
	     if (ntohs(in.status) == SUCCESS)
		return 1 ;
	     return 0 ;
	  }
   close(sd) ;
   return -1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int save_all_status(u_short dacport)
  Parameters  : dacport - DAC port #
  Returns     :  1 - all exit-status will be saved
                 0 - all exit-status will not be saved [error in dacerrno]
		-1 - commn. error
  Errors      : DAC_ER_NOERROR
                DAC_ER_STARTED
  Calls       : connect_dac, htons, readn, writen, close
  Notes       : Exit-status of all the processes will be saved by DAC.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int save_all_status(dacport)
u_short dacport;
{
   dac_savestatusall_ot in ;
   u_short this_op ;
   int sd ;
				/* connect to DAC */
   if ((sd = connect_dac(dacport)) == -1)
      return -1 ;
				/* send op-code & read response */
   this_op = htons(DAC_OP_SAVE_STATUS_ALL) ;
   if (writen(sd, (char *)&this_op, sizeof(u_short)))
      if (readn(sd, (char *)&in, sizeof(dac_savestatusall_ot)))
       {
	  close(sd) ;
	  dacerrno = ntohs(in.error) ;
	  if (ntohs(in.status) == SUCCESS)
	     return 1 ;
	  return 0 ;
       }
   close(sd) ;
   return -1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int verify_app(u_short dacport, char *name)
  Parameters  : dacport - DAC port #
                name    - pointer to location of size NAME_LEN where the name 
		of a process/handler will be stored if it's not executable.
		If name is a NULL ptr then process/handler name is not copied.
  Returns     :  1 - application executable
                 0 - error in application process/handler [error in dacerrno]
		-1 - commn. error
  Errors      : DAC_ER_NOERROR
                DAC_ER_EXECHANDLER
		DAC_ER_EXECPROCESS
  Calls       : connect_dac, htons, readn, writen, strcpy, close
  Notes       : The permissions to execute all components of the application
                are verified.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int verify_app(dacport, name)
u_short dacport; 
char *name;
{
   dac_verifyapp_ot in ;
   u_short this_op ;
   int sd ;
				/* connect to DAC */
   if ((sd = connect_dac(dacport)) == -1)
      return -1 ;
				/* send op-code & read response */
   this_op = htons(DAC_OP_VERIFY_APP) ;
   if (writen(sd, (char *)&this_op, sizeof(u_short)))
      if (readn(sd, (char *)&in, sizeof(dac_verifyapp_ot)))
       {
	  close(sd) ;		/* return name if 'name' is not NULL */
	  if (name != NULL)
	     strcpy(name, in.name) ;
	  dacerrno = ntohs(in.error) ;
	  in.status = ntohs(in.status);
	  if (in.status == SUCCESS)
	     return 1 ;
	  return 0 ;
       }
   close(sd) ;
   return -2 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int get_process_status(char *host, u_short dacport, char *name, 
                                           u_short *state, u_char *retstatus)
  Parameters  : dacport   - DAC port #
                name      - name of process
                state     - location to store state of process
		retstatus - location to store exit-status of process
  Returns     :  1 - exit-status obtained
                 0 - exit-status not obtained
		-1 - commn. error
  Errors      : DAC_ER_NOERROR
                DAC_ER_NOTSTARTED
		DAC_ER_NOPROCESS
  Calls       : connect_dac, htons, strcpy, readn, writen, close
  Notes       : The state and the exit-status are returned if valid process.
                Exit-status is meaningful only if the state is EXITTED.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification: Revised to enable remote contacts. Yuan Shi 11-15-96
---------------------------------------------------------------------------*/  

int get_process_status(host, dacport, name, state, retstatus)
char *host;
u_short dacport; 
char *name; 
u_short *state; 
u_char *retstatus;
{
   dac_getstatus_it out ;
   dac_getstatus_ot in ;
   u_short this_op ;
   int sd ;
				/* connect to DAC */
   if ((sd = r_connect_dac(host, dacport)) == -1)
      return -1 ;
				/* send op-code, process name & read response*/
   this_op = htons(DAC_OP_GET_STATUS) ;
   strcpy(out.name, name) ;
   if (writen(sd, (char *)&this_op, sizeof(u_short)))
      if (writen(sd, (char *)&out, sizeof(dac_getstatus_it)))
	 if (readn(sd, (char *)&in, sizeof(dac_getstatus_ot)))
	  {
	     close(sd) ;	/* store process state, exit-status */
	     dacerrno = ntohs(in.error) ;
	     *state = ntohs(in.state) ;
	     *retstatus = ntohs(in.retstatus) ;
	     if (ntohs(in.status) == SUCCESS)
		return 1 ;
	     return 0 ;
	  }
   close(sd) ;
   return -1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int get_process_state(char *host, u_short dacport, char *name, 
                                                              u_short *state)
  Parameters  : dacport - DAC port #
                name   - name of process
                state   - location to store state of process
  Returns     :  1 - process-state obtatined
                 0 - process state not obtained [error in dacerrno]
		-1 - commn. error
  Errors      : DAC_ER_NOERROR
                DAC_ER_NOPROCESS
  Calls       : connect_dac, readn, writen, htons, strcpy, close
  Notes       : The process-state [waiting/ready/running/started/exitted/
                failed/killed] is returned if valid process.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification: Revised to enable remote contact. Yuan Shi 11-15-96
---------------------------------------------------------------------------*/  

int get_process_state(host, dacport, name, state)
char *host;
u_short dacport; 
char *name; 
u_short *state;
{
   dac_getstate_it out ;
   dac_getstate_ot in ;
   u_short this_op ;
   int sd ;
				/* connect to DAC */
   if ((sd = r_connect_dac(host, dacport)) == -1)
      return -1 ;
				/* send op-code, process name & get response */
   this_op = htons(DAC_OP_GET_STATE) ;
   strcpy(out.name, name) ;
   if (writen(sd, (char *)&this_op, sizeof(u_short)))
      if (writen(sd, (char *)&out, sizeof(dac_getstate_it)))
	 if (readn(sd, (char *)&in, sizeof(dac_getstate_ot)))
	  {
	     close(sd) ;	/* return process-state */
	     dacerrno = ntohs( in.error) ;
	     *state = ntohs(in.state) ;
	     if (ntohs(in.status) == SUCCESS)
		return 1 ;
	     return 0 ;
	  }
   close(sd) ;
   return -1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int get_app_state(char *host, u_short dacport, u_short *state)
  Parameters  : dacport - DAC port #
                state   - location to store state of application
  Returns     :  1 - application state obtained
                 0 - application state not obtained [error in dacerrno]
		-1 - commn. error
  Errors      : DAC_ER_NOERROR
  Calls       : connect_dac, htons, readn, writen, close
  Notes       : The application status is obtained from DAC and returned.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification: Revised to enable remote contact. Yuan Shi 11-15-96
---------------------------------------------------------------------------*/  

int get_app_state(host, dacport, state)
char *host;
u_short dacport; 
u_short *state; 
{
   dac_getappstate_ot in ;
   u_short this_op ;
   int sd ;
				/* connect to DAC */
   if ((sd = r_connect_dac(host, dacport)) == -1)
      return -1 ;
				/* send op-cod & get response */
   this_op = htons(DAC_OP_GET_APPSTATE) ;
   if (writen(sd, (char *)&this_op, sizeof(u_short)))
      if (readn(sd, (char *)&in, sizeof(dac_getappstate_ot)))
       {
	  close(sd) ;		/* store application-state */
	  dacerrno = ntohs(in.error) ;
	  *state = ntohs(in.state) ;
	  if (ntohs(in.status) == SUCCESS)
	     return 1 ;
	  return 0 ;
       }
   close(sd) ;
   return -1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int kill_process(char *host, u_short dacport, char *name)
  Parameters  : dacport - DAC port #
                name - name of process
  Returns     :  1 - process killed
                 0 - process not killed [error in dacerrno]
		-1 - commn. error
  Errors      : DAC_ER_NOERROR
                DAC_ER_NOPROCESS
  Calls       : connect_dac, htons, readn, writen, strcpy, close
  Notes       : The specified process is killed.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification: Revised to enable remote contact. Yuan Shi 11-15-96 
---------------------------------------------------------------------------*/  

int kill_process(host, dacport, name)
char *host;
u_short dacport; 
char *name;
{
   dac_kill_it out ;
   dac_kill_ot in ;
   u_short this_op ;
   int sd ;
				/* connect to DAC */
   if ((sd = r_connect_dac(host, dacport)) == -1)
      return -1 ;
				/* send op-code, process name & read response*/
   this_op = htons(DAC_OP_KILL) ;
   strcpy(out.name, name) ;
   if (writen(sd, (char *)&this_op, sizeof(u_short)))
      if (writen(sd, (char *)&out, sizeof(dac_kill_it)))
	 if (readn(sd, (char *)&in, sizeof(dac_kill_ot)))
	  {
	     close(sd) ;
	     dacerrno = ntohs(in.error) ;
	     if (ntohs(in.status) == SUCCESS)
		return 1 ;
	     return 0 ;
	  }
   close(sd) ;
   return -1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int exit_app(char *host, u_short dacport)
  Parameters  : dacport - DAC port #
  Returns     :  1 - all processes killed
                 0 - error in killing processes [error in dacerrno]
		-1 - commn. error
  Errors      : DAC_ER_NOERROR
  Calls       : connect_dac, htons, writen, readn, close
  Notes       : All processes of the application are killed.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification: Revised to enable remote contact. Yuan Shi 11-15-96
---------------------------------------------------------------------------*/  

int exit_app(host, dacport)
char *host;
u_short dacport;
{
   dac_killall_ot in ;
   u_short this_op ;
   int sd ;
				/* connect to DAC */
   if ((sd = r_connect_dac(host, dacport)) == -1)
   {
	   close(sd);
	   return -1 ;
   }
				/* send op-code & read response */
   this_op = htons(DAC_OP_KILL_ALL) ;
   if (writen(sd, (char *)&this_op, sizeof(u_short)))
      if (readn(sd, (char *)&in, sizeof(dac_killall_ot)))
       {
	  close(sd) ;
	  dacerrno = ntohs(in.error) ;
	  if (ntohs(in.status) == SUCCESS)
	     return 1 ;
	  return 0 ;
       }
   close(sd) ;
   return -1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int exit_dac(u_short dacport)
  Parameters  : dacport - DAC port #
  Returns     :  1 - DAC exitted
                 0 - DAC not exitted [error in dacerrno]
		-1 - commn. error
  Errors      : DAC_ER_NOERROR
  Calls       : connect_dac, readn, writen, htons, close
  Notes       : DAC and all processes/handlers associated with the application
                are exitted.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int exit_dac(host, dacport)
char *host;
u_short dacport;
{
   dac_exit_ot in ;
   u_short this_op ;
   int sd ;
				/* connect to DAC */
   if ((sd = r_connect_dac(host, dacport)) == -1)
      return -1 ;
				/* send op-code & get response */
   this_op = htons(DAC_OP_EXIT) ;
   if (writen(sd, (char *)&this_op, sizeof(u_short)))
      if (readn(sd, (char *)&in, sizeof(dac_exit_ot)))
       {
	  close(sd) ;
	  dacerrno = ntohs(in.error) ;
	  if (ntohs(in.status) == SUCCESS)
	     return 1 ;
	  return 0 ;
       }
   close(sd) ;
   return -1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int r_connect_dac(char *host, u_short dacport)
  Parameters  : host - remote host name
		dacport - DAC port #
  Returns     : socket descriptor of connection to DAC [or] -1 on error
  Called by   : start_app, save_process_status, save_all_status, verify_app,
                get_process_status, get_process_state, get_app_state, 
		kill_process, exit_app, exit_dac
  Calls       : get_socket, do_connect, gethostid
  Notes       : connection is made to DAC [at dacport].
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification: Revised to enable remote contact. Yuan Shi 11-15-96
---------------------------------------------------------------------------*/  

int r_connect_dac(host, dacport)
char *host;
u_short dacport;
{
   int sd ;

   if ((sd = get_socket()) != -1)
      if (do_connect(sd, inet_addr(host), dacport))
	 return sd ;
   return -1 ;
}
/*---------------------------------------------------------------------------
  Prototype   : int connect_dac(u_short dacport)
  Parameters  : dacport - DAC port #
  Returns     : socket descriptor of connection to DAC [or] -1 on error
  Called by   : start_app, save_process_status, save_all_status, verify_app,
                get_process_status, get_process_state, get_app_state, 
		kill_process, exit_app, exit_dac
  Calls       : get_socket, do_connect, gethostid
  Notes       : connection is made to DAC [at dacport].
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int connect_dac(dacport)
u_short dacport;
{
   int sd ;

   if ((sd = get_socket()) != -1)
      if (do_connect(sd, sng_gethostid(), dacport))
	 return sd ;
   return -1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int getproclist(char *host, u_short dacport, 
			procinfo_t **list_proc, **end_proc int *plist_cnt)
  Parameters  : -
  Returns     : 
  Called by   : 
  Calls       :
  Notes       :
  Date        : July' 94
  Coded by    : Feijian Sun
  Modification: Revised to enable remote contact. Yuan Shi 11-15-96
---------------------------------------------------------------------------*/

int getproclist(host, dacport, list_proc, end_proc, plist_cnt)
char *host;
u_short dacport;
procinfo_t **list_proc, **end_proc;
int *plist_cnt;
{
        dac_getproclist_ot in;
        procinfo_t *p_q;
        int sockdac;
        int i, j, k;
        u_short op;

        i = j = k = 0;
                                /* connect to DAC */
        if ((sockdac = r_connect_dac(host, dacport)) != -1) {
            op = htons(DAC_OP_GETPROCLIST);
            if (writen(sockdac, (char *)&op, sizeof(u_short)))
                if (readn(sockdac, (char *)&in, sizeof(dac_getproclist_ot))) {
		    in.plist_cnt = ntohs(in.plist_cnt);
                    (*plist_cnt) = in.plist_cnt;
                    if (in.plist_cnt > 0) { 
                        while ((*plist_cnt) > BUFFE_SIZE * j) {
                            j++;
                            for ( ; i < BUFFE_SIZE * j && i < (*plist_cnt); i ++) {
                                if ((p_q = (procinfo_t *) malloc(sizeof(procinfo_t)))
                                                    == NULL)
                                    exit(E_MALLOC);
                                k = i - BUFFE_SIZE * (j - 1);
                                strcpy(p_q->name, (in.buffer[k]).name);
                                strcpy(p_q->login, (in.buffer[k]).login);
                                strcpy(p_q->path, (in.buffer[k]).path);
                                p_q->host = (in.buffer[k]).host;
                                p_q->state = (in.buffer[k]).state;
				p_q->next = NULL;
				strcpy(p_q->elapsed_time, (in.buffer[k]).elapsed_time);
                                if (i == 0)
                                    (*list_proc) = (*end_proc) = p_q;
                                else {
                                    (*list_proc)->next = p_q;
                                    (*list_proc) = p_q;
                                }
                            }
                            if ((*plist_cnt) > BUFFE_SIZE * j)
                                if (!readn(sockdac, (char *)&in, sizeof(dac_getproclist_ot)))
                                    break;
                        }
                    } else {
                        printf("\n\n\t\t\t No process found.\n");
                        return 0;
                    }
                    close(sockdac);
                    return 1;
                }
        }
        printf("Lost communication with DAC?\n");
        return 0;
}



void free_proc_list(list_pp, end_pp, list_cntp)
procinfo_t **list_pp;
procinfo_t **end_pp;
int *list_cntp;
{
        procinfo_t *p_q1, *p_q2;

        p_q1 = p_q2 = (*end_pp);
        while (p_q1 != NULL) {
                p_q1 = p_q2->next;
                free(p_q2);
                p_q2 = p_q1;
        }
        (*list_pp) = (*end_pp) = NULL;
        (*list_cntp) = 0;
}


/*---------------------------------------------------------------------------
  Prototype   : int gethandlerlist(u_short dacport, handlerinfo_t **list_handler, 
					**end_handler int *hlist_cnt)
  Parameters  : -
  Returns     :
  Called by   :
  Calls       :
  Notes       :
  Date        : October' 94
  Coded by    : Feijian Sun
  Modification: 
---------------------------------------------------------------------------*/

int gethandlerlist(dacport, list_handler, end_handler, hlist_cnt)
u_short dacport;
handlerinfo_t **list_handler, **end_handler;
int *hlist_cnt;
{
        dac_gethandlerlist_ot in;
        handlerinfo_t *p_q;
        int sockdac;
        int i, j, k;
        u_short op;

        i = j = k = 0;
                                // connect to DAC 
        if ((sockdac = connect_dac(dacport)) != -1) {
            op = htons(DAC_OP_GETHANDLERLIST);
            if (writen(sockdac, (char *)&op, sizeof(u_short)))
                if (readn(sockdac, (char *)&in, sizeof(dac_gethandlerlist_ot))) {
                    in.hlist_cnt = ntohs(in.hlist_cnt);
                    (*hlist_cnt) = in.hlist_cnt;
                    if (in.hlist_cnt > 0) {
                        while ((*hlist_cnt) > BUFFE_SIZE * j) {
                            j++;
                            for ( ; i < BUFFE_SIZE * j && i < (*hlist_cnt); i ++) {
                                p_q = (handlerinfo_t *) malloc(sizeof(handlerinfo_t));
				if (p_q == NULL)
				{
				    perror("DACLIB. gethandlerlist. Cannot malloc");
                                    exit(E_MALLOC);
				}
                                k = i - BUFFE_SIZE * (j - 1);
                                strcpy(p_q->name, (in.buffer[k]).name);
                                strcpy(p_q->login, (in.buffer[k]).login);
                                strcpy(p_q->mapid, (in.buffer[k]).mapid);
                                strcpy(p_q->inet_addr, (in.buffer[k]).inet_addr);
                                p_q->host = (in.buffer[k]).host;
                                p_q->type = (in.buffer[k]).type;
                                p_q->port = (in.buffer[k]).port;
                                p_q->next = NULL;
                                if (i == 0)
                                    (*list_handler) = (*end_handler) = p_q;
                                else {
                                    (*list_handler)->next = p_q;
                                    (*list_handler) = p_q;
                                }
                            }
                            if ((*hlist_cnt) > BUFFE_SIZE * j)
                                if (!readn(sockdac, (char *)&in, sizeof(dac_gethandlerlist_ot)))
                                    break;
                        }
                    } else {
                        printf("\n\n\t\t\t No handler found.\n");
                        return 0;
                    }
                    close(sockdac);
                    return 1;
                }
        }
        printf("Lost communication with DAC.\n");
        return 0;
}



void free_handler_list(list_hh, end_hh, list_cnth)
handlerinfo_t **list_hh;
handlerinfo_t **end_hh;
int *list_cnth;
{
        handlerinfo_t *p_q1, *p_q2;

        p_q1 = p_q2 = (*end_hh);
        while (p_q1 != NULL) {
                p_q1 = p_q2->next;
                free(p_q2);
                p_q2 = p_q1;
        }
        (*list_hh) = (*end_hh) = NULL;
        (*list_cnth) = 0;
}


/*---------------------------------------------------------------------------
  Prototype   : int start_fdd(u_short dacport, int debug, int *fid)
  Parameters  : dacport  - location to store the port # of DAC
  Returns     : 0 - error in DAC exec
                1 - FDD started
  Calls       : fork, setpgrp, exit, sprintf
  Notes       :
  Date        :
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

int start_fdd(dacport, debug, fid)
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
	printf("SNG_PATH undefined. Exiting ...\n");
	exit(0);
   }
   sprintf(FDD_PATH,"%s/bin/fdd",getenv("SNG_PATH"));
   sprintf(buf_debug,"%d",debug);

   noexec = 0 ;
   if (!(pid = fork()))         /* exec FDD */
    {
#if IRIX
       BSD_setpgrp(0, getpid()) ; 
#elseif Linux || AIX
       setpgrp();
#endif

/*
#else
       setpgrp(0, getpid()) ; 
#endif
*/
       execl(FDD_PATH, "fdd", buf_dacport, buf_debug, (char*)0) ;
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
  Prototype   : int gethostlist(u_short dacport, host_t **list_host,
                                        host_t **end_host, int *hlist_cnt)
  Parameters  : -
  Returns     :
  Called by   : FDD
  Calls       :
  Notes       :
  Date        : October' 94
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

int gethostlist(dacport, list_host, end_host, hlist_cnt)
u_short dacport;
host_t **list_host, **end_host;
int *hlist_cnt;
{
        dac_getcpulist_ot in;
        host_t *p_q;
        int sockdac;
        int i, j, k;
        u_short this_op;

        i = j = k = 0;
                                /* connect to DAC */
        if ((sockdac = connect_dac(dacport)) == -1)
            return -1;
        this_op = htons(DAC_OP_GET_CPULIST);
        if (writen(sockdac, (char *)&this_op, sizeof(u_short)))
            if (readn(sockdac, (char *)&in, sizeof(dac_getcpulist_ot))) {
                    (*hlist_cnt) = in.hlist_cnt;
	            if ((*hlist_cnt) > 0) {
                        while ((*hlist_cnt) > BUFFE_SIZE * j) {
                            j++;
                            for ( ; i < BUFFE_SIZE * j && i < (*hlist_cnt); i ++) {
                                if ((p_q = (host_t *) malloc(sizeof(host_t)
))
                                                    == NULL)
                                    exit(E_MALLOC);
                                k = i - BUFFE_SIZE * (j - 1);
				strcpy(p_q->app_id, in.buffer[k].app_id);
// printf("DACLIB. gethostlist. appid(%s)\n", p_q->app_id);
                                p_q->hostid = in.buffer[k].hostid;
                                p_q->proc_id = ntohl(in.buffer[k].proc_id);
                                strcpy(p_q->login, in.buffer[k].login);
                                p_q->next = NULL;
                                if (i == 0)
                                    (*list_host) = (*end_host) = p_q;
                                else {
                                    (*end_host)->next = p_q;
                                    (*end_host) = p_q;
                                }
                            }
                            if ((*hlist_cnt) > BUFFE_SIZE * j)
                                if (!readn(sockdac, (char *)&in, sizeof(dac_getcpulist_ot)))
                                    break;
                        }
                    } else {
                        printf("\n\n\t\t\t No host found.\n");
                        close(sockdac);
                        return 0;
                    }
                    close(sockdac);
                    return 1;
            }
/*
        printf("Lost communication with DAC.\n");
*/
        close(sockdac);
        return 0;
}



void free_host_list(list_hh, end_hh, list_cnth)
host_t **list_hh;
host_t **end_hh;
int *list_cnth;
{
        host_t *p_q1, *p_q2;

        p_q1 = p_q2 = (*end_hh);
        while (p_q1 != NULL) {
                p_q1 = p_q2->next;
                free(p_q2);
                p_q2 = p_q1;
        }
        (*list_hh) = (*end_hh) = NULL;
        (*list_cnth) = 0;
}


/*---------------------------------------------------------------------------
  Prototype   : int fddchangeproc(u_short dacport, host_t *hostp)
  Parameters  : -
  Returns     :
  Called by   : FDD
  Calls       :
  Notes       :
  Date        : October' 94
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

int fddchangeproc(dacport, hostp)
u_short dacport;
host_t *hostp;
{
        dac_fddchangeproc_it out;
        u_short this_op;
        int sockdac;
                                /* connect to DAC */
        if ((sockdac = connect_dac(dacport)) == -1)
            return -1;
        this_op = htons(DAC_OP_FDDCHANGE_PROC);
        if (writen(sockdac, (char *)&this_op, sizeof(u_short))) {
                out.hostid = hostp->hostid;
        	out.proc_id = htonl(hostp->proc_id);
        	strcpy(out.login, hostp->login);
                out.cidport = hostp->cidport;
                if (writen(sockdac, (char *)&out, sizeof(dac_fddchangeproc_it))) {
                        close(sockdac);
                        return 1;
                }
        }
        close(sockdac);
        return 0;
}


/*---------------------------------------------------------------------------
  Prototype   : int fddretrievetuple(handlerinfo_t *handler_list, host_t *deadhost)
  Parameters  : -
  Returns     :
  Called by   : FDD
  Calls       :
  Notes       :
  Date        : October' 94
  Coded by    : Feijian Sun
  Modification:
- --------------------------------------------------------------------------*/

int fddretrievetuple(handler_list, deadhost)
handlerinfo_t *handler_list;
host_t *deadhost;
{
        tsh_retrieve_it out;
	handlerinfo_t *p_q;
        int tsh_sock, tshop;
        u_short tsh_port;

        out.host = deadhost->hostid;
        tshop = htons(TSH_OP_RETRIEVE);
        p_q = handler_list;
        while (p_q != NULL) {
            if (p_q->type == 't') {
                if ((tsh_port = sngd_getmap(p_q->mapid, p_q->host , p_q->login,
                                PMD_PROT_TCP)) == 0) {
                        perror("DACLIB_fddretrievetuple::sngd_getmap\n");
                        return(0);
                }
                if ((tsh_sock = get_socket()) == -1) {
                        perror("DACLIB_fddretrievetuple::get_socket\n") ;
                        return(0);
                }
                if (!do_connect(tsh_sock, p_q->host, tsh_port)) {
                        perror("DACLIB_fddretrievetuple::do_connect\n");
                        return(0);
                }
                if (!writen(tsh_sock, (char *)&tshop, sizeof(u_short))) {
			close(tsh_sock);
			return(0);
		}
                if(!writen(tsh_sock, (char *)&out, sizeof(tsh_retrieve_it))) {
                        close(tsh_sock);
                        return(0);
                }
                close(tsh_sock);
            }
            p_q = p_q->next;
        }
	return(1);
}

