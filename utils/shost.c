/*********************************************************************************

  	shost.c -- Setup $HOME/.sng_hosts using /etc/hosts

	Usage: shost [default | mask1 [mask2] [mask3] ...]
		The IP address mask(s) designate the network clusters of hosts to use.
		For example, mask1=129.245.23.101, shost will only pick the hosts with
		the same first three dot-prefixes, namely 129.245.23.* .
	Author: Yuan Shi
	Date: 92

**********************************************************************************/
#include "synergy.h" 

typedef struct sngfile_struct {
	char ipaddr[NAME_LEN];
	char name[NAME_LEN];
	struct sngfile_struct *next;
} hosts_it;
hosts_it *list_p, *end_p;
FILE *sngfp;
struct hostent *host;
struct sockaddr_in dest;
char hostname[128], *ipaddr;

main (argc, argv)
int argc;
char *argv[];
{
	hosts_it *hostsng, *hostfree;
        char sfname[128], buff[128], login[128];
        char flag, namebuf[NAME_LEN];
	int host_idx, i, rep, cnt = 1;

	if (argc <= 1) {
		printf("\n%s : Setup parallel processing hosts on multiple clusters. \n",argv[0]);
		printf("Usage : %s <default | ipaddr1, ipaddr2, ...> \n\n",argv[0]);
		exit(1);
	}
	strcpy(login,getpwuid(getuid())->pw_name);
	if (isdigit(argv[1][0])) {
		ipaddr = (char *)malloc(128*4);
		strcpy(ipaddr,argv[1]);
	} else {
        gethostname(hostname,sizeof(hostname));
        if ((host=gethostbyname(hostname)) == NULL) {
		printf(" Invalid host (%s)\n",hostname);	
		exit (1);
	}
        dest.sin_family = host->h_addrtype;
        bcopy(host->h_addr, (caddr_t) & dest.sin_addr, host->h_length);
        if ((ipaddr = inet_ntoa(dest.sin_addr)) == (char *)-1)
 	{	
		printf("Invalid host (%s).\n",hostname);
		exit (1);
	}
	}

	hostsng = list_p = end_p = NULL;
	host_idx = 0;

loop:
	cnt ++;
	for (i=strlen(ipaddr); ipaddr[i] != '.'; i--);
	ipaddr[i] = 0;
	printf("*** IP address mask (%s)\n",ipaddr);

	strcpy(sfname, "/etc/hosts"); 
        if ((sngfp = fopen(sfname,"r")) != NULL) {
       	    while (fgets(buff, 225, sngfp)!='\0') {
		if (!isdigit(buff[0]) || (buff[0] == '#') || (buff[0] == 0)) continue;
	/* get ipaddr to namebuf */
		sscanf(buff,"%s",namebuf);
		if (!same_cluster(namebuf)) continue;
                if ((hostsng = (hosts_it *)malloc(sizeof(hosts_it))) == NULL) {
			printf("\n\nError : markhosts malloc\n");
               		exit(1);
		}
		sscanf(buff,"%s %s",hostsng->ipaddr, hostsng->name);
		printf("=== Found cluster host (%s,%s)\n",hostsng->ipaddr,
			hostsng->name);
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
	    printf("\n\n\t === Cannot file host file (%s).", sfname);
	    exit (1);
	}
 	if (cnt < argc)
	{
		strcpy(buff,argv[cnt]);
		if (isdigit(buff[0]))
		{
			ipaddr = (char *)malloc(128*4);
			strcpy(ipaddr,argv[cnt]);
			goto loop;
		}	
	}
        hostsng = hostfree = end_p;
        sprintf(sfname,"%s/.sng_hosts", getenv("HOME")); 
        if ((sngfp = fopen(sfname,"w")) == NULL) {
                printf("Error : fopen\n");
                return;
	}
        while (hostfree != NULL) { 
                fprintf(sngfp, "%s %s tcp unix %s none\n", hostfree->ipaddr,
			hostfree->name,login);
		hostfree = hostsng->next;
		free(hostsng);
                hostsng = hostfree;
        }
	fclose(sngfp);
	printf("\n*** Synergy host file (%s) created. \n",sfname);
}	

int same_cluster(name)
char *name;
{
	
	int i,j;
	
	if (!strncmp(ipaddr,name,strlen(ipaddr))) return 1;
	else return 0;
}
