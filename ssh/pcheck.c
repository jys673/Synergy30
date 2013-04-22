/*.........................................................................*/
/*                  PCHECK.C ------> Synergy Parallel Run Query program    */
/*.........................................................................*/

#include "pcheck.h"

typedef struct sngfile_struct {
	char ipaddr[NAME_LEN];
	char name[NAME_LEN];
	char protocol[NAME_LEN];
	char os[NAME_LEN];
	char login[NAME_LEN];
	char status[10]; 
	char buffline[PATH_LEN];
	struct sngfile_struct *next;
} hosts_it;
hosts_it *list_p, *end_p;
char hostname[NAME_LEN];
char hostip[NAME_LEN];
char hostlogin[NAME_LEN];

char *get_hostname();
void read_hosts();
int status2;

/*---------------------------------------------------------------------------
  Prototype   : int main()
  Parameters  : - 
  Returns     : -
  Called by   : user 
  Calls       : exit, printf, get_socket, pmd_getmap,  malloc, free, 
		writen, readn
  Notes       : Parallel checking of a application running. If it exit,
		all process exit_information of it will be reported. 
  Date        : July' 1994 
  Coded by    : Feijian Sun
  Modification:
               February '13, updated by Justin Y. Shi
---------------------------------------------------------------------------*/  

int main(argc, argv)
int argc; 
char **argv;
{
	appmsg_t *p_q;
	int app_choice;
	int app_num, i;
	hosts_it *hostsng;
	char tmphost[128];

	if (argc > 2) {
		printf("Usage : %s [<host name>]\n", argv[0]);
		exit(1);
	}
	read_hosts();
	gethostname(hostname, sizeof(hostname));
/* 
	if (!ping_cid(hostname,getpwuid(getuid())->pw_name))
	{
		printf("SNGD not ready. Auto-starting ...\n");
		status2=system("cid &");
		sleep(3);
	}
*/
	/* Search through all hosts. Yuan Shi 11-16-96 */
   	hostsng = end_p;
	while (hostsng != NULL)
	{
	     strcpy(hostname, hostsng->ipaddr);
	     strcpy(hostip, hostsng->name);
	     strcpy(hostlogin, hostsng->login);
	     hostsng = hostsng->next;
	     while (TRUE) {
	    	free_app_list(&list_app, &end_app, &alist_cnt);
/* fix YS94 */
	    	alist_cnt = 0;
	    	list_app = end_app = NULL;
/* fix YS94 */
	    	if (!getapplist(hostname, hostlogin, 
				&list_app, &end_app, &alist_cnt)) 
		{
/*
        		printf("\n\t\t\t----------------------------\n");
			printf("\n\n\t\tPress Enter to Continue.");
			getchar();
*/
			break;
		}
	    	if (alist_cnt != 0) {
		p_q = end_app;
		if (argc == 2) {
		    while (p_q != NULL) { 
			if (!strcmp(p_q->app_id, argv[1]))
			    break;
			p_q = p_q->next;
		    }
		} else {
		    app_choice = display_app(hostip, &app_num);
	    	    if (app_choice == 'Q' || app_choice == 'q')
			break;
		    else if (app_choice == 'U' || app_choice == 'u')
			continue;
/*
		    if (app_choice == 'D' || app_choice == 'd') {
        		printf("\n\tEnter application number: ");
        		scanf("%d", &app_num);
                        for (i = 0; i < app_num - 1 && p_q != NULL; i++)
                            p_q = p_q->next;
        		if (p_q != NULL)
			    if (!delapp(hostname, hostlogin, p_q->app_id)) {
				printf("Error : PCHECK delapp\n");
				exit(1);
			}
			continue;
		    }
*/ 
		    else if (app_choice == 'R' || app_choice == 'r') {
			if (!delapplist(hostname, getpwuid(getuid())->pw_name)) {
				printf("Error : PCHECK delapplist\n");
				exit(1);
			}
			continue;
		    } 
		    else if (app_choice == 'K' || app_choice == 'k') {
			killapp();
			continue;
		    }
		    else if (app_choice == 'A' || app_choice == 'a') {
			killallapp();
			continue;
		    }
		    else if (app_choice == 'P' || app_choice == 'p')
	    	    	for (i = 0; i < app_num - 1 && p_q != NULL; i++)
		       	    p_q = p_q->next;
		    else {
			printf("\n\n\t\tIncorrect Choice. Press enter to continue.");
			getchar();
			continue;
		    }

		}
		if (p_q != NULL) { 
		    if (p_q->app_status != APP_EXITTED && 
					p_q->app_status != APP_KILLED) {
		        dacport = p_q->dacport;
		        check_proc();
			continue;
		    } else {
	    	        printf("\n\t\t Application (%s) exitted!\n", 
				p_q->app_id);
	    	        getchar(); getchar(); 
			continue;
		    }
		    if (argc == 2) break;
			continue;
		} else {
		    printf("Cannot find Such application.\n");
		    argc = 1;
		    continue;
		}
	    } else {
/*
	        status2=system("clear");
        	printf("\n\t\t\tPARALLEL APPLICATION MONITOR");
        	printf("\n\t\t\t----------------------------\n");
		printf("\n\n\n\t\tNo application lives on (%s).",hostname);
		printf("\n\n\n\t\t   Press Enter to continue.");
		getchar();
*/
		break;
	    }
	} 
     }  /* of while true */
	status2=system("clear");
        printf("\n\t\t\tPARALLEL APPLICATION MONITOR");
        printf("\n\t\t\t----------------------------\n");
	printf("\n\n\n\t\t\t   End of Host List.");
	printf("\n\n\n\t\t\t Press Enter to exit.");
	getchar();
} 


