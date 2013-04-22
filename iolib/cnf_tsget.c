/*====================================================================*/
/*  Subroutine   cnf_tsget()                                          */
/*    Destructive read a typle from a named tuple space               */
/*                                                                    */
/*====================================================================*/
#include "synergy.h"
#include "iolib.h"

int  cnf_tsget( id, tpname, tpvalue, tpsize )
    int   id, tpsize;
    char   *tpname; 
    char   *tpvalue;
{
    int     sock;
    char mapid[MAP_LEN] ;
    u_short this_op = TSH_OP_GET;
    sng_map *link_pt;
    tsh_get_it out;
    tsh_get_ot1 in1;
    tsh_get_ot2 in2;
 
	this_op = htons(this_op) ;
	link_pt = handles[id];
	if ((sock = get_socket()) == -1)
	{
		perror("cnf_tsget: get_socket error\n") ;
		close(sock);
		return(TSGET_ER) ;
	}
	if (!do_connect(sock, inet_addr(link_pt->cpu), 
		link_pt->port))
	{
		perror("cnf_tsget: TSH connection error\n") ;
		close(sock);
		return(TSGET_ER) ;
	}      
	if (!writen(sock, (char *)&this_op, sizeof(u_short)))
	{
		perror("cnf_tsget: Op code send error\n") ;
		close(sock);
		return(TSGET_ER) ;
	}
	strcpy(out.expr,tpname);
	out.host = sng_map_hd.host; /* gethostid(); */ 
	out.port = link_pt->ret_port;
	out.len = htonl(tpsize);
	out.proc_id = htonl(getpid());
	sprintf(mapid, "sng$cid$%s", getpwuid(getuid())->pw_name);
	out.cidport = pmd_getmap(mapid, sng_map_hd.host, (u_short)PMD_PROT_TCP);
				/* send data to TSH */
	if (!writen(sock, (char *)&out, sizeof(tsh_get_it)))
	{
		perror("cnf_tsget: Length send error\n") ;
		close(sock);
		return(TSGET_ER);
	}
				/* read result */
	if (!readn(sock, (char *)&in1, sizeof(tsh_get_ot1)))
	{
		perror("cnf_tsget: read status error\n") ;
		close(sock);
		return(TSGET_ER) ;
	}
	             /* get connection in the return port */
	if (ntohs(in1.status) != SUCCESS) 
	{
		close(sock);
		if (tpsize == -1) return(0); /* async read */
		sock = get_connection(link_pt->sd, 0) ;
	}
                                /* read tuple details from TSH */
	if (!readn(sock, (char *)&in2, sizeof(tsh_get_ot2)))
	{
       		perror("cnf_tsget: read result length error\n") ;
       		close(sock) ;
		return(-1);
	}                           /* print tuple details from TSH */
	strcpy(tpname,in2.name) ;
	tpsize = ntohl(in2.length) ;
                                /* read, print  tuple from TSH */
	if (!readn(sock, tpvalue, tpsize))
	{
		perror("cnf_tsget: tuple read error\n") ;
		close(sock);
		return(TSGET_ER);
	} 
/*
printf(" CNF_TSGET. Sent in host(%ul) port(%d) tpname(%s)\n",
		out.host, out.cidport, tpname);
*/
	close(sock) ;
	return(tpsize);
}
