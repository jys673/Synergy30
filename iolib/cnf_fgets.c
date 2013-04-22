/*--------------------------------------------------------------------------
    cnf_fgets() - read a line from file into buffer 
	(starting at address buff).
    RETURNS:  0 on EOF, o/w number of bytes read.
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"

int cnf_fgets(id, buf, bufsiz)
    int   id;		    /* index into cnf_map to get channel #/ptr */
    char  *buf;		    /* address of receiving buffer */
    int   bufsiz;	    /* max size of receiving buffer */
{

    int  i, nread;	    /* the number of bytes read/received */
    int  sock;	    	    /* read socket */
    sng_map 	*link_pt;
    u_short	this_op = FAH_OP_FGETS;	/* Op code for fgets */
    fah_fgets_ot in ;
    fah_fgets_it out ;

    link_pt = handles[id];
    if (link_pt == NULL)
    {
	printf("cnf_fgets: Wrong object handle.\n");
	return(FGETS_ER);
    }
    switch (link_pt->type) {
    case 'f':
				/* get socket and connect to FAH */
	if ((sock = get_socket()) == -1)
	{
		printf("cnf_fgets: get_socket error\n") ;
		close(sock);
		return(FGETS_ER) ;
	}
	if (!do_connect(sock, inet_addr(link_pt->cpu), 
		link_pt->port))
	{
		printf("cnf_fgets: connect FAH error\n") ;
		close(sock);
		return(FGETS_ER) ;
	}      
	/* Send Op Code */
	this_op = htons(this_op);
	if (!writen(sock, (char *)&this_op, sizeof(this_op)))
	{
	       printf("cnf_fgets: send Op_Code error\n") ;
		close(sock);
	       return(FGETS_ER) ;
	}
   	out.fid = htons((u_short)link_pt->sd) ;
	out.len = htonl(bufsiz);
	/* Send parameters */
	if (!writen(sock, (char *)&out, sizeof(out)))
	{
		printf("cnf_fgets: send parameter error\n") ;
		close(sock);
		return(FGETS_ER);
	}
	/* read result len */
	if (!readn(sock, (char *)&in, sizeof(in)))
	{
		printf("cnf_fgets: read FAH length error\n") ;
		close(sock);
		return(FGETS_ER) ;
	}
	/* Read result from FAH */
	nread = ntohl(in.len);
	if (!readn(sock, (char *)buf, nread))
	{
		printf("cnf_fgets: read result error\n") ;
		close(sock);
		return(FGETS_ER) ;
	} 
	if (!ntohs(in.status) && (nread == 0)) buf = NULL;
	close(sock) ;
	break;
    default:
	printf("cnf_fgets: illegal type(%c)", link_pt->type);
	return(FGETS_ER);
    }
    if (sng_map_hd.d>1) {
	printf("cnf_fgets from: %s msg: |%s|\n", link_pt->ref_name, 
	    buf);
    }
    if (buf == NULL) return (0);
    else return (nread); 
}
