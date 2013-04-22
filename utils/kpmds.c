#include "synergy.h"

main ()
{
	FILE *infile;
	char *buf;
	char ipaddr[32], login[32], host[128], home[128], filenm[128];
	char local_host[128], hostname[128];
	int  status, sd;
	u_short op; 

	gethostname(hostname,sizeof(hostname));
	sprintf(home,"%s",getenv("HOME"));
	sprintf(filenm,"%s/.sng_hosts",home);
	if ((infile = fopen(filenm, "r")) !=NULL)
 	{
		status = fscanf(infile, "%s %s %s %s %s",
			ipaddr, host, home, home, login);
  		while (status > 0)
		{
			kill_pmd(ipaddr, login, host);
			status = fscanf(infile, "%s %s %s %s %s",
				ipaddr, host, home, home, login);
		}
	}
}

kill_pmd(ipaddr, login, hostnm)
char *ipaddr;
char *login;
char *hostnm;
{
   pmd_exit_ot in ;
   int sock ;
   u_long pmd_host ;
   struct hostent *host ;
   u_short this_op; 
                                /* determine address of PMD */
   if (isdigit(ipaddr[0]))
      pmd_host = inet_addr(ipaddr) ;
   else
    {
       if ((host = gethostbyname(ipaddr)) == NULL)
        {
           printf("connectPmd::gethostbyname\n") ;
           exit(1) ;
        }
       pmd_host = *((long *)host->h_addr_list[0]) ;
    }
                                /* get socket and connect to PMD */
   if ((sock = get_socket()) == -1)
    {
       printf("connectPmd::get_socket\n") ;
       exit(1) ;
    }
   if (!do_connect(sock, pmd_host, htons(PMD_PORT)))
    {
       printf("(%s) \t\tPMD down\n", hostnm) ;
       return ;
    }
   this_op = htons(PMD_OP_EXIT) ;
   if (!writen(sock, (char *)&this_op, sizeof(this_op)))
    {
       printf("main::writen\n") ;
       exit(1) ;
    }
/* 
   if (!readn(sock, (char *)&in, sizeof(in)))
    {
       printf("\nOpExit::readn\n") ;
       getchar() ;  getchar() ;
       return ;
    }
   printf("\n\nFrom PMD :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d\n", ntohs(in.error)) ;
   getchar() ;  getchar() ;
*/ 
   close(sock) ;
   printf("(%s) \t\tPMD removed\n",hostnm); 
}

