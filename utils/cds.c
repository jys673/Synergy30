/*Check the pmds and cids, and Creat the Living_Host_List*/

#include "synergy.h"
#define LIVING_HOST_LIST ".snghosts"

void arlm_handler();
char *getenv();

int g_status;

main(int argc, char *argv[])
{
	FILE *infile, *outfile;
	char ipaddr[32], login[32], dae_host[128], ptlnm[32], osnm[32];
	char local_hostname[128], filenm[128], home[128], fsys[128];
	int status;
	int file_sw = 0, check_sw = 0;

	if (argc > 1) 
	{
		if (argv[1][1] == 'a') check_sw = 1; // Show all hosts including marked
		else file_sw = 1;  // Generate a live host file
	}
	gethostname(local_hostname, sizeof(local_hostname));
	strcpy(home, getenv("HOME"));
	sprintf(filenm, "%s/.sng_hosts", home);
 
	signal(SIGALRM, arlm_handler);
	if ((infile = fopen(filenm, "r")) != NULL)
	{ 
		if (file_sw) 
		    if ((outfile = fopen(LIVING_HOST_LIST, "w")) == NULL)
			exit(1);
 		status = fscanf(infile, "%s %s %s %s %s %s\n", 
				ipaddr, dae_host, ptlnm, osnm, login, fsys);
		while ((status > 0)) {
		if (ipaddr[0] != '#') {
			g_status = 1;
			g_status = check_pmd(ipaddr);
			if (g_status) 
			{
				g_status = check_cid(ipaddr, login, dae_host);
				if (g_status)
				{
					printf("++ (%s) ready. \n", dae_host);  
					if (file_sw) 
						fprintf(outfile, 
							"%s %s %s %s %s %s\n",
							ipaddr, dae_host, ptlnm, 
							osnm, login, fsys);
				} 
			//	else if (!file_sw) // 2nd part of report
			//		printf("%s) (%s)\n",dae_host, login); 
			}
			else if (!file_sw) printf(" ????? PMD down (%s,%s)\n", ipaddr, dae_host); 
		} else if (check_sw) printf("Unavailable host [%s,%s]\n", ipaddr, dae_host);

		status = fscanf(infile, "%s %s %s %s %s %s\n", 
				ipaddr, dae_host, ptlnm, osnm, login, fsys);
		}
	}
	fclose(infile);
	if (file_sw) fclose(outfile);
}

int check_pmd(ipaddr)
char *ipaddr;
{

	pmd_exit_ot in;
	int sock;
        u_long pmd_host;
	struct hostent *host;
	u_short this_op;

	if (isdigit(ipaddr[0]))
		pmd_host = inet_addr(ipaddr);
	else {
		if ((host = gethostbyname(ipaddr)) == NULL) {
			printf("Error! Check_PMD::gethostbyname\n");
			return 0;
		}
		pmd_host = *((long *)host->h_addr_list[0]);
	}
                                /*Get socket and connect to PMD*/
	alarm(7);
	if ((sock = get_socket()) == -1) {
		printf("Error! Check_PMD::get_socket\n");
                return 0;
        }
	if (!do_connect(sock, pmd_host, htons(PMD_PORT))) {
		perror("Error! Check_PMD::do_connect");
		return 0;
	}
	this_op = htons(PMD_OP_NULL);
	if (!writen(sock, (char *)&this_op, sizeof(this_op))) {
		printf("Error! Check_PMD::writen\n");
		return 0;
	}
        if (!readn(sock, (char *)&in, sizeof(in))) {
		printf("Error! Check_PMD::readn\n");
		return 0;
        }
	close(sock);
	return 1;
}


int check_cid(ipaddr, login, hostnm)
char *ipaddr;
char *login;
char *hostnm;
{
        cid_null_ot in;
        int sock, cidport;
        char mapid[MAP_LEN];
	u_long cid_host; 
        struct hostent *host;
        u_short this_op;

        if (isdigit(ipaddr[0]))
                cid_host = inet_addr(ipaddr);
        else {
                if ((host = gethostbyname(ipaddr)) == NULL) {
                        printf("O/S error (ccid)::gethostbyname\n");
                        return 0;
                }
                cid_host = *((long *)host->h_addr_list[0]);
        }
				/*Get socket and connect to CID*/
	alarm(2);
	if ((sock = get_socket()) == -1) {
		printf("O/S error (ccid)::get_socket\n");
		return 0;
	}
	sprintf(mapid, "sng$cid$%s", login);
	if (!(cidport = pmd_getmap(mapid, cid_host, (u_short)PMD_PROT_TCP))) {
		printf("PMD down -> CID down (%s,%s)\n", ipaddr,hostnm);
		return 0;
	}
        if (!do_connect(sock, cid_host, cidport)) {

                printf(" ????? CID down (%s,%s) (%s)\n",ipaddr, hostnm, login);
                return 0;
        }
	this_op = htons(CID_OP_NULL);
	if (!writen(sock, (char *)&this_op, sizeof(this_op))) {
		printf("Network error (ccid) ::writen\n");
		return 0;
	}
	if (!readn(sock, (char *)&in, sizeof(in))) {
		printf("Network error (ccid) ::readn\n");
		return 0;
	}
	printf("++ Benchmark (%2d) ",ntohs(in.status));
	close (sock);
	return 1;
}

void arlm_handler()
{
	printf(" Alarm !! Unreacheable host found. \n"); 
	g_status = 0;
	signal(SIGALRM, arlm_handler);
	return;
}
