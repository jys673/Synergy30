/*********************************************************/
/*             delhost: mark a node unavailable          */
/*********************************************************/
#include "synergy.h"

int check_alive();

typedef struct sngfile_struct {
	char buffline[128];
	struct sngfile_struct *next;
} hosts_it;

main (argc, argv)
int argc;
char *argv[];
{
	hosts_it *hostsng, *hostfree, *list_p, *end_p;
	FILE *sngfp;
        char sfname[128], buff[128], name[128];
	char t_name[128], t_ipaddr[128], t_temp[128], t_os[128], t_login[128],t_fsys[5];
	int host_idx, i, force_delete, marked;
	struct sockaddr_in dest;                /* for dotaddress convertion */
        struct hostent *host;

        if (argc < 2 || argc >3) {
		printf("Markoff or remove a node from .sng_hosts \n");
                printf("Usage: delhost <hostname or ipaddr> [-f]\n");
                return;
        }
	if (argc > 2) force_delete = 1; // -f option
	else force_delete = 0;

	hostsng = list_p = end_p = NULL;
	host_idx = 0;

        sprintf(sfname,"%s/.sng_hosts", getenv("HOME")); 

        if ((sngfp = fopen(sfname,"r")) != NULL) {
       	    while (fgets(buff, 225, sngfp)) {
                if ((hostsng = (hosts_it *)malloc(sizeof(hosts_it))) == NULL) {
			printf("Error : DELHOSTS_malloc\n");
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
	} else {
	    printf("file (%s) not present.", sfname);
	    return;
	}
	strcpy(name, argv[1]);
	if (!isdigit(argv[1][0]))
	{
		if ((host=gethostbyname(name)) == NULL)
                {  printf("++ Bad node address (%s)\n", name);
                        perror("Invalid Hostname Found");
                        exit(1);
                }
                dest.sin_family = host->h_addrtype;
                bcopy(host->h_addr, (caddr_t) & dest.sin_addr, host->h_length);
                strcpy(name, inet_ntoa(dest.sin_addr));
	}
	printf("covered IP=(%s)\n",name);
	hostsng = hostfree = end_p;
	while (hostfree != NULL) 
	{
		sscanf(hostfree->buffline, "%s %s %s %s %s %s", 
			t_ipaddr, t_name, t_temp, t_os, t_login, t_fsys);
		if (t_ipaddr[0] == '#') // marked
		{
			marked == 1;
			strcpy(t_ipaddr, &t_ipaddr[1]); // remove mark for comp
		} else marked = 0;
		if (isdigit(name[0])? !strcmp(t_ipaddr, name) : !strcmp(t_name, name)) {
			if (force_delete) // Remove this line
			{
		    		if (hostfree == hostsng) end_p = hostfree->next;
		   		else hostsng->next = hostfree->next;
		    		free(hostfree);
		    		hostfree = hostsng;
		    		printf("Host (%s) deleted.\n",name);
				break;
			}  
			if (!marked) // mark this line 
			{
				sprintf(hostfree->buffline, 
				  "#%s %s %s %s %s %s\n", 
				  t_ipaddr,t_name,t_temp,t_os,t_login,t_fsys);
				printf("Host (%s) marked unavailable.\n",
					t_name);
			}
			break;
		}
		hostsng = hostfree;
		hostfree = hostfree->next;
	}
	if (hostfree == NULL) {
		printf("Host (%s) does not exist.\n", name);
		exit(1);
	}

        hostsng = hostfree = end_p;
        if ((sngfp = fopen(sfname,"w")) == NULL) {
                printf("Error : fopen\n");
                return;
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