/*---------------------------------------------------------------------------
  Prototype   : int display_app(int *app_num)
  Parameters  : -
  Returns     : Choice number from the application checking menu.
  Called by   : main 
  Calls       : system, printf, scanf
  Notes       : Return the choice number selected by user.
  Date        : July' 1994 
  Coded by    : Feijian Sun 
  Modification: 
---------------------------------------------------------------------------*/


int display_app(host, app_num)
char *host;
int *app_num;
{
	appmsg_t *p_q;
	int appn, i, k1, k2;
	int choice;
	char status[15];

	i = 1;
        choice = 'U'; 
	p_q = end_app;

        status2=system("clear");
        printf("\n\t\t\tPARALLEL APPLICATION MONITOR");
	printf("\n\t\t\t  (%s)",host);
        printf("\n\t\t\t----------------------------\n");
        while (p_q != NULL) {
		switch (p_q->app_status)
		{
			case APP_READY : strcpy(status,"Ready");
					break;
			case APP_RUNNING: strcpy(status,"Running");
					break;
			case APP_EXITTED: strcpy(status,"Exitted");
					break;
			case APP_KILLED:  strcpy(status,"Killed");
					break;
			case APP_NOT_READY: strcpy(status,"Not ready");
					break;
			case APP_PROC_NOT_READY: 
					strcpy(status,"Proc not ready");
					break;
			case APP_OBJ_NOT_READY: 
					strcpy(status,"Obj not ready");
		}
                printf("\t(%d)[%s] Port:[%u] Status:[%s] P:[%d]\n", 
			i, p_q->app_id, p_q->dacport, status, p_q->num_process);
		if (choice != 'N' && choice != 'n')
		{ k1 = strlen(p_q->start_time) - 1; if (k1 < 0) k1 = 0; }
		p_q->start_time[k1] = 0x0;
		if (choice != 'N' && choice != 'n')
		{ k2 = strlen(p_q->complete_time) -1; if (k2 < 0) k2 = 0; }
		p_q->complete_time[k2] = 0x0;
		printf("\t\tStarted:[%s]  Elapsed:[%s]\n", 
				p_q->start_time, p_q->complete_time);
        	p_q = p_q->next;
		i++;
		if (p_q == NULL || ((i - 1) % 7) == 0) {
		    printf("\n\n\t ------------------------------------------------------------");
		    printf("\n\t [P]rocess Monitor | [U]pdate status    | [R]emove inactive"); 
		    if (p_q != NULL) 
		    printf("\n\t [N]ext page       | [K]ill application | kill [A]pplication");
		    else 
		    printf("\n\t [N]ext front page | [K]ill application | kill [A]ll");
		    printf("\n\t ------------------------------------------------------------");
		    printf("\n\t [Q]uit");

	    	    printf("\n\n\t\tEnter Choice : ");
		    choice = getchar(); getchar(); 
	  	    if (choice != 'P' && choice != 'p' && choice != 'N' && 
			choice != 'n' && choice != 'U' && choice != 'u' && 
			choice != 'R' && choice != 'r' && choice != 'K' && 
			choice != 'k' && choice != 'A' && choice != 'a' &&
			choice != 'Q' && choice != 'q')
			return 0;
		    if (choice == 'P' || choice == 'p') {
			printf("\n\t\t\tApplication Number : ");
			status2=scanf("%d", &appn);
			if (appn < 1 || appn > i)
				return 0;
			else
				*app_num = appn;
		    }
		    if (choice == 'N' || choice == 'n') {
			if (p_q == NULL) {
				i = 1;
				p_q = end_app;
			}
			status2=system("clear");
			printf("\n\t\t\tPARALLEL APPLICATION MONITOR");
			printf("\n\t\t\t----------------------------\n"); 
		    } else
			break;
		}
        }
	return(choice);
}



