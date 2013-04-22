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
FILE *sngfp;

main (argc, argv)
int argc;
char **argv;
{
	hosts_it *hostsng, *hostfree;
        char sfname[128], buff[128];
        char flag, namebuf[NAME_LEN];
	int host_idx, i, rep;
	int check_status = 0;

	hostsng = list_p = end_p = NULL;
	host_idx = 0;
	if (argc > 1) check_status = 1;

        sprintf(sfname,"%s/.sng_hosts", getenv("HOME")); 
	printf("== Checking Host Pool Status ... \n");
        if ((sngfp = fopen(sfname,"r")) != NULL) {
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
			if (check_status)
			strcpy(hostsng->status,check_alive(hostsng->ipaddr,
			hostsng->login));
			else strcpy(hostsng->status,"-----");
		}
		else {
			sscanf(hostsng->ipaddr,"%c %s",&flag, namebuf);
			if (check_status)
			strcpy(hostsng->status,check_alive(namebuf,
			hostsng->login));
			else strcpy(hostsng->status,"-----");
		}
		if (check_status)
			printf("++ (%s) %s\n", hostsng->status, hostsng->name);
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

        flag = 'Y';
        while (flag == 'Y' || flag == 'y') {
            system("clear");
            printf("\n   \"uhost\" of Synergy V3.0 : Unmark hosts for parallel processing\n");
            printf("=Status=No.===IP Address=================Host Name===============Login==F. Sys.=\n");
       	    i = 1;
	    hostsng = end_p;
	    while (hostsng != NULL) {
		printf("[%s] (%3d) %-20s %-30s %-10s %-5s\n",
			hostsng->status, i, 
			hostsng->ipaddr, hostsng->name, hostsng->login,
			hostsng->fsys);
		if (i % 20 == 0) {
			markhosts(host_idx);
		        system("clear");
            		printf("\n   \"uhost\" of Synergy V3.0 : Unmark hosts for parallel processing\n");
            		printf("=Status=No.===IP Address=================Host Name===============Login==F. Sys.=\n");
		}
		hostsng = hostsng->next;
		i ++;
	    }
	    if (i % 20 > 0) markhosts(host_idx);
	    flag = 'n'; 
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
	printf("\n\t Synergy host file (~/.sng_hosts) updated. \n");
}	


void markhosts(list_cnt)
int list_cnt;
{
	hosts_it *hostsng;
	char temp_buff[128], ipaddr[128], name[128], ch;
	int begin, end, i, j;

	do {
		begin = end = -1;
		printf("\t Unmark from (0 to continue) #:");
		scanf("%d", &begin);
		if (begin > 0) {
		    printf("\t                        To   #: ");
		    scanf("%d", &end);
		}
		if ((begin < 0) || (end < 0)) 
			exit;
		if (end < begin) end = begin;
		hostsng = end_p;
		if (begin > 0 && end > 0 &&
				begin <= list_cnt && end <= list_cnt) {
			for (i = 1; i <= begin; i ++)
			    if (i != 1)
				hostsng = hostsng->next;
			for (i = i - 1; i <= end; i++) {
			    if (hostsng->buffline[0] == '#') {
				for (j=0; j<strlen(hostsng->buffline); j++)
					temp_buff[j] = hostsng->buffline[j+1];
				strcpy(hostsng->buffline, temp_buff);
				sscanf(hostsng->buffline,"%s %s", ipaddr,name);
				printf("\t (%s, %s) unmarked.\n",name, ipaddr);
			    }
			    hostsng = hostsng->next;
			} 
		}
	} while (begin > 0 && end > 0); 
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
