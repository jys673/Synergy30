/*--------------------------------------------------------------------------
    cnf_fread() - read a 'record' from file into buffer 
	(starting at address buff).
    RETURNS:  0 if EOF o/w number of items read.
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"

int cnf_fread(id, buf, bufsiz, nitems)
    int   id;		    /* index into cnf_map to get channel #/ptr */
    char  *buf;		    /* address of receiving buffer */
    int   bufsiz;	    /* max size of receiving buffer */
    int   nitems;	    /* number of bufsiz blocks to read */
{

    int  i, nread;	    /* the number of bytes read/received */
    int  sock;	    	    /* read socket */
    sng_map 	*link_pt;
    u_short	this_op = FAH_OP_FREAD;	/* Op code for READ */
    fah_fread_ot in ;
    fah_fread_it out ;

    link_pt = handles[id];
    if (link_pt == NULL)
    {
	printf("Wrong object handle.\n");
	return(FREAD_ER);
    }
    switch (link_pt->type) {
    case 'f':
				/* get socket and connect to FAH */
	if ((sock = get_socket()) == -1)
	{
		printf("connectFAH::get_socket\n") ;
		close(sock);
		return(FREAD_ER) ;
	}
	if (!do_connect(sock, inet_addr(link_pt->cpu), 
		link_pt->port))
	{
		printf("connectFAH::do_connect\n") ;
		close(sock);
		return(FREAD_ER) ;
	}      
	/* Send Op Code */
	this_op = htons(this_op);
	if (!writen(sock, (char *)&this_op, sizeof(this_op)))
	{
	       printf("cnf_fread: send op_code error\n") ;
	       close(sock);
	       return(FREAD_ER) ;
	}
   	out.fid = htons((u_short)link_pt->sd) ;
	out.nitems = htonl(nitems) ;
	out.size = bufsiz;
	nread = out.size*nitems;
	out.size = htonl(out.size);
	/* Send parameters */
	if (!writen(sock, (char *)&out, sizeof(out)))
	{
		printf("cnf_read: send parameter error\n") ;
	        close(sock);
		return(FREAD_ER);
	}
	/* read result len */
	if (!readn(sock, (char *)&in, sizeof(in)))
	{
		printf("cnf_read: read FAH error\n") ;
	        close(sock);
		return(FREAD_ER) ;
	}
	/* Read result from FAH */
	nread = ntohl(in.nitems) * bufsiz;
	if (!readn(sock, (char *)buf, nread))
	{
		printf("\nOpFread::readn\n") ;
	        close(sock);
		return(FREAD_ER) ;
	}
	close(sock) ;
	break;
    default:
	printf("cnf_fread: illegal type(%c)", link_pt->type);
	return(FREAD_ER);
    }
    if (sng_map_hd.d>1) {
	printf("cnf_read from: |%s|\n", link_pt->ref_name); 
    }
    return(ntohl(in.nitems));
}
