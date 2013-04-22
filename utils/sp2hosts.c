/*********************************************************************************

  	sp2hosts.c -- Construct $HOME/.sng_hosts using LL's hostfile
		and /u/users/user0/yshi/hps.hosts.			 
	Author: Yuan Shi
	Date: 92

**********************************************************************************/
#include "synergy.h" 

struct hostet {
	char name[128];
	char ip[128];
	int  select;
};
struct hostet hps_hosts[512];
FILE *fd;

int main (argc, argv)
int argc;
char **argv[];
{
	char buf[128], buff[512], host[32];
	int  i, j, k;
	int  status;
  	char cmd[256];

	for (i=0; i<512; i++) hps_hosts[i].select = 0;
	strcpy(buf, "/u/user0/yshi/hps.hosts"); 
	i = 0;
        if ((fd = fopen(buf,"r")) != NULL) {
       	    while (fgets(buff, 225, fd)) {
		if (!isdigit(buff[0]) || (buff[0] == '#')) continue;
		sscanf(buff,"%s %s",hps_hosts[i].ip, hps_hosts[i].name);
/*
		printf("=== Found hps host (%s,%s)\n", 
			hps_hosts[i].ip, hps_hosts[i].name);
*/
		i++;
	    }
	    fclose(fd);
	} else {
	    printf("\n\n\t === Cannot file host file (%s).", buf);
	}
	printf(" Total HPS hosts read (%d) \n", i+1);

	/* read LL's hostlist from stdio */

	status = read(0, cmd, 22 );
	while (status > 0)
	{
		strncpy(host, cmd, 21);
/*
		printf(" lucky host:(%s)\n", host);
*/
		mark_host(host);
		status = read(0, cmd, 22);
	}	
	generate();
	close(0);
	exit (1);
}


/* Mark the given host in hps_hosts list for future generation */
mark_host(host)
char *host;
{
	int i, j;
	char buf[128];

	/* strip off the first dot address */
	for (i=0; host[i]!='.'; i++);
	host[i] = 0;
	sprintf(buf, "%s-hps", host);
/*
	printf("converted host name(%s)\n", buf);
*/
	
	for (i=0; i<512; i++)
	{
		if (!strncmp(hps_hosts[i].name,buf, strlen(buf)))
		{
			hps_hosts[i].select = 1;	
			printf(" host maked (%s)\n", buf);
			return;
		}
	}
	printf(" Cannot find matching host?? (%s)\n",host);
	return;
}

generate()
{
	int i;
	char buf[256]; /* for printing a line */
	char fname[32];

	if (!(fd = fopen("/u/user0/yshi/.sng_hosts", "w")))
	{
		perror("Cannot write to $HOME/.sng_hosts?? \n");
		exit ;
	}

	for (i=0; i<512; i++)
	{
		if (hps_hosts[i].select)
		{
			fprintf(fd,"%s %s tcp unix yshi hps\n",
				hps_hosts[i].ip, hps_hosts[i].name);
			start_remote(hps_hosts[i].ip, "yshi", "cid &");
		}
	}
	fclose(fd);
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

printf(" start %s @(%s)\n",cmd, host);

  if (!(se = getservbyname("exec", "tcp"))) {
	perror(" cannot get execport??");
	return 0;
  }
  execport = se->s_port;
  endservent();
  if (!rexec(&host, execport, login, "s1nerg1700", cmd, (char *)0))
  {
	perror(" cannot rexec?? \n");
	return 0;
  }
  return 1;
}
