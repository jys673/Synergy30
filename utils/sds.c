/* sds -- Setup CIDs on all hosts defined in ~./sng_hosts. 

   Author: Yuan Shi
   Feburay 1994
*/
#include "synergy.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#define MAX_PORTLEN 8
#define RSH_COMMAND "/usr/bin/ssh"
#define MAX_LEN 128

main ()
{
	FILE *infile;
	char *buf;
	char ipaddr[MAX_LEN], login[MAX_LEN], host[MAX_LEN], 
		home[MAX_LEN], filenm[MAX_LEN];
	char last_host[MAX_LEN], hostname[MAX_LEN], fsys[MAX_LEN];
  	char cmd[256];

	gethostname(hostname,sizeof(hostname));
	sprintf(home,"%s", getenv("HOME"));
	sprintf(filenm,"%s/.sng_hosts",home);
	bzero(last_host, MAX_LEN);
	if ((infile = fopen(filenm, "r")) !=NULL)
 	{
		while( fscanf(infile, "%s %s %s %s %s %s",
			ipaddr, host, home, home, login, fsys) > 0 )
		{
			// printf(" found host (%s) login(%s)\n",ipaddr, login);
			if ((ipaddr[0] != '#') && strcmp(last_host,ipaddr))
			{
				if (strcmp(hostname,ipaddr)==0) // So we can see the STDIO in the log 
					system("~/synergy/bin/cid&");
				else 
				start_remote(host,login,"~/synergy/bin/cid &");
				strcpy(last_host, ipaddr);
			}
		}
		exit (0);
	} printf("Node file not found. Nothing to do...\n"); 
	exit(1);
}

/* start a local pmd */
int start_local(cmd)
char *cmd;
{
	printf("Starting a local cid.\n");
	system(cmd);
	return;
	if (fork())
	{ 
	  return;
	}
	else { /* Child */
	  char	*argv [16] ;	/* To pass rsh arguments */
	  int	argc = 0 ;
	  
	  argv [argc++] = cmd ;
	  argv [argc++] = (char *)0;
	  execvp (argv[0], argv);
	  _exit(1); 
	}
	printf(" Fork failed. Exiting ... \n");
	exit(0);
}

/* Start a remote cid */
int start_remote ( host, login, cmd)
char *host; 
char *login;
char *cmd; 
{
  int
	  rfd [2],	/* rsh Read fd */
	  wfd [2], 	/* rsh write fds */
	  efd [2] ;	/* rsh Error fds */

printf(" start a remote cid: (%s) login(%s)\n",host,login);

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
	  argv [argc++] = host ;
	  if ( login )
		{
		  argv [argc++] = "-l" ;
		  argv [argc++] = login ;
		}
	  argv [argc++] = "-n" ;
	  argv [argc++] = cmd ;
	  argv [argc++] = (char *)0;
printf("Execvp (%s)\n", argv[0]);
	  execvp (argv[0], argv);
	  _exit(1); 
	}
	printf("Fork failed \n");
	exit(1); 
}
