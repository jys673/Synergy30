/*
 Used in automatic parallelizer-generated parallel programs for master/worker
 to read data from tuple spaces.

 Feijian Sun
 06/12/2003
 */
#include "synergy.h"
#include "iolib.h"

int  _read_data( id, tpname, tpvalue, tpsize )
    int   id, tpsize;
    char *tpname;
    char *tpvalue;
{
    int     sock;
    u_short this_op = TSH_ADVANCED_OP_READ_DATA;
    sng_map *link_pt;
    tsh_get_it out;
    tsh_get_ot1 in1;
    tsh_get_ot2 in2;

    this_op = htons(this_op) ;
    link_pt = handles[id];
    if ((sock = get_socket()) == -1)
    {
            perror("_read_data: get_socket error\n") ;
            close(sock);
            return(TSGET_ER) ;
    }
    if (!do_connect(sock, inet_addr(link_pt->cpu),
            link_pt->port))
    {
            perror("_read_data: TSH connection error\n") ;
            close(sock);
            return(TSGET_ER) ;
    }
    if (!writen(sock, (char *)&this_op, sizeof(u_short)))
    {
            perror("_read_data: Op code send error\n") ;
            close(sock);
            return(TSGET_ER) ;
    }
    strcpy(out.expr,tpname);
    out.host = sng_map_hd.host; /* gethostid(); */
    out.port = link_pt->ret_port;
    out.len  = htonl(tpsize);
    out.proc_id = htonl(getpid());
                            /* send data to TSH */
    if (!writen(sock, (char *)&out, sizeof(tsh_get_it)))
    {
            perror("_read_data: Length send error\n") ;
            close(sock);
            return(TSGET_ER);
    }
                            /* read status */
    if (!readn(sock, (char *)&in1, sizeof(tsh_get_ot1)))
    {
            perror("_read_data: read status error\n") ;
            close(sock);
            return(TSGET_ER) ;
    }
                            /* get connection in the return port */
    if (ntohs(in1.status) != SUCCESS)
    {
            close(sock);
            if (tpsize == -1) return(0); /* non-blocking read */
            sock = get_connection(link_pt->sd, 0) ;
    }
                            /* read tuple details from TSH */
    if (!readn(sock, (char *)&in2, sizeof(tsh_get_ot2)))
    {
            perror("_read_data: tuple details read error\n") ;
            close(sock) ;
            return(-1);
    }
                            /* read tuple from TSH */
    strcpy(tpname,in2.name) ;
    tpsize = ntohl(in2.length) ;
    if (!readn(sock, tpvalue, tpsize))
    {
            perror("_read_data: tuple read error\n") ;
            close(sock);
            return(TSGET_ER);
    }

    close(sock) ;
    return(tpsize);
}
