/*====================================================================*/
/*  Subroutine   cnf_dzap()                                          */
/*     Removes all local CID's tuples                                 */
/*                                                                    */
/*====================================================================*/
#include "synergy.h"
#include "iolib.h"

int  cnf_dzap( )
{
    int  sock;
    u_short this_op = TSH_OP_ZAP;
    tsh_start_it out;

        if ((sock = get_socket()) == -1)
        {
                perror("cnf_dread. No socket\n");
                close(sock);
                return (TSREAD_ER);
        }
        if (!do_connect(sock, sng_map_hd.host, sng_map_hd.cidport))
        {
                perror("cnf_dread. No connect\n");
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
	strcpy(out.appid, sng_map_hd.appid);
				/* send data to TSH */
	if (!writen(sock, (char *)&out, sizeof(tsh_start_it)))
	{
		perror("cnf_dget: Length send error\n") ;
		close(sock);
		return(TSGET_ER);
	}
        close(sock) ;

        return 1;
}
