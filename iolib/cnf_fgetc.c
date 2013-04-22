/*--------------------------------------------------------------------------
    cnf_fgetc() - read a char from file into buffer 
    RETURNS:  0 on EOF, o/w 1.
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"

int cnf_fgetc(id, buf)
    int   id;		    /* index into cnf_map to get channel #/ptr */
    char  *buf;		    /* address of receiving buffer */
{
    int  sock;	    	    /* read socket */
    sng_map 	*link_pt;
    u_short	this_op = FAH_OP_FGETC;	/* Op code for fgetc */
    fah_fgetc_ot in ;
    fah_fgetc_it out ;

    link_pt = handles[id];
    if (link_pt == NULL)
    {
	printf("cnf_fgetc: Wrong object handle.\n");
	return(FGETC_ER);
    }
    switch (link_pt->type) {
    case 'f':
				/* get socket and connect to FAH */
	if ((sock = get_socket()) == -1)
	{
		printf("cnf_fgetc: get_socket error\n") ;
		close(sock);
		return(FGETC_ER) ;
	}
	if (!do_connect(sock, inet_addr(link_pt->cpu), 
		link_pt->port))
	{
		printf("cnf_fgetc: connect FAH error\n") ;
		close(sock);
		return(FGETC_ER) ;
	}      
	/* Send Op Code */
	this_op = htons(this_op);
	if (!writen(sock, (char *)&this_op, sizeof(this_op)))
	{
	       printf("cnf_fgetc: send Op_Code error\n") ;
		close(sock);
	       return(FGETC_ER) ;
	}
   	out.fid = htons((u_short)link_pt->sd) ;
	/* Send parameters */
	if (!writen(sock, (char *)&out, sizeof(out)))
	{
		printf("cnf_fgetc: send parameter error\n") ;
		close(sock);
		return(FGETC_ER);
	}
	/* read result len */
	if (!readn(sock, (char *)&in, sizeof(in)))
	{
		printf("cnf_fgetc: read FAH length error\n") ;
		close(sock);
		return(FGETC_ER) ;
	}
	*buf = in.ch;
	close(sock) ;
	break;
    default:
	printf("cnf_fgetc: illegal type(%c)", link_pt->type);
	return(FGETC_ER);
    }
    if (sng_map_hd.d>1) {
	printf("cnf_fgetc from: %s ch: |%c|\n", link_pt->ref_name, 
	    *buf);
    }
    return(ntohs(in.status));
}
