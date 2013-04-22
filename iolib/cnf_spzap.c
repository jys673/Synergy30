/*====================================================================*/
/*  Subroutine   cnf_spzap()                                          */
/*     Removes all "retrieve" entries in TSH                          */
/*                                                                    */
/*====================================================================*/
#include "synergy.h"
#include "iolib.h"

int  cnf_spzap( tsd)
int tsd;
{
    int  sock;
    u_short this_op = TSH_OP_ZAP;
    tsh_start_it out;
    sng_map  *link_pt;

	link_pt = handles[tsd];

        if ((sock = get_socket()) == -1)
        {
                perror("cnf_spzap. No socket\n");
                close(sock);
                return (TSREAD_ER);
        }
        if (!do_connect(sock, inet_addr(link_pt->cpu), link_pt->port))
        {
                perror("cnf_spzap. No connect\n");
                close(sock);
                return(TSREAD_ER);
        }
	this_op = htons(this_op) ;
	if (!writen(sock, (char *)&this_op, sizeof(u_short)))
	{
		perror("cnf_dget: Op code send error\n") ;
		close(sock);
		return(TSGET_ER) ;
	}
        close(sock) ;

        return 1;
}
