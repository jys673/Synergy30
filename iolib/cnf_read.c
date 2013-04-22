/*--------------------------------------------------------------------------
    cnf_read() - read a 'record' from file or pipe into buffer 
	(starting at address buff).
    RETURNS:  0 on EOF, o/w number of bytes read.
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"

int cnf_read(id, buf, bufsiz)
    int   id;		    /* index into cnf_map to get channel #/ptr */
    int   bufsiz;	    /* max size of receiving buffer */
    char  *buf;		    /* address of receiving buffer */
{

    int  nread;		    /* the number of bytes read/received */
    int  sock;	    	    /* read socket */
    u_long len; 
    sng_map 	*link_pt;
    fah_fgets_it out;

    link_pt = handles[id];
    if (link_pt == NULL)
    {
	printf("Wrong object handle.\n");
	return(READ_ER);
    }
    switch (link_pt->type) {
    case 'p': 
	if (link_pt->dirn == '<') /* always use local mbx */
	{
		if (link_pt->ret_port == -1) /* get connection first */
		{
			sock = link_pt->sd;
			link_pt->sd = get_connection(sock, 0);
			close(sock);
			link_pt->ret_port = 0;
		}
		if(!readn(link_pt->sd,(char *)&out,sizeof(out)))
		{
			printf("cnf_read: Pipe read length error\n");
			close(link_pt->sd);
			return(READ_ER);
		} 
		if (!writen(link_pt->sd,(char *)&out,sizeof(out)))
		{
			printf("cnf_read: Pipe write sync error\n");
			close(link_pt->sd);
			return(READ_ER);
		}
		out.len = ntohl(out.len);
		if (out.len != 0) 
		{
			if (!readn(link_pt->sd,buf,out.len))
			{
				printf("cnf_read: Pipe read error\n");
				close(link_pt->sd);
				return(READ_ER);
			}
		}
		bufsiz = out.len;
		nread = out.len;
	} /* of INPUT */
	break;
    default:
	printf("cnf_read: illegal type(%c)", link_pt->type);
	return(READ_ER);
    }
    if (sng_map_hd.d>2) {
	printf("[%s] cnf_read from: %s msg: |%s|", sng_map_hd.name,
		link_pt->ref_name, buf);
    }
    return(nread);
}
