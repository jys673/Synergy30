/*Check the pmds and cids, and Creat the Living_Host_List*/

#include "synergy.h"
#define LIVING_HOST_LIST ".snghosts"

main(argc, argv)
char *argv;
int argc;
{
	FILE *infile, *outfile;
	char ipaddr[32], login[32], dae_host[128], ptlnm[32], osnm[32];
	char local_hostname[128], filenm[128], home[128], fsys[128];
	int status, substat, cpmdid, ccidid;
	int file_sw = 0;
	char cpmd_path[128], ccid_path[128];


	if (argc > 1) file_sw = 1;
	gethostname(local_hostname, sizeof(local_hostname));
	sprintf(home, "%s", getenv("HOME"));
	sprintf(filenm, "%s/.sng_hosts", home);
 
	sprintf(cpmd_path, "%s/bin/cpmd", getenv("SNG_PATH"));
        sprintf(ccid_path, "%s/bin/ccid", getenv("SNG_PATH"));

	if ((infile = fopen(filenm, "r")) != NULL)
	{ 
		if (file_sw) 
		    if ((outfile = fopen(LIVING_HOST_LIST, "w")) == NULL)
			exit(1);
 		status = fscanf(infile, "%s %s %s %s %s %s\n", 
				ipaddr, dae_host, ptlnm, osnm, login, fsys);
		while ((status > 0)) {
		if (ipaddr[0] != '#') {
			if ((cpmdid = fork())== 0) {
                                execl(cpmd_path, "cpmd", ipaddr, login, dae_host, 
					(char *)0);
                                printf("Error! CPMDCID_PMD::execl\n");
                                exit(1);
			}
			if (cpmdid < 0) {
				printf("Error! CPMDCID_PMDID::fork\n");
				exit(2);
			}
			if (wait(&substat) < 0) {
				printf("Error! CPMDCID_PMD::wait\n");
				exit(3);
			}
			if (substat == 0) {
				if ((ccidid = fork()) == 0) {
        	                        execl(ccid_path, "ccid", ipaddr, login, 
						dae_host, (char *)0);
                        	        printf("Error! CPMDCID_CID::execl\n");
                                	exit(4);
				}
				if (ccidid < 0) {
					printf("Error! CPMDCID_CIDID::fork\n");
					exit(5);
				}
				if (wait(&substat) < 0) {
					printf("Error! CPMDCID_CID::wait\n");
					exit(6);
				}
				if (substat == 0)
				{
					printf("++ (%s) ready. \n", dae_host);  
					if (file_sw) 
						fprintf(outfile, 
							"%s %s %s %s %s %s\n",
							ipaddr, dae_host, ptlnm, 
							osnm, login, fsys);
				} 
				else if (!file_sw)
					printf(" cid down (%s)\n",dae_host); 
			}
			else if (!file_sw) printf(" pmd down (%s)\n", dae_host); 
		}
		status = fscanf(infile, "%s %s %s %s %s %s\n", 
				ipaddr, dae_host, ptlnm, osnm, login, fsys);
		}
	}
	fclose(infile);
	if (file_sw) fclose(outfile);
}
