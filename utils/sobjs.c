/* sobjs -- Setup object daemons on all hosts defined in argv[1]. 

   Author: Yuan Shi
   February 1994
*/
#include "synergy.h" 

#define RSH_COMMAND "/usr/ucb/rsh"

main (argc, argv)
int argc;
char **argv;
{
	FILE *infile;
	char ipaddr[32], login[32], host[128], filenm[128];
	char cmd[4], appid[128], objnm[32];
	char local_host[128], hostname[128];
	char buf[256];
	int  status;

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
			if (!strcmp(hostname,host)) 
			{
				start_local(cmd,appid,objnm,ipaddr);
			} 
			else {
				start_remote(host,cmd,appid,objnm,login,ipaddr);
			} 
			status = fscanf(infile, "%s %s %s %s %s %s",
				ipaddr, host, cmd, appid, objnm, login );
		}
	}
}

/* start a local object daemon */
int start_local(cmd,appid,objnm, ipaddr)
char *cmd, *appid, *objnm, *ipaddr;
{
   	u_long pmd_host ;
   	struct hostent *host ;
   	int portn;
	char mapid[MAP_LEN];

   	if (isdigit(ipaddr[0]))
      	pmd_host = inet_addr(ipaddr) ;
   	else
    	{
       		if ((host = gethostbyname(ipaddr)) == NULL)
        	{
           	perror("connect pmd error\n") ;
	   	return;
        	}
       		pmd_host = *((long *)host->h_addr_list[0]) ;
    	}
    	sprintf(mapid,"%s$%s",appid,objnm);
   	if ((portn = pmd_getmap(mapid, pmd_host, (u_short)PMD_PROT_TCP))>0)
    	{
        	printf("Object (%s) already up\n",objnm);
		return;
    	}
	if (fork())
	{ 
	  return;
	}
	else { /* Child */
	  char	*argv [16] ;	/* To pass arguments */
	  int	argc = 0 ;
	  
	  argv [argc++] = cmd ;
	  argv [argc++] = "-a";
	  argv [argc++] = appid;
	  argv [argc++] = objnm;
	  argv [argc++] = (char *)0;
	  execvp (argv[0], argv);
	  _exit(1); 
	}
}

/* Start a remote object daemon */
int start_remote ( hostnm, cmd, appid, objnm, login, ipaddr)
char *hostnm; 
char *cmd, *appid, *objnm;
char *login, *ipaddr;
{
  	int
	  rfd [2],	/* rsh Read fd */
	  wfd [2], 	/* rsh write fds */
	  efd [2] ;	/* rsh Error fds */
   	u_long pmd_host ;
   	struct hostent *host ;
   	int portn;
	char mapid[MAP_LEN];

   	if (isdigit(ipaddr[0]))
      	pmd_host = inet_addr(ipaddr) ;
   	else
    	{
       		if ((host = gethostbyname(ipaddr)) == NULL)
        	{
           	perror("connect pmd error\n") ;
	   	return;
        	}
       		pmd_host = *((long *)host->h_addr_list[0]) ;
    	}
    	sprintf(mapid,"%s$%s",appid,objnm);
   	if ((portn = pmd_getmap(mapid, pmd_host, (u_short)PMD_PROT_TCP))>0)
    	{
        	printf("Object (%s) already up\n",objnm);
		return;
    	}

  if ( pipe (wfd) || pipe (efd) || pipe (rfd) )
	{
	  perror("Cannot open pipe\n");
	  return ( -3 ) ;
	}
	  
  /*
   *  Fork off a child to execute the daemon on the remote machine in a
   *  rsh. The parent waits for connection from the daemon
   */
  if ( fork () )	/* Parent */
  {
	  (void) close (rfd[0]);
	  (void) close (wfd[1]);
	  (void) close (efd[1]);
	return; 
  } 
  else		/* Child */
	{
	  char	*argv [16] ;	/* To pass rsh arguments */
	  int	argc = 0 ;
	  register int i ;
	  
	  (void) dup2 (rfd[0], 0);	/* Redirect stdin */
	  (void) dup2 (wfd[1], 1);	/* Redirect stdout */
	  (void) dup2 (efd[1], 2);	/* Redirect stderr */

	  for (i = 20; --i > 2; )	/* Close all else */
		  (void)close(i);
	  
	  argv [argc++] = RSH_COMMAND ;
	  argv [argc++] = hostnm ;
	  if ( login )
		{
		  argv [argc++] = "-l" ;
		  argv [argc++] = login ;
		}
	  argv [argc++] = "-n" ;
	  argv [argc++] = cmd ;
	  argv [argc++] = "-a" ;
	  argv [argc++] = appid ;
	  argv [argc++] = objnm ;
	  argv [argc++] = (char *)0;
	  execvp (argv[0], argv);
	  _exit(1); 
	}
	printf("Fork failed \n");
	_exit(1); 
}
