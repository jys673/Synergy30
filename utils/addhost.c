#include "synergy.h" 

int test_valid();
int check_alive();

typedef struct sngfile_struct {
	char buffline[PATH_LEN];
	struct sngfile_struct *next;
} hosts_it;

main (argc, argv)
int argc;
char *argv[];
{
	FILE *sngfp;
	hosts_it *list_p, *end_p, *hostsng, *hostfree;
	char hostname[128], ipaddr[128], login[128];
        char ch, sfname[128], buff[128], ipaddrbuf[128];	
	int host_idx, rep, valid, unmarked=0;

	if (argc < 2 || argc > 4) {
		printf("Add a node to .sng_hosts \n");
		printf("Usage: addhost <hostname or ipaddr> [login]\n");
		return;
	}

	hostsng = list_p = end_p = NULL;
	host_idx = 0;
        sprintf(sfname,"%s/.sng_hosts", getenv("HOME")); 

        if ((sngfp = fopen(sfname,"r")) != NULL) {
       	    while (fgets(buff, 225, sngfp)) {
                if ((hostsng = (hosts_it *)malloc(sizeof(hosts_it))) == NULL) {
			printf("\n\nError : markhosts malloc\n");
               		exit(1);
		}
	        strcpy(hostsng->buffline, buff);
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
	} else { /* generate one entry in .sng_hosts: local_host login */
                if ((hostsng = (hosts_it *)malloc(sizeof(hosts_it))) == NULL) {
                        printf("\n\nError : markhosts malloc\n");
                        exit(1);
                }
		gethostname(hostname, sizeof(hostname));
		test_valid(hostname, ipaddr);	/* get ipaddr by hostname */	
		strcpy(login, getpwuid(getuid())->pw_name);
		sprintf(hostsng->buffline, "%s %s tcp unix %s none", ipaddr, hostname, login);
		hostsng->next = NULL;
		list_p = end_p = hostsng;
		host_idx ++;
	}

        if (argc >= 3 && argv[2][0] != '-')
                strcpy(login, argv[2]);
	else
                strcpy(login, getpwuid(getuid())->pw_name);
	strcpy(hostname, argv[1]);
        if (isdigit(hostname[0])) strcpy(ipaddr, argv[1]);
	else test_valid(hostname, ipaddr);

	/* check redundancy */
	hostsng = end_p;
        while (hostsng != NULL) {
	  sscanf(hostsng->buffline, "%s", ipaddrbuf);
          if (!strcmp(ipaddrbuf, ipaddr)) {
		printf("Host (%s, %s) already exists.\n", ipaddr, hostname);
		exit (1);
          }
	  if (ipaddrbuf[0] == '#')
	  {
		sscanf(ipaddrbuf, "%c %s",&ch,ipaddrbuf);
		if (!strcmp(ipaddrbuf, ipaddr)) // Enable it
		{
			unmarked = 1;
			strcpy(hostsng->buffline, &hostsng->buffline[1]);
		}
	  }
          hostsng = hostsng->next;
	}

	/* update the file */
	if (!unmarked) 
	{
		sprintf(buff, "%s %s unix tcp %s none\n", 
			ipaddr, hostname, login);
		printf("(%s,%s) added. \n",hostname, login);
        	if ((hostsng = (hosts_it *)malloc(sizeof(hosts_it))) == NULL) 
		{
           		printf("\n\nError : markhosts malloc\n");
           		exit(1);
        	}
        	sprintf(hostsng->buffline, "%s", buff);
        	hostsng->next = NULL;
        	list_p->next = hostsng;
        	list_p = list_p->next;
	}
        hostsng = hostfree = end_p;
        if ((sngfp = fopen(sfname,"w")) == NULL) {
                printf("Error : fopen\n");
                exit(1);
	}
        while (hostfree != NULL) { 
                fprintf(sngfp, "%s", hostfree->buffline);
		hostfree = hostsng->next;
		free(hostsng);
                hostsng = hostfree;
        }
	fclose(sngfp);
	printf("(%s) updated. \n",sfname);
}	

/* test_valid translates a hostname into its dotted IP address */
int test_valid(hostname, ipaddr)
char *hostname;
char *ipaddr;
{
        struct hostent *host;
        struct sockaddr_in dest;
        char *destdotaddr;
        char buff1[128], buff2[128];

        strcpy(buff1, hostname);
        if ((host=gethostbyname(buff1)) != NULL) {
                dest.sin_family = host->h_addrtype;
                bcopy(host->h_addr, (caddr_t) & dest.sin_addr, host->h_length);
                if ((destdotaddr = inet_ntoa(dest.sin_addr)) == (char *)-1)
		{
			printf("Invalid host (%s).\n",hostname);
			exit (1);
		}
                else {
			strcpy(ipaddr, destdotaddr);
			return(1);
		}
         } else {
		printf("Invalid host (%s).\n",hostname);
		exit (1);
	}
}


int check_alive(host, login)
char *host;
char *login;
{
        int status, substat, cpmdid, ccidid;
        char cpmd_path[128], ccid_path[128];

        sprintf(cpmd_path, "%s/bin/cpmd", getenv("SNG_PATH"));
        sprintf(ccid_path, "%s/bin/ccid", getenv("SNG_PATH"));

        if ((cpmdid = fork())== 0) {
           execl(cpmd_path, "cpmd", host, login, host, (char *)0);
           exit(1);
        }
        if (cpmdid < 0)
                exit(2);
        if (wait(&substat) < 0)
                exit(3);
        if (substat == 0) {
                if ((ccidid = fork()) == 0) {
                       execl(ccid_path, "ccid", host, login, host, (char *)0);
                        exit(4);
                }
                if (ccidid < 0)
                        exit(5);
                if (wait(&substat) < 0)
                        exit(6);
                if (substat == 0)
                        return (1);
                else
                        return (0);
        }
        else
                return (0);
}

