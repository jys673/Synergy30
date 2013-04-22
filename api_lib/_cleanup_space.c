/*
 Used in automatic parallelizer-generated parallel programs for master to
 cleanup previous data in tuple spaces.

 Feijian Sun
 06/12/2003
 */
#include "synergy.h"
#include "iolib.h"

int  _cleanup_space( id, workerId )
    int   id;
    char *workerId;
{
    int     sock;
    u_short this_op = TSH_ADVANCED_OP_CLEANUP_SPACE;
    sng_map *link_pt;
    tsh_put_it out;
    tsh_put_ot in;

    this_op = htons(this_op) ;
    link_pt = handles[id];
    if ((sock = get_socket()) == -1)
    {
            perror("_cleanup_space: get_socket error\n") ;
            close(sock);
            return(TSPUT_ER);
    }
    if (!do_connect(sock, inet_addr(link_pt->cpu),
            link_pt->port))
    {
            perror("_cleanup_space: TSH connection error\n") ;
            close(sock);
            return(TSPUT_ER) ;
    }
    if (!writen(sock, (char *)&this_op, sizeof(u_short)))
    {
            perror("_cleanup_space: Op code send error\n") ;
            close(sock);
            return(TSPUT_ER) ;
    }
    strcpy(out.name,workerId);
    out.priority = 1;       /* Saved for later implementation */
    out.priority = htons(out.priority) ;
    out.host = sng_map_hd.host; /* gethostid(); */
    out.port = link_pt->ret_port;
    out.length = htonl(0) ;
    out.proc_id = htonl(getpid());
                            /* send data to TSH */
    if (!writen(sock, (char *)&out, sizeof(tsh_put_it)))
    {
            perror("_cleanup_space: Length send error\n") ;
            close(sock);
            return(TSPUT_ER);
    }
                            /* read status */
    if (!readn(sock, (char *)&in, sizeof(tsh_put_ot)))
    {
            perror("_cleanup_space: read status error\n") ;
            close(sock);
            return(TSPUT_ER) ;
    }

    close(sock);
    sys_TIDS++;
    return((int)ntohs(in.error));
}
