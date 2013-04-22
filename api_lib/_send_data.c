/*
 Used in automatic parallelizer-generated parallel programs for master/worker
 to send data into tuple spaces.

 Feijian Sun
 06/12/2003
 */
#include "synergy.h"
#include "iolib.h"

int  _send_data( id, tpname, tpvalue, tpsize )
    int   id, tpsize;
    char *tpname;
    char *tpvalue;
{
    int     sock;
    u_short this_op = TSH_ADVANCED_OP_SEND_DATA;
    sng_map *link_pt;
    tsh_put_it out;
    tsh_put_ot in;

    this_op = htons(this_op) ;
    link_pt = handles[id];
    if ((sock = get_socket()) == -1)
    {
            perror("_send_data: get_socket error\n") ;
            close(sock);
            return(TSPUT_ER);
    }
    if (!do_connect(sock, inet_addr(link_pt->cpu),
            link_pt->port))
    {
            perror("_send_data: TSH connection error\n") ;
            close(sock);
            return(TSPUT_ER) ;
    }
    if (!writen(sock, (char *)&this_op, sizeof(u_short)))
    {
            perror("_send_data: Op code send error\n") ;
            close(sock);
            return(TSPUT_ER) ;
    }
    strcpy(out.name,tpname);
    out.priority = 1;       /* Saved for later implementation */
    out.priority = htons(out.priority) ;
    out.host = sng_map_hd.host; /* gethostid(); */
    out.port = link_pt->ret_port;
    out.length = htonl(tpsize) ;
    out.proc_id = htonl(getpid());
                            /* send data to TSH */
    if (!writen(sock, (char *)&out, sizeof(tsh_put_it)))
    {
            perror("_send_data: Length send error\n") ;
            close(sock);
            return(TSPUT_ER);
    }
                            /* send tuple to TSH */
    if (!writen(sock, tpvalue, tpsize))
    {
            perror("_send_data: Value send error\n");
            close(sock);
            return(TSPUT_ER);
    }
                            /* read status */
    if (!readn(sock, (char *)&in, sizeof(tsh_put_ot)))
    {
            perror("_send_data: read status error\n") ;
            close(sock);
            return(TSPUT_ER) ;
    }

    close(sock);
    sys_TIDS++;
    return((int)ntohs(in.error));
}
