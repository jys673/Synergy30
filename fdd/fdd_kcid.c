#include "synergy.h"

main(argc, argv)
int argc;
char *argv[];
{
	u_long hostid;
	u_short cidport;

   	cid_exit_ot in;
   	int sock;
   	u_short this_op;

        hostid = atoi(argv[1]);
        cidport = atoi(argv[2]);

   	alarm(10);
   	if ((sock = get_socket()) == -1) exit(1);
   	if (!do_connect(sock, hostid, cidport)) exit(2);
   	this_op = htons(CID_OP_EXIT);
   	if (!writen(sock, (char *)&this_op, sizeof(this_op))) exit(3);
/*   	if (!readn(sock, (char *)&in, sizeof(in))) exit(4); */
	close(sock);
   	exit(0);
}
