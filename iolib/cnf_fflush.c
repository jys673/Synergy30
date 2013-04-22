/*--------------------------------------------------------------------------
    cnf_fflush() - flushes a file 
    RETURNS:  1 if success or 0 if error.
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"

int cnf_fflush(id)
    int   id;		    /* index into cnf_map to get channel #/ptr */
{

    int  sock;	    	    /* read socket */
    sng_map 	*link_pt;
    u_short	this_op = FAH_OP_FFLUSH;	/* Op code for flush */
    fah_fflush_ot in ;
    fah_fflush_it out ;

    link_pt = handles[id];
    if (link_pt == NULL)
    {
	printf("cnf_fflush: Wrong object handle.\n");
	return(FFLUSH_ER);
    }
    switch (link_pt->type) {
    case 'f':
				/* get socket and connect to FAH */
	if ((sock = get_socket()) == -1)
	{
		printf("cnf_fflush: get_socket error\n") ;
		close(sock);
		return(FFLUSH_ER) ;
	}
	if (!do_connect(sock, inet_addr(link_pt->cpu), 
		link_pt->port))
	{
		printf("cnf_fflush: connect FAH error\n") ;
		close(sock);
		return(FFLUSH_ER) ;
	}      
	/* Send Op Code */
	this_op = htons(this_op);
	if (!writen(sock, (char *)&this_op, sizeof(this_op)))
	{
	       printf("cnf_fflush: send Op_Code error\n") ;
	       close(sock);
	       return(FFLUSH_ER) ;
	}
   	out.fid = htons((u_short)link_pt->sd) ;
	/* Send parameters */
	if (!writen(sock, (char *)&out, sizeof(out)))
	{
		printf("cnf_fflush: send parameter error\n") ;
		close(sock);
		return(FFLUSH_ER);
	}
	/* Read result from FAH */
	if (!readn(sock, (char *)&in, sizeof(in)))
	{
		printf("cnf_fflush: read result error\n") ;
		close(sock);
		return(FFLUSH_ER) ;
	} 
	close(sock) ;
	break;
    default:
	printf("cnf_fflush: illegal type(%c)", link_pt->type);
	return(FFLUSH_ER);
    }
    if (sng_map_hd.d>1) {
	printf("cnf_fflush status |%d|\n", ntohs(in.status));
    }
    return(ntohs(in.status));
}
