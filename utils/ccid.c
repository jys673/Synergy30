/*Check remote CID*/

#include "synergy.h"

main(argc, argv)
int argc;
char **argv;
{
        char *ipaddr;
        char *login;
        char *hostnm;

        cid_null_ot in;
        int sock, cidport;
        char mapid[MAP_LEN];
	u_long cid_host; 
        struct hostent *host;
        u_short this_op;

        ipaddr = argv[1];
        login = argv[2];
        hostnm = argv[3];

        if (isdigit(ipaddr[0]))
                cid_host = inet_addr(ipaddr);
        else {
                if ((host = gethostbyname(ipaddr)) == NULL) {
                        printf("O/S error (ccid)::gethostbyname\n");
                        exit(1);
                }
                cid_host = *((long *)host->h_addr_list[0]);
        }
				/*Get socket and connect to CID*/
	alarm(TIME$OUT);
	if ((sock = get_socket()) == -1) {
		printf("O/S error (ccid)::get_socket\n");
		exit(2);
	}
	sprintf(mapid, "sng$cid$%s", login);
	if (!(cidport = pmd_getmap(mapid, cid_host, (u_short)PMD_PROT_TCP))) {
/*
		printf("PMD down -> CID down (%s)\n", hostnm);
*/
		exit(3);
	}
        if (!do_connect(sock, cid_host, cidport)) {
/*
                printf("CID down (%s)\n", hostnm);
*/
                exit(4);
        }
	this_op = htons(CID_OP_NULL);
	if (!writen(sock, (char *)&this_op, sizeof(this_op))) {
		printf("Network error (ccid) ::writen\n");
		exit(5);
	}
	if (!readn(sock, (char *)&in, sizeof(in))) {
		printf("Network error (ccid) ::readn\n");
		exit(6);
	}
	printf("++ Benchmark (%2d) ",ntohs(in.status));
	close (sock);
	exit(0);
}