/*---------------------------------------------------------------------------
  Prototype   : void display_proc(void)
  Parameters  : -
  Returns     : Choice number from the process checking menu.
  Called by   : main 
  Calls       : system, printf, scanf
  Notes       : Return the choice number selected by user.
  Date        : July' 1994 
  Coded by    : Feijian Sun 
  Modification: 
---------------------------------------------------------------------------*/

void display_proc()
{
	procinfo_t *p_q;
	int i;
	char status[10], hostname[NAME_LEN];
	struct in_addr host_addr;
			
        i = 1;
        p_q = end_proc;
	status2=system("clear");
	printf("\n\t\t\tPARALLEL PROCESS MONITOR");
	printf("\n\t\t\t------------------------\n");
        while (p_q != NULL) {
		host_addr.s_addr = p_q->host;
		strcpy(hostname,get_hostname(inet_ntoa(host_addr)));
		switch (p_q->state)
		{
			case WAITING: 	strcpy(status,"Waiting");
					break;
			case READY:	strcpy(status,"Ready");
					break;
                        case RUNNING:   strcpy(status,"Running");
                                        break;
			case STARTED:	strcpy(status,"Started");
					break;
			case EXITTED:	strcpy(status,"Exitted");
					break;
			case FAILED:	strcpy(status,"Failed");
					break;
			case KILLED:	strcpy(status,"Killed"); 
		}
                printf("  (%d)[%s]\tStatus:[%s]\tElapsed:[%s]\n",
			i, p_q->name, status, p_q->elapsed_time);
		printf("\t (%s,%s)::[%s]\n",hostname,p_q->login,p_q->path);
                if ((p_q->next != NULL) && ((i % 7) == 0)) {
			sleep(2);
		        status2=system("clear");
        		printf("\n\t\t\tPARALLEL PROCESS MONITOR");
        		printf("\n\t\t\t------------------------\n");
                }
                p_q = p_q->next;
                i++;
        }
}


