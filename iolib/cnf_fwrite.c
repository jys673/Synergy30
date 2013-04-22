/*--------------------------------------------------------------------------
    cnf_fwrite() - write a 'record' from buffer into file 
	(starting at address buff).
    RETURNS:  number of bytes written.
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"

int cnf_fwrite(id, buf, bufsiz, nitems)
    int   id;		    /* index into cnf_map to get channel #/ptr */
    char  *buf;		    /* address of receiving buffer */
    int   bufsiz;	    /* max size of receiving buffer */
    int   nitems;	    /* number of bufsiz blocks to write */
{

    int  nread;		    /* the number of bytes read/received */
    int  sock;	    	    /* read socket */
    sng_map 	*link_pt;
    u_short	this_op = FAH_OP_FWRITE;	/* Op code for FWRITE */
    fah_fwrite_ot in ;
    fah_fwrite_it out ;

    link_pt = handles[id];
    if (link_pt == NULL)
    {
	printf("Wrong object handle.\n");
	return(FWRITE_ER);
    }
    switch (link_pt->type) {
    case 'f':
				/* get socket and connect to FAH */
	if ((sock = get_socket()) == -1)
	{
		printf("connectFAH::get_socket\n") ;
		close(sock);
		return(FWRITE_ER) ;
	}
	if (!do_connect(sock, inet_addr(link_pt->cpu), 
		link_pt->port))
	{
		printf("connectFAH::do_connect\n") ;
		close(sock);
		return(FWRITE_ER) ;
	}      
	/* Send Op Code */
	this_op = htons(this_op);
	if (!writen(sock, (char *)&this_op, sizeof(this_op)))
	{
	       printf("cnf_fwrite: send op_code error\n") ;
	       close(sock);
	       return(FWRITE_ER) ;
	}
   	out.fid = htons((u_short)link_pt->sd) ;
	out.nitems = htonl(nitems) ;
	out.size = htonl(bufsiz);
	nread = nitems;
	/* Send parameters */
	if (!writen(sock, (char *)&out, sizeof(out)))
	{
		printf("cnf_read: send parameter error\n") ;
	        close(sock);
		return(FWRITE_ER);
	}
	/* send the stuff */
	if (!writen(sock, (char *)buf, nitems * bufsiz))
	{
		printf("cnf_write: write FAH error\n") ;
	        close(sock);
		return(FWRITE_ER) ;
	}
	/* read FAH operation status */
	if (!readn(sock, (char *)&in, sizeof(in)))
	{
		printf("cnf_write: read FAH status error\n");
	        close(sock);
		return(FWRITE_ER);
	}
	if (!(ntohs(in.status)))
	{
		printf("cnf_write: remote write failure. Error(%d)\n",
			in.error );
		return(FWRITE_ER);
	}
	close(sock) ;
	break;
    default:
	printf("cnf_fwrite: illegal type(%c)", link_pt->type);
	return(FWRITE_ER);
    }
    if (sng_map_hd.d>1) {
	printf("cnf_write from: (%s) \n", link_pt->ref_name); 
    }
    return(ntohl(in.nitems));
}
