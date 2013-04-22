
/**************************************************************************
	cnf_gts(tsd) -- Get all tid's processor assignments in one shot.
	Yuan Shi 11-24-96
***************************************************************************/
#include "synergy.h"
#include "iolib.h"
	
int cnf_gts(tsd)
   int tsd;
{
    int     sock, status, tids, i;
    char mapid[MAP_LEN] ;
    u_short this_op = TSH_OP_TIDINFO;
    sng_map *link_pt;
    tsh_tidinfo_it out;
    tsh_get_it out2;
    tsh_tidinfo_ot in;
    tid_list *tid_ptr;
  
	this_op = htons(this_op) ;
	link_pt = handles[tsd];
/*
printf(" cnf_dget. tsd(%d) tid(%s) cpu(%s)\n",
		tsd, tpname, link_pt->cpu);  
*/
	if ((sock = get_socket()) == -1)
	{
		perror("connectTsh::get_socket\n") ;
		return(TSPUT_ER);
	}
	if (!do_connect(sock, inet_addr(link_pt->cpu), 
		link_pt->port))
	{
		perror("connectTsh::do_connect\n") ;
		close(sock);
		return(TSPUT_ER) ;
	}      
	if (!writen(sock, (char *)&this_op, sizeof(u_short)))
	{
		perror("cnf_tsput: Op code send error\n") ;
		close(sock);
		return(TSPUT_ER) ;
	}
				/* read total tid numbers */
	if (!readn(sock, (char *)&in, sizeof(tsh_tidinfo_ot)))
	{
		perror("cnf_tsput: read status error\n") ;
		close(sock);
		return(TSPUT_ER) ;
	}
	tids = ntohs(in.status);
printf("cnf_gts, status (%d)\n", tids);
	if (tids == 0) /* send return address */
	{
		out2.host = sng_map_hd.host;
		out2.port = link_pt->ret_port;
		if (!writen(sock, (char *)&out2, sizeof(tsh_get_it)))
		{
			perror("cnf_gts. writen error \n");
			return 0;
		}
		close(sock);
printf("cnf_gts. wait for answer??\n");
		sock = get_connection(link_pt->sd, 0);
				/* read total tid numbers */
		if (!readn(sock, (char *)&in, sizeof(tsh_tidinfo_ot)))
		{
			perror("cnf_tsput: read status error\n") ;
			close(sock);
			return(TSPUT_ER) ;
		}
		tids = ntohs(in.status);
		printf("cnf_gts, status (%d)\n", tids);
	}
printf("cnf_gts. Receiving tids: \n");
	for (i=0; i< tids; i++)
	{
		if (!readn(sock, (char *)&in, sizeof(tsh_tidinfo_ot)))
		{
			perror("cnf_tsput: read status error\n") ;
			close(sock);
			return(TSPUT_ER) ;
		}
	
		tid_ptr = (struct tid_lt *) malloc(sizeof(tid_list));
		if (tid_ptr == NULL)
		{
			perror("No more memory? \n");
			return (0);
		}
		tid_ptr->host = (in.tid_host);
		tid_ptr->cid_port = (in.tid_cid_port);
		strcpy(tid_ptr->tid, in.tid);
printf(" cnf_dput: From TSH: tid(%s) info: host(%lu) cidport(%d)\n", 
		tid_ptr->tid, tid_ptr->host, tid_ptr->cid_port);
		tid_ptr->next = tid_header;
		tid_header = tid_ptr;
	}
	return (1);
}

