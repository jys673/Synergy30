/*====================================================================*/
/*  Subroutine   cnf_tsput()                                          */
/*     Inserts a typle into a named tuple space                       */
/*                                                                    */
/*====================================================================*/
#include "synergy.h"
#include "iolib.h"

int  cnf_tsput( id, tpname, tpvalue, tpsize )
    int   id, tpsize;
    char   *tpname; 
    char   *tpvalue;
{
    int     sock;
    char mapid[MAP_LEN] ;
    u_short this_op = TSH_OP_PUT;
    sng_map *link_pt;
    tsh_put_it out;
    tsh_put_ot in;
   
	this_op = htons(this_op) ;
	link_pt = handles[id];
	if ((sock = get_socket()) == -1)
	{
		perror("connectTsh::get_socket\n") ;
		return(TSPUT_ER);
	}
	if (!do_connect(sock, inet_addr(link_pt->cpu), 
		link_pt->port))
	{
		perror("connectTsh::do_connect\n") ;
		close(sock);
		return(TSPUT_ER) ;
	}      
	if (!writen(sock, (char *)&this_op, sizeof(u_short)))
	{
		perror("cnf_tsput: Op code send error\n") ;
		close(sock);
		return(TSPUT_ER) ;
	}
	strcpy(out.name,tpname);
	out.priority = 1; 	/* Saved for later implementation */
	out.length = tpsize;
	out.host = sng_map_hd.host; /* gethostid(); */
        out.port = link_pt->ret_port;
	out.priority = htons(out.priority) ;
	out.length = htonl(out.length) ;
	out.proc_id = htonl(getpid());
				/* send data to TSH */
	if (!writen(sock, (char *)&out, sizeof(tsh_put_it)))
	{
		perror("cnf_tsput: Length send error\n") ;
		close(sock);
		return(TSPUT_ER);
	}
				/* send tuple to TSH */
	if (!writen(sock, tpvalue, tpsize))
	{
		perror("cnf_tsput: Value send error\n");
		close(sock);
		return(TSPUT_ER);
	}
				/* read result */
	if (!readn(sock, (char *)&in, sizeof(tsh_put_ot)))
	{
		perror("cnf_tsput: read status error\n") ;
		close(sock);
		return(TSPUT_ER) ;
	}
				/* print result from TSH */
	close(sock);
	sys_TIDS++;
	return((int)ntohs(in.error));
}
