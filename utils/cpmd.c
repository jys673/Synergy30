/*Check remote PMD*/

#include "synergy.h"

main(argc, argv)
int argc;
char **argv;
{
	char *ipaddr;
	char *login;
	char *hostnm;

	pmd_exit_ot in;
	int sock;
        u_long pmd_host;
	struct hostent *host;
	u_short this_op;

	ipaddr = argv[1];
	login = argv[2];
	hostnm = argv[3];

	if (isdigit(ipaddr[0]))
		pmd_host = inet_addr(ipaddr);
	else {
		if ((host = gethostbyname(ipaddr)) == NULL) {
			printf("Error! Check_PMD::gethostbyname\n");
			exit(1);
		}
		pmd_host = *((long *)host->h_addr_list[0]);
	}
                                /*Get socket and connect to PMD*/
	alarm(TIME$OUT);
	if ((sock = get_socket()) == -1) {
		printf("Error! Check_PMD::get_socket\n");
                exit(2);
        }
	if (!do_connect(sock, pmd_host, htons(PMD_PORT))) {
/* 
		printf("PMD down (%s)\n", hostnm);
*/ 
		exit(3);
	}
	this_op = htons(PMD_OP_NULL);
	if (!writen(sock, (char *)&this_op, sizeof(this_op))) {
		printf("Error! Check_PMD::writen\n");
		exit(4);
	}
        if (!readn(sock, (char *)&in, sizeof(in))) {
		printf("Error! Check_PMD::readn\n");
		exit(5);
        }
	close(sock);
	exit(0);
}

