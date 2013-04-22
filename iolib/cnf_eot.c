/*====================================================================*/
/*  Subroutine   cnf_eot()                                            */
/*     Marks the end of tasks.                        */
/*     Yuan Shi 11-24-96                                              */
/*====================================================================*/
#include "synergy.h"
#include "iolib.h"

int  cnf_eot( id )
    int   id;
{
    int     sock;
    char mapid[MAP_LEN] ;
    u_short this_op = TSH_OP_EOT;
    sng_map *link_pt;
    tsh_retrieve_it out;
   
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
	out.proc_id = htonl(sys_TIDS);
	if (!writen(sock, (char *)&out, sizeof(tsh_retrieve_it)))
	{
		perror("cnf_eot: sys_TIDS send err\n") ;
		close(sock);
		return(TSPUT_ER) ;
	}
printf("cnf_eot: sys_TIDS(%d)\n", sys_TIDS);

	close(sock);
	return(1);
}