/*---------------------------------------------------------------------------
  Prototype   : void check_proc(void)
  Parameters  : -
  Returns     : - 
  Called by   : main
  Calls       : system, printf, scanf
  Notes       : Return the choice number selected by user.
  Date        : July' 1994
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void check_proc ()
{
	int choice, sd;
		
	while (TRUE) {
/*
	        if (signal(SIGINT, killfresh) == (void *)-1) {
        	        printf("Error : check_proc::signal\n");
                	exit(1);
        	}
*/
		if ((frshid = fork()) == 0) {
			freshproc();
			exit(0);
		}
		if (frshid < 0) {
			printf("Error : frshid::fork\n");
			return;
		}
	    	getchar(); getchar();
	    	kill(frshid, SIGTERM);
		waitpid(frshid, 0, 0);

		if ((sd = r_connect_dac(hostname, dacport)) != -1) {
			close (sd);
			printf("\n\t\t      PROCESS CONTROL OPTIONS");
			printf("\n\t\t-----------------------------------");
			printf("\n\t\t[C]heck status   |   auto [R]efresh");
			printf("\n\t\t[K]ill process   |   kill [A]ll");
			printf("\n\t\t[B]ack to Application Monitor"); 
			printf("\n\n\t\tEnter Choice : ");
			choice = getchar(); getchar();
			if (choice != 'C' && choice != 'c' &&
				choice != 'R' && choice != 'r' &&
				choice != 'K' && choice != 'k' &&
				choice != 'A' && choice != 'a' &&
				choice != 'B' && choice != 'b') 
			{
                        	printf("\n\t\tThe choice is not correct. Press enter to continue ...\n");
                        	getchar();
                        	break;
                    	}
			if (choice == 'C' || choice == 'c') {
				if (!hand_proc())
					break;
			}
			else if (choice == 'B' || choice == 'b')
				break;
			else if (choice == 'K' || choice == 'k')
				OpKill();
			else if (choice == 'A' || choice == 'a') {
				OpKillAll();
				printf("\n\t\tPress enter to Application Monitor ...\n");
				getchar(); 
				break;
			}
			status2=system("clear");
		} else
			break; 
	}
}

/*
void killfresh()
{
	if (frshid == 0)
		kill(getpid(), SIGKILL);
	else
		kill(frshid, SIGKILL);
}
*/

void freshproc()
{
	while (TRUE) {
		plist_cnt = 0;
		list_proc = end_proc = NULL;
	 	free_proc_list(&list_proc, &end_proc, &plist_cnt);
		if (getproclist(hostname, dacport, &list_proc, &end_proc, &plist_cnt)) {
			display_proc();
			printf("\n\tPress enter to stop auto-updating ...\n");
			sleep(5);
		} else {

/*			printf("\n\n\t\t Application completed. Press enter ...\n");
			getchar(); getchar();
*/
                        status2=system("clear");
                        printf("\n\t\t\tPARALLEL PROCESS MONITOR");
                        printf("\n\t\t\t------------------------\n");
                        printf("\n\n\n\n\t\t\tApplication completed. Press enter ...\n");
                        getchar();
			exit(0);
		}
	}
}


