/*--------------------------------------------------------------------------
    cnf_fseek() - set the reader pointer from "from" to "offset" in a file 
    RETURNS:  1 if success or 0 if error.
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"

int cnf_fseek(id, from, offset)
    int   id;		    /* index into cnf_map to get channel #/ptr */
    int   from, offset;
{

    int  sock;	    	    /* read socket */
    sng_map 	*link_pt;
    u_short	this_op = FAH_OP_FFLUSH;	/* Op code for flush */
    fah_fseek_ot in ;
    fah_fseek_it out ;

    link_pt = handles[id];
    if (link_pt == NULL)
    {
	printf("cnf_fseek: Wrong object handle.\n");
	return(FSEEK_ER);
    }
    switch (link_pt->type) {
    case 'f':
				/* get socket and connect to FAH */
	if ((sock = get_socket()) == -1)
	{
		printf("cnf_fseek: get_socket error\n") ;
		close(sock);
		return(FSEEK_ER) ;
	}
	if (!do_connect(sock, inet_addr(link_pt->cpu), 
		link_pt->port))
	{
		printf("cnf_fseek: connect FAH error\n") ;
		close(sock);
		return(FSEEK_ER) ;
	}      
	/* Send Op Code */
	this_op = htons(this_op);
	if (!writen(sock, (char *)&this_op, sizeof(this_op)))
	{
	       printf("cnf_fseek: send Op_Code error\n") ;
	       close(sock);
	       return(FSEEK_ER) ;
	}
   	out.fid = htons((u_short)link_pt->sd) ;
	out.from = htonl(from);
	out.offset = htonl(offset);
	/* Send parameters */
	if (!writen(sock, (char *)&out, sizeof(out)))
	{
		printf("cnf_fseek: send parameter error\n") ;
		close(sock);
		return(FSEEK_ER);
	}
	/* Read result from FAH */
	if (!readn(sock, (char *)&in, sizeof(in)))
	{
		printf("cnf_fseek: read result error\n") ;
		close(sock);
		return(FSEEK_ER) ;
	} 
	close(sock) ;
	break;
    default:
	printf("cnf_fseek: illegal type(%c)", link_pt->type);
	return(FSEEK_ER);
    }
    if (sng_map_hd.d>1) {
	printf("cnf_fseek status |%d|\n", ntohs(in.status));
    }
    return(ntohs(in.status));
}
