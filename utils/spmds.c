/* spmds -- Setup PMDs on all hosts defined in ~./sng_hosts. 

   Author: Yuan Shi
   Feburay 1994
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#define MAX_PORTLEN 8
#define RSH_COMMAND "/usr/ucb/rsh"

main ()
{
	FILE *infile;
	char *buf;
	char ipaddr[32], login[32], host[128], home[128], filenm[128];
	char local_host[128], hostname[128];
	int  status, i, j, k;
  	char cmd[256];

	gethostname(hostname,sizeof(hostname));
	sprintf(home,"%s", getenv("HOME"));
	sprintf(filenm,"%s/.sng_hosts",home);
	if ((infile = fopen(filenm, "r")) !=NULL)
 	{
		status = fscanf(infile, "%s %s %s %s %s",
			ipaddr, host, home, home, login);
  		while (status > 0)
		{
			if (!strcmp(hostname,host)) 
				start_local("pmd");
			else {
				start_remote(host,login,"pmd");
			} 
			status = fscanf(infile, "%s %s %s %s %s",
				ipaddr, host, home,home, login );
		}
	}
}

/* start a local pmd */
int start_local(cmd)
char *cmd;
{
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
}

/* Start a remote pmd */
int start_remote ( host, login, cmd)
char *host; 
char *login;
char *cmd; 
{
  int
	  rfd [2],	/* rsh Read fd */
	  wfd [2], 	/* rsh write fds */
	  efd [2] ;	/* rsh Error fds */

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
	  execvp (argv[0], argv);
	  _exit(1); 
	}
	printf("Fork failed \n");
	exit(1); 
}
