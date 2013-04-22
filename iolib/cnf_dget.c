/*====================================================================*/
/*  Subroutine   cnf_dget()                                           */
/*    Destructive read a typle from a direct tuple space              */
/*                                                                    */
/*====================================================================*/
#include "synergy.h"
#include "iolib.h"

int  cnf_dget(tpname, tpvalue, tpsize )
    int    tpsize;
    char   *tpname; 
    char   *tpvalue;
{
    int  sock;
    u_short this_op = TSH_OP_GET;
    sng_map *link_pt;
    tsh_get_it out;
    tsh_get_ot1 in1;
    tsh_get_ot2 in2;

	/* The first handle */ 
	link_pt = handles[0];

        if ((sock = get_socket()) == -1)
        {
                perror("cnf_dget. No socket\n");
                close(sock);
                return (TSREAD_ER);
        }
        if (!do_connect(sock, sng_map_hd.host, sng_map_hd.cidport))
        {
                perror("cnf_dget. No connect\n");
                close(sock);
                return(TSREAD_ER);
        }
	this_op = htons(this_op) ;
	if (!writen(sock, (char *)&this_op, sizeof(u_short)))
	{
		perror("cnf_dget: Op code send error\n") ;
		close(sock);
		return(TSGET_ER) ;
	}
	strcpy(out.expr,tpname);
	out.host = sng_map_hd.host; /* gethostid(); */ 
	out.port = link_pt->ret_port;
	out.len = htonl(tpsize);
	out.proc_id = htonl(getpid());
	strcpy(out.appid, sng_map_hd.appid);
				/* send data to TSH */
	if (!writen(sock, (char *)&out, sizeof(tsh_get_it)))
	{
		perror("cnf_dget: Length send error\n") ;
		close(sock);
		return(TSGET_ER);
	}
				/* read result */
	if (!readn(sock, (char *)&in1, sizeof(tsh_get_ot1)))
	{
		perror("cnf_dget: read status error\n") ;
		close(sock);
		return(TSGET_ER) ;
	}
	             /* get connection in the return port */
	if (ntohs(in1.status) != SUCCESS) 
	{
		close(sock);
/*
printf(" cnf_dget. cannot get tuple (%s). Wait...\n",tpname);
*/
		if (tpsize == -1) return(0); /* async read */
		sock = get_connection(link_pt->sd, 0) ;
	}
                                /* read tuple details from TSH */
	if (!readn(sock, (char *)&in2, sizeof(tsh_get_ot2)))
	{
       		perror("cnf_dget: read result length error\n") ;
       		close(sock) ;
		return(-1);
	}                           /* print tuple details from TSH */
	strcpy(tpname,in2.name) ;
	tpsize = ntohl(in2.length) ;
                                /* read, print  tuple from TSH */
	if (!readn(sock, tpvalue, tpsize))
	{
		perror("cnf_dget: tuple read error\n") ;
		close(sock);
		return(TSGET_ER);
	} 
      in1.status = htons(SUCCESS);
      in1.error = htons(TSH_ER_NOERROR);
      if (!writen(sock, (char *)&in1, sizeof(tsh_get_ot1)))
	perror("\n DGet ack error");
      close(sock) ;

      if (ntohs(in2.priority) == 0) /* A virtual tuple */
      {
         tpsize = getRealTuple((this_op), in2.appid, in2.name,tpvalue);

      }
      return(tpsize);
}


