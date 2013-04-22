/*--------------------------------------------------------------------------
    cnf_fputc() - write a char from buffer to file 
    RETURNS:  1 if success or 0 if error.
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"

int cnf_fputc(id, buf)
    int   id;		    /* index into cnf_map to get channel #/ptr */
    char  buf;		    /* address of receiving buffer */
{

    int  sock;	    	    /* read socket */
    sng_map 	*link_pt;
    u_short	this_op = FAH_OP_FPUTC;	/* Op code for fputc */
    fah_fputc_ot in ;
    fah_fputc_it out ;

    link_pt = handles[id];
    if (link_pt == NULL)
    {
	printf("cnf_fputc: Wrong object handle.\n");
	return(FPUTC_ER);
    }
    switch (link_pt->type) {
    case 'f':
				/* get socket and connect to FAH */
	if ((sock = get_socket()) == -1)
	{
		printf("cnf_fputc: get_socket error\n") ;
		close(sock);
		return(FPUTC_ER) ;
	}
	if (!do_connect(sock, inet_addr(link_pt->cpu), 
		link_pt->port))
	{
		printf("cnf_fputc: connect FAH error\n") ;
		sleep(2);
		while (!do_connect(sock, inet_addr(link_pt->cpu), 
				   link_pt->port))
		{
			sleep(2);
		} 
	}      
	/* Send Op Code */
	this_op = htons(this_op);
	if (!writen(sock, (char *)&this_op, sizeof(this_op)))
	{
	       printf("cnf_fputc: send Op_Code error\n") ;
		close(sock);
	       return(FPUTC_ER) ;
	}
   	out.fid = htons((u_short)link_pt->sd) ;
	out.ch = buf;
	/* Send parameters */
	if (!writen(sock, (char *)&out, sizeof(out)))
	{
		printf("cnf_fputc: send parameter error\n") ;
		close(sock);
		return(FPUTC_ER);
	}
	/* Read result from FAH */
	if (!readn(sock, (char *)&in, sizeof(in)))
	{
		printf("cnf_fputc: read result error\n") ;
		close(sock);
		return(FPUTC_ER) ;
	} 
	close(sock) ;
	break;
    default:
	printf("cnf_fputc: illegal type(%c)", link_pt->type);
	return(FPUTC_ER);
    }
    if (sng_map_hd.d>1) {
	printf("cnf_fputc from: %s msg: |%c|\n", link_pt->ref_name, 
	    buf);
    }
    return(ntohs(in.status));
}
