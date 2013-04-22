/* xpc.c. A program forked by "bcr" and is responsible for talk to CID */

#include "synergy.h"

int main (argc, argv)
int argc;
char **argv;
{
  u_long         lcid_hostid;
  sng_int16      lcid_port;
  int            socket_len;
  cid_recieve_bc *bc_socket;
  u_short        this_op;
  int            sock;

  sscanf(argv[1], "%ul\0", &lcid_hostid);
  lcid_port  = atoi(argv[2]);
  socket_len = atoi(argv[4]);
  bc_socket = (cid_recieve_bc *) malloc(socket_len);
  bcopy(argv[3], (char *)bc_socket, socket_len);

  this_op    = CID_OP_VT_UPD;

printf("XPC: lcid_hosid:(%ul)\n", lcid_hostid);
printf("XPC: lcid_port:(%d)\n", lcid_port);
printf("XPC: socket_len:(%d)\n", socket_len);
printf("XPC: this_op:(%d)\n", this_op);

printf("XPC: argv[3](0).host:(%ul)\n", bc_socket[0].host);
printf("XPC: argv[3](0).port:(%d)\n", bc_socket[0].port);

  if ((sock = get_socket()) != -1)
    if (do_connect(sock, lcid_hostid, lcid_port))
      if (writen(sock, (char *)&this_op, sizeof(u_short)))
        if (writen(sock, (char *)&socket_len, sizeof(int)))
          if (writen(sock, argv[3], socket_len))
		free(argv[3]);
  close(sock);
}

