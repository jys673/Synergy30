#include "synergy.h"

main(argc, argv)
int argc;
char *argv[];
{
	u_long hostid;
	int procid;
	u_short cidport;
	char app_id[APP_LEN];
        cid_detectproc_ot in;
	cid_detectproc_it out;
        int sock;
        u_short this_op;

	hostid = atoi(argv[1]);
	cidport = atoi(argv[2]);
	strcpy(app_id, argv[3]);
	procid = atoi(argv[4]);

	alarm(10);
	if ((sock = get_socket()) == -1) exit(-1);
        if (do_connect(sock, hostid, cidport)) { 
	    this_op = htons(CID_OP_DETECT_PROC);
	    strcpy(out.appid, app_id);
	    out.proc_id =  htonl(procid);
	    if (writen(sock, (char *)&this_op, sizeof(u_short))) 
		if (writen(sock, (char *)&out, sizeof(out))) 
		    if (readn(sock, (char *)&in, sizeof(in))) 
			if (ntohs(in.status) == SUCCESS) {

			    printf("FDD: CID & WORKER on host (%u) are fine!\n", hostid);

			    close(sock);
			    exit(0);
			} else if (ntohs(in.status) == FAILURE) {
			    close(sock);
                            exit(1);
			}
	}
	close(sock);
	exit(2);
}