/*---------------------------------------------------------------------------
  Prototype   : int hand_proc()
  Parameters  : -
  Returns     : Choice number from the application checking menu.
  Called by   : main
  Calls       : system, printf, scanf
  Notes       : Return the choice number selected by user.
  Date        : July' 1994
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/


int hand_proc()
{
        procinfo_t *p_q;
        int i, choice;
        char status[10];
	struct in_addr host_addr;

        status2=system("clear");
        printf("\n\t\t\tPARALLEL PROCESS MONITOR");
        printf("\n\t\t\t------------------------\n");
	plist_cnt = 0;
	list_proc = end_proc = NULL;
	free_proc_list(&list_proc, &end_proc, &plist_cnt);
        if (getproclist(hostname, dacport, &list_proc, &end_proc, &plist_cnt)) {
	    i =1;
            p_q = end_proc;
            while (p_q != NULL) {
                switch (p_q->state)
                {
                        case WAITING:   strcpy(status,"Waiting");
                                        break;
                        case READY:     strcpy(status,"Ready");
                                        break;
                        case RUNNING:   strcpy(status,"Running");
                                        break;
                        case STARTED:   strcpy(status,"Started");
                                        break;
                        case EXITTED:   strcpy(status,"Exitted");
                                        break;
                        case FAILED:    strcpy(status,"Failed");
                                        break;
                        case KILLED:    strcpy(status,"Killed");
                }
                printf("\t(%d)[%s]\tStatus:[%s]\tElapsed:[%s]\n",
			i, p_q->name, status, p_q->elapsed_time);
		host_addr.s_addr = p_q->host;
		strcpy(hostname,get_hostname(inet_ntoa(host_addr)));
                printf("\t\t (%s,%s)::[%s]\n",hostname,
					p_q->login,p_q->path);
                p_q = p_q->next;
                i++;

                if (p_q == NULL || (i - 1) % 7 == 0) {
                        printf("\n\n\t -----------------------------------------------------------\n");
                        printf("\n\t auto [R]efresh      |   [K]ill process   |    kill [A]ll");
                        if (p_q != NULL)
                        printf("\n\t [N]ext page         |   [B]ack to Application Monitor ");
                        else
                        printf("\n\t [N]ext front page   |   [B]ack to Application Monitor ");

                        printf("\n\n\t\tEnter Choice : ");
                        choice = getchar(); getchar();
                        if (choice != 'N' && choice != 'n' &&
                                choice != 'R' && choice != 'r' &&
                                choice != 'K' && choice != 'k' &&
                                choice != 'A' && choice != 'a' &&
                                choice != 'B' && choice != 'b')
                        {
                                printf("\n\t\tThe choice is not correct. Press enter to continue ...\n");
                                getchar();
                                continue;
                        }
			if (choice == 'R' || choice == 'r')
				return(1);
                        if (choice == 'B' || choice == 'b')
                                return(0);
                        if (choice == 'K' || choice == 'k') {
                                OpKill();
				return(1);
			}
                        if (choice == 'A' || choice == 'a') {
                                OpKillAll();
                                printf("\n\n\t\tPress enter to Application Monitor ...\n");
                                getchar();
				return(0);
                        }
			if (choice == 'N' || choice == 'n') {
                                if (p_q == NULL) {
                                        i = 1;
                                        p_q = end_proc;
                                }
                                status2=system("clear");
                                printf("\n\t\t\tPARALLEL PROCESS MONITOR");
                                printf("\n\t\t\t------------------------\n");
                        }
		}
	    }
	}
        status2=system("clear");
        printf("\n\t\t\tPARALLEL PROCESS MONITOR");
        printf("\n\t\t\t------------------------\n");
        printf("\n\n\n\n\t\t\tApplication completed. Press enter ...\n");
        getchar();
}



/*---------------------------------------------------------------------------
  Prototype   : void OpKill(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : kill_process, printf, exit, getchar
  Notes       : The process is killed.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpKill()
{
   char name[NAME_LEN];
   int stat ;
				/* obtain process name */
   printf("\n\tEnter process name: ") ;
   status2=scanf("%s", name) ;
				/* kill process */
   if ((stat = kill_process(hostname, dacport, name)) == -1)
    {
        printf("\t Cannot find [%s]. Application not present.\n",name);
        getchar(); getchar();
        return;
    }
				/* process errors */
   if (stat)
      printf("\t [%s] : Process killed.\n",name) ;
   else
      if (dacerrno == DAC_ER_NOPROCESS)
	 printf("Error : No such process.\n") ;
      else
	 printf("Error : Unknown error.\n") ;
   getchar(); getchar();
}


/*---------------------------------------------------------------------------
  Prototype   : void OpKillAll()
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : exit_app, printf, exit, getchar
  Notes       : All processes/handlers for the application are killed.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpKillAll()
{
   int stat ;
   printf("\t Killing all processes. Please wait ...\n");
				/* kill application */
   if ((stat = exit_app(hostname, dacport)) == -1)
    {
        printf("\t Application not present.\n");
        getchar(); getchar();
        return;
    }
				/* process errors */
   if (stat)
      printf("\t All processes killed.\n") ;
   else
      printf("Error : Unknown error.\n") ;
}


