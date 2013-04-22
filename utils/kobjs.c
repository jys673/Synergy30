/* kobjs -- Remove object daemons on all hosts defined in argv[1]. 

   Author: Yuan Shi
   Feburay 1994
*/
#include "synergy.h"

main (argc,argv)
int argc;
char **argv;
{
	FILE *infile;
	char ipaddr[32], login[32], host[128], filenm[128];
	char cmd[4], appid[128], objnm[32];
	char local_host[128], hostname[128];
	char buf[256];
	int  status;
	u_short op; 

	if (argc < 2)
	{
		printf("Usage: sobjs <object_desc_file>\n");
		exit(1);
	}
	strcpy(filenm,argv[1]);
	gethostname(hostname,sizeof(hostname));
	if ((infile = fopen(filenm, "r")) !=NULL)
 	{
		status = fscanf(infile, "%s %s %s %s %s %s",
			ipaddr, host, cmd, appid, objnm, login);
  		while (status > 0)
		{
			sprintf(buf,"%s$%s",appid,objnm);
			kill_obj(ipaddr, buf, host, cmd, login);
			status = fscanf(infile, "%s %s %s %s %s %s",
				ipaddr, host, cmd, appid, objnm, login );
		}
	}
}


kill_obj(ipaddr, mapid, hostnm, type, login)
char *ipaddr;
char *mapid;
char *hostnm;
char *type;
char *login;
{
   tsh_exit_ot in ;
   int sock ;
   u_long cid_host ;
   struct hostent *host ;
   int cidport;
   u_short this_op; 

                                /* determine address of CID */
   if (isdigit(ipaddr[0]))
      cid_host = inet_addr(ipaddr) ;
   else
    {
       if ((host = gethostbyname(ipaddr)) == NULL)
        {
           perror("connect obj::gethostbyname\n") ;
	   return;
        }
       cid_host = *((long *)host->h_addr_list[0]) ;
    }
                                /* get socket and connect to CID */
   if ((sock = get_socket()) == -1)
    {
       perror("connect obj::get_socket\n") ;
       return ;
    }
   if (!(cidport = sngd_getmap(mapid, cid_host, login, (u_short)PMD_PROT_TCP)))
    {
        printf("PMD down \t(%s)\n",hostnm);
	return;
    }
   if (!do_connect(sock, cid_host, cidport))
    {
       printf("Object (%s) not alive on (%s)\n",mapid,hostnm) ;
       return ;
    }
   if (!strcmp(type,"tsh")) this_op = htons(TSH_OP_EXIT);
   else this_op = htons(FAH_OP_EXIT);

   if (!writen(sock, (char *)&this_op, sizeof(this_op)))
    {
       perror("Error in writing\n") ;
       exit(1) ;
    } 
/* 
   if (!readn(sock, (char *)&in, sizeof(in)))
    {
       printf("\nOpExit::readn\n") ;
       getchar() ;  getchar() ;
       return ;
    }
   printf("\n\nFrom CID :\n") ;
   printf("\n\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d\n", ntohs(in.error)) ;
   getchar() ;  getchar() ;
*/
   close (sock); 
   printf("Object (%s) removed on(%s)\n",mapid,hostnm); 
}
