#include "synergy.h" 

void markhosts(/*int list_cnt*/);
char *check_alive();

typedef struct sngfile_struct {
	char ipaddr[NAME_LEN];
	char name[NAME_LEN];
	char protocol[NAME_LEN];
	char os[NAME_LEN];
	char login[NAME_LEN];
	char fsys[5];
	char status[10]; 
	char buffline[PATH_LEN];
	struct sngfile_struct *next;
} hosts_it;
hosts_it *list_p, *end_p;
FILE *sngfp, *tfp;

main (argc, argv)
int argc;
char **argv;
{
	hosts_it *hostsng, *hostfree;
        char sfname[128], buff[128], newname[128];
        char flag, namebuf[NAME_LEN];
	char *newP, *DotIP;
	int host_idx, i, rep;
	int check_status = 0;
	struct sockaddr_in dest;                /* for dotaddress convertion */
	struct hostent *host;

	hostsng = list_p = end_p = NULL;
	host_idx = 0;
	if (argc < 2)
	{
		printf("Usage: %s unmark_host\n", argv[0]);
		exit(0);
	};
	newP=argv[1];

	printf("newP = (%s)\n", newP);
	if (!isdigit(newP[0])) // translate into IP address
	{
     		if ((host=gethostbyname(newP)) == NULL)
     		{  printf("++ Bad node address (%s)\n", newP);
        		perror("Invalid Hostname Found");
        		exit(1);
     		}
     		dest.sin_family = host->h_addrtype;
     		bcopy(host->h_addr, (caddr_t) & dest.sin_addr, host->h_length);
     		newP = inet_ntoa(dest.sin_addr);
	}
	printf(" newP2 = (%s)\n", newP);

        sprintf(sfname,"%s/.sng_hosts", getenv("HOME")); 
	printf("== Checking Host Pool Status ... \n");
        if ((sngfp = fopen(sfname,"r")) != NULL) {
	    strcpy(newname, ".tempf");
	    tfp = fopen(newname, "w");  // Opens the output file 
       	    while (fgets(buff, 225, sngfp)) {
                if ((hostsng = (hosts_it *)malloc(sizeof(hosts_it))) == NULL) {
			printf("\n\nError : markhosts malloc\n");
               		exit(1);
		}
	        strcpy(hostsng->buffline, buff);
		sscanf(buff,"%s %s %s %s %s %s",hostsng->ipaddr,
			hostsng->name, hostsng->protocol,
			hostsng->os, hostsng->login,hostsng->fsys);
		if (hostsng->ipaddr[0] != '#') 
		{
			// Write out directly.
			fwrite(buff, strlen(buff), 1, tfp);
		}
		else {
			// Compare with newP
			sscanf(hostsng->ipaddr,"%c %s",&flag, namebuf);
			if (!strcmp(namebuf, newP)) // found a match
			{
				// remove '#'
				buff[0] = ' ';
			}
			fwrite(buff, strlen(buff), 1, tfp);
		}
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
	    printf("\n\n\t Synergy host file (%s) not present.", sfname);
            return;
	}
	fclose(tfp);
	fclose(sngfp);
	sprintf(buff, "rm %s", sfname);
	system(buff);
	sprintf(buff, "mv %s %s", newname, sfname),
	system(buff);
	printf("\n\t Synergy host file (~/.sng_hosts) updated. \n");
}	

char *check_alive(host, login)
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
                        return "Ready";
                else
                        return "No   ";
        }
        else
                return "No   ";
}
