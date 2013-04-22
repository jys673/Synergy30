/* ipcvt -- convert the LL hostfile into ~./sng_hosts. 

   Author: Yuan Shi
   November 1996
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include "synergy.h"

#define MAX_PORTLEN 8
#define RSH_COMMAND "/usr/bin/rsh"

main ()
{
	FILE *infile;
	char *buf;
	char ipaddr[32], login[32], host[128], home[128], filenm[128];
	char local_host[128], hostname[128], fsys[5];
	int  status, i, j, k;
  	char cmd[256];
	struct ary_t {
		char ipname[128];
	} hosts[1000];


	gethostname(hostname,sizeof(hostname));
	/* Start remote pmd's */
	strcpy(cmd,"hostlist");
	status = read(0, cmd, 22 );
	while (status > 0)
	{
		strncpy(host, cmd, 21);
		printf(" host =(%s)\n", host);
		start_remote(host, "yshi", "cid &");
		status = read(0, cmd, 22);
	}	

/*
	sprintf(home,"%s", getenv("HOME"));
	sprintf(filenm,"%s/.sng_hosts",home);
	if ((infile = fopen(filenm, "r")) !=NULL)
 	{
		status = fscanf(infile, "%s %s %s %s %s %s",
			ipaddr, host, home, home, login, fsys);
  		while (status > 0) 
		{
			if (ipaddr[0] != '#')
			{
			if (!strcmp(hostname,host)) 
				start_local("cid &");
			else {
				start_remote(host,"yshi","cid &");
			} 
			}
			status = fscanf(infile, "%s %s %s %s %s %s",
				ipaddr, host, home,home, login, fsys );
		}
	}
*/
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
  static u_short execport = 0;
  struct servent *se;
  int i;

printf(" start a remote cid: (%s)\n",host);

  if (!(se = getservbyname("exec", "tcp"))) {
	perror(" cannot get execport??");
	return 0;
  }
  execport = se->s_port;
printf(" execport = (%d)\n", execport);
  endservent();
printf(" before rexec?? \n");
  if (!rexec(&host, execport, login, "s1nerg1700", cmd, (char *)0))
  {
	perror(" cannot rexec?? \n");
	return 0;
  }
  return 1;
}
