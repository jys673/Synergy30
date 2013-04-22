/*--------------------------------------------------------------------------
    cnf_fputs() - write a line from buffer to file 
    RETURNS:  number of bytes written or 0 if error.
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"

int cnf_fputs(id, buf, bufsiz)
    int   id;		    /* index into cnf_map to get channel #/ptr */
    char  *buf;		    /* address of receiving buffer */
    int   bufsiz;
{

    int  i, nread;	    /* the number of bytes read/received */
    int  sock;	    	    /* read socket */
    sng_map 	*link_pt;
    u_short	this_op = FAH_OP_FPUTS;	/* Op code for fputs */
    fah_fputs_ot in ;
    fah_fputs_it out ;

    link_pt = handles[id];
    if (link_pt == NULL)
    {
	printf("cnf_fputs: Wrong object handle.\n");
	return(FPUTS_ER);
    }
    switch (link_pt->type) {
    case 'f':
				/* get socket and connect to FAH */
	if ((sock = get_socket()) == -1)
	{
		printf("cnf_fputs: get_socket error\n") ;
		close(sock);
		return(FPUTS_ER) ;
	}
	if (!do_connect(sock, inet_addr(link_pt->cpu), 
		link_pt->port))
	{
		printf("cnf_fputs: connect FAH error\n") ;
		close(sock);
		return(FPUTS_ER) ;
	}      
	/* Send Op Code */
	this_op = htons(this_op);
	if (!writen(sock, (char *)&this_op, sizeof(this_op)))
	{
	       printf("cnf_fputs: send Op_Code error\n") ;
	       close(sock);
	       return(FPUTS_ER) ;
	}
   	out.fid = htons((u_short)link_pt->sd) ;
	nread = bufsiz+1;
	out.len = htonl(nread);
	/* Send parameters */
	if (!writen(sock, (char *)&out, sizeof(out)))
	{
		printf("cnf_fputs: send parameter error\n") ;
		close(sock);
		return(FPUTS_ER);
	}
	/* send the buffer */
	if (!writen(sock, buf, nread))
	{
		printf("cnf_fputs: read FAH length error\n") ;
		close(sock);
		return(FPUTS_ER) ;
	}
	/* Read result from FAH */
	if (!readn(sock, (char *)&in, sizeof(in)))
	{
		printf("cnf_fputs: read result error\n") ;
		close(sock);
		return(FPUTS_ER) ;
	} 
	close(sock) ;
	break;
    default:
	printf("cnf_fputs: illegal type(%c)", link_pt->type);
	return(FPUTS_ER);
    }
    if (sng_map_hd.d>1) {
	printf("cnf_fputs from: %s msg: |%s|\n", link_pt->ref_name, 
	    buf);
    }
    return(ntohs(in.status));
}
