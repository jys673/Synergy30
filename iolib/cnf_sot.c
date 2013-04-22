/*====================================================================*/
/*  Subroutine   cnf_sot()                                            */
/*     Marks the start of scantering of tasks.                        */
/*     Yuan Shi 11-24-96                                              */
/*====================================================================*/
#include "synergy.h"
#include "iolib.h"

int  cnf_sot( id )
    int   id;
{
    int     sock;
    char mapid[MAP_LEN] ;
    u_short this_op = TSH_OP_SOT;
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
	close(sock);
	sys_TIDS = 0;
	return(1);
}
