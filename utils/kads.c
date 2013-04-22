/***************************************************************
	kads.c -- Kills cid and pmd on all hosts by $HOME/.sng_hosts.
	Author: Yuan Shi
	Date: November 1996
****************************************************************/
#include "synergy.h"

main (argc, argv)
int argc;
char **argv;
{
	FILE *infile;
	char *buf;
	char ipaddr[32], login[32], host[128], home[128], filenm[128];
	char local_host[128], hostname[128];
	int  status, sd;
	u_short op; 

	if (argc > 2)
	{
		strcpy(filenm,argv[2]);
	} else {
		gethostname(hostname,sizeof(hostname));
		sprintf(home,"%s",getenv("HOME"));
		sprintf(filenm,"%s/.sng_hosts",home);
	} 
	if ((infile = fopen(filenm, "r")) !=NULL)
 	{
		status = fscanf(infile, "%s %s %s %s %s %s",
			ipaddr, host, home, home, login, home);
  		while (status > 0)
		{
			if (ipaddr[0] != '#') kill_cid(ipaddr, login, host);
			status = fscanf(infile, "%s %s %s %s %s %s",
				ipaddr, host, home, home, login, home);
		}
	}
}


kill_cid(ipaddr, login, hostnm)
char *ipaddr;
char *login;
char *hostnm;
{
   cid_exit_ot in ;
   int sock ;
   u_long cid_host ;
   struct hostent *host ;
   int cidport;
   char mapid[MAP_LEN];
   u_short this_op; 

                                /* determine address of CID */
   if (isdigit(ipaddr[0]))
      cid_host = inet_addr(ipaddr) ;
   else
    {
       if ((host = gethostbyname(ipaddr)) == NULL)
        {
           printf("connectCid::gethostbyname\n") ;
	   return;
        }
       cid_host = *((long *)host->h_addr_list[0]) ;
    }
                                /* get socket and connect to CID */
   if ((sock = get_socket()) == -1)
    {
       printf("connectCid::get_socket\n") ;
       return ;
    }
   sprintf(mapid,"sng$cid$%s",login);
   if (!(cidport = pmd_getmap(mapid, cid_host, (u_short)PMD_PROT_TCP)))
    {
        printf("(%s) \t\tPMD down\n",hostnm);
	return;
    }
   if (!do_connect(sock, cid_host, cidport))
    {
       printf("(%s) \t\tCID down\n",hostnm) ;
       return ;
    }
   this_op = htons(CID_OP_EXIT);
   if (!writen(sock, (char *)&this_op, sizeof(this_op)))
    {
       printf("Error in writing CID\n") ;
       exit(1) ;
    } 
   close (sock); 
   if (!(sock = get_socket()))
   {
	perror(" cannot get socket? \n");
	exit(1);
   }
   if (!do_connect(sock, cid_host, PMD_PORT))
    {
       printf("(%s) \t\tPMD down\n",hostnm) ;
       return ;
    }
   this_op = htons(PMD_OP_EXIT);
   if (!writen(sock, (char *)&this_op, sizeof(this_op)))
    {
       printf("Error in writing PMD\n") ;
       exit(1) ;
    } 
   close (sock); 
   printf("(%s) \t\tAll daemons removed\n",hostnm); 
}
