/*====================================================================*/
/*    Destroy all tuples in a named tuple space                       */
/*    Yuan Shi, April 2004                                            */
/*    Upon request by Negesh */ 
/*====================================================================*/
#include "synergy.h"
#include "iolib.h"

int  cnf_rmall( id )
    int   id;
{
    int     sock;
    char mapid[MAP_LEN] ;
    u_short this_op = TSH_RMALL;
    sng_map *link_pt;
    tsh_get_it out;
    tsh_get_ot1 in1;

	this_op = htons(this_op) ;
	link_pt = handles[id];
	if ((sock = get_socket()) == -1)
	{
		perror("cnf_tsget: get_socket error\n") ;
		close(sock);
		return(TSGET_ER) ;
	}
	if (!do_connect(sock, inet_addr(link_pt->cpu), 
		link_pt->port))
	{
		perror("cnf_tsget: TSH connection error\n") ;
		close(sock);
		return(TSGET_ER) ;
	}      
	if (!writen(sock, (char *)&this_op, sizeof(u_short)))
	{
		perror("cnf_tsget: Op code send error\n") ;
		close(sock);
		return(TSGET_ER) ;
	}
				/* read result */
	if (!readn(sock, (char *)&in1, sizeof(tsh_get_ot1)))
	{
		perror("cnf_tsrmall: read status error\n") ;
		close(sock);
		return(TSGET_ER) ;
	}
	             /* get connection in the return port */
	if (ntohs(in1.status) != SUCCESS) 
	{
		printf(" cnf_tsrmall. Received ACK error??\n");
		close(sock);
		return(TSGET_ER);
	}

	// printf(" CNF_TSRMALL. Sent in host(%ul) port(%d)\n", out.host, out.cidport);
	close(sock) ;
	return(0);
}