/*---------------------------------------------------------------------------
  Prototype   : void killapp(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : exit_dac, printf, exit, getchar
  Notes       : DAC is made to exit.
  Date        : July' 1994 
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/  

void killapp()
{
	appmsg_t *p_q;
	int i, stat, app_num ;
        cid_changeappmsg_it out;

	printf("\n\tEnter application number: ");
	status2=scanf("%d", &app_num);
	p_q = end_app;
        for (i = 0; i < app_num - 1 && p_q != NULL; i++)
                p_q = p_q->next;
	if (p_q == NULL) {
		printf("\t No application present.\n");
		getchar(); getchar();
		return;
	}
	if (p_q->app_status == APP_EXITTED || p_q->app_status == APP_KILLED) {
		printf("Application (%s) exitted!\n", p_q->app_id);
		getchar(); getchar();
		return;
	}
				/* exit DAC */
	if ((stat = exit_app(hostname, p_q->dacport)) != 1) {
		printf("Found a dead application controllerc. Cleanup CID. \n");
		strcpy(out.app_id, p_q->app_id);
		out.app_status = APP_KILLED;
		out.elapsed_time = 0; 
		gethostname(hostname, sizeof(hostname));
		if (!(changeappmsg(hostname, getpwuid(getuid())->pw_name,out)))
			printf("Send app KILLED status failure.\n");
		getchar(); getchar();
		return;
	}
	printf("\t [%s] : Application killed.\n", p_q->app_id);
}


/*---------------------------------------------------------------------------
  Prototype   : void killallapp(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : exit_dac, printf, exit, getchar
  Notes       : DAC is made to exit.
  Date        : July' 1994
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void killallapp()
{
        appmsg_t *p_q;
	int stat;

        p_q = end_app;
        if (p_q == NULL) {
                printf("\t No application present.\n");
                getchar(); getchar();
                return;
        }
	printf("\t Killing all applications. Please wait ...\n");
	while (p_q != NULL) {
		if (p_q->app_status != APP_EXITTED && p_q->app_status != APP_KILLED)
		        if ((stat = exit_app(hostname, p_q->dacport)) != 1) {
                		printf("Error : PCHECK killallapp_exit_app. App_name:(%s)\n",
							p_q->app_id);
		                getchar(); getchar();
				return;
			}
		p_q = p_q->next;
	}
	printf("\t All applications killed\n");
}

void read_hosts()
{
	int host_idx;
	char sfname[NAME_LEN], buff[255];
	FILE *sngfp;
	hosts_it *hostsng;

	hostsng = list_p = end_p = NULL;
	host_idx = 0;

        sprintf(sfname,"%s/.sng_hosts", getenv("HOME")); 
        if ((sngfp = fopen(sfname,"r")) != NULL) {
       	    while (fgets(buff, 225, sngfp)) {
		if (buff[0] == '#') continue;
                if ((hostsng = (hosts_it *)malloc(sizeof(hosts_it))) == NULL) {
			printf("\n\nError : markhosts malloc\n");
               		exit(1);
		}
		sscanf(buff,"%s %s %s %s %s",hostsng->ipaddr,
			hostsng->name, hostsng->protocol,
			hostsng->os, hostsng->login);
		strcpy(hostsng->status,"-----");
		hostsng->next = NULL;
	        host_idx ++;
	        if (host_idx == 1)
			list_p = end_p = hostsng;
	        else {
			list_p->next = hostsng;
			list_p = list_p->next;
	        }
	    }
	    fclose(sngfp);
	} else {
	    printf("\n\n\t === Synergy host file (%s) not present.", sfname);
	    exit (1);
	}
}

char *get_hostname(ipaddr)
char *ipaddr;
{
	hosts_it *hostsng;

	hostsng = end_p;
	while (hostsng != NULL)
	{
		if (!strcmp(ipaddr,hostsng->ipaddr)) return hostsng->name;
		hostsng = hostsng->next;
	}
	printf(" Cannot find ipaddr (%s)?? \n",ipaddr);
	exit (1);
}

