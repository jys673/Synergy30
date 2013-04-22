/*====================================================================*/
/*  Subroutine   cnf_dput()                                           */
/*     Inserts a typle into a direct tuple space                      */
/*                                                                    */
/*====================================================================*/
#include "synergy.h"
#include "iolib.h"

int  cnf_dput(tsd, tid, tpname, tpvalue, tpsize )
    int    tsd; 
    long   tpsize;
    char   *tid, *tpname; 
    char   *tpvalue;
{
    tid_list *tid_ptr;
 
	if (!strcmp(tid,"local")) return(
		send_to(tid, sng_map_hd.host,sng_map_hd.cidport,
			tpname, tpvalue, tpsize)); 
	tid_ptr = tid_header;
	while (tid_ptr != NULL)
	{
/*
printf(" cnf_dput. check tid list. tid(%s) host(%ul) port(%d)\n",
			tid_ptr->tid, tid_ptr->host, tid_ptr->cid_port);
*/
		if (!strcmp(tid_ptr->tid, tid))
		{  /* found a matching tid */
/*
printf(" cnf_dput. found a match (%s) (%s)\n", tid_ptr->tid, tid);
*/
			if (tid_ptr->host > 0)  /* tid has host  infor */
			{
/*
printf(" cnf_dput: (%s) found TID(%s) info:Host(%ul) port(%d)\n", tpname, tid, tid_ptr->host,
			tid_ptr->cid_port);
*/

				return(
				send_to(tid_ptr->tid, 
					tid_ptr->host,
					tid_ptr->cid_port, 
					tpname, tpvalue, tpsize));
			}
			else	
			{
printf(" cnf_dput: cannot find tid destination?? (%s) \n", tpname);
				return (0);
			}
			break;
		}
		tid_ptr = tid_ptr->next;
	}
	printf("cnf_dput. cannot find tid dest2? (%s) tid(%s)\n", tpname,tid);
}

int send_to(tid, shost, cid_port, tpname, tpvalue, tpsize)
   char *tid, *tpname;
   ulong shost;
   int   cid_port;
   char *tpvalue;
   long  tpsize;
{ 
   int    id, sock;
   char mapid[MAP_LEN] ;
   char host[MAP_LEN], login[MAP_LEN];
   u_short this_op = TSH_OP_PUT;
   sng_map *link_pt;
   tsh_put_it out;
   tsh_put_ot in;

/*
printf(" Send_direct_to(tid(%s) tpname(%s) host(%ul) cid_port(%d)\n", tid,
		tpname, shost, cid_port);
	if (strcmp(tid, "local")) 
		if (shost == sng_map_hd.host) {
		printf(" local exists. No need to send\n");
		return 1;
	}
*/
	id = 0; /* The first handle */
	link_pt = handles[id];
 
 	if (( sock = get_socket()) == -1)
	{
		perror("connectCid3:: get socket failure\n");
		return -1;
	}
lp:
	if (!do_connect(sock,shost, cid_port))
	{
		perror("connectCid3:: no connection\n");
		printf(" host(%lu) port(%d)\n",shost, cid_port);
		sleep(2);
		goto lp;
	}
	this_op = htons(this_op) ;
	if (!writen(sock, (char *)&this_op, sizeof(u_short)))
	{
		perror("cnf_dput: Op code send error\n") ;
		close(sock);
		return(TSPUT_ER) ;
	}
	strcpy(out.name,tpname);
	out.priority = 1; 	/* Saved for later implementation */
	out.length = htonl(tpsize);
	out.host = (sng_map_hd.host); /* gethostid(); */
        out.port = (link_pt->ret_port);
	out.priority = htons(out.priority) ;
	out.proc_id = htons(getpid());
	strcpy(out.appid, sng_map_hd.appid);

				/* send data to TSH */
	if (!writen(sock, (char *)&out, sizeof(tsh_put_it)))
	{
		perror("cnf_dput: Length send error\n") ;
		close(sock);
		return(TSPUT_ER);
	}
				/* send tuple to TSH */
	if (!writen(sock, tpvalue, tpsize))
	{
		perror("cnf_dput: Value send error\n");
		close(sock);
		return(TSPUT_ER);
	}
				/* read result */
	if (!readn(sock, (char *)&in, sizeof(tsh_put_ot)))
	{
		perror("cnf_dput: read status error\n") ;
		close(sock);
		return(TSPUT_ER) ;
	}
				/* print result from TSH */
	close(sock);
	return(ntohs(in.error));
}

int scan_send(tpname, tpvalue, tpsize)
   char *tpname;
   char *tpvalue;
   long  tpsize;
{
   host_t *host_ptr;
   tid_list *tid_ptr;
   int status;

	/* First send it the the local CID */
	status = send_to("local", sng_map_hd.host,
			 sng_map_hd.cidport, tpname, tpvalue, tpsize);
printf(" Scan Send (%s)!!! \n",tpname);


  	host_ptr = list_host;
	while (host_ptr != NULL)
	{
		if (host_ptr->hostid != sng_map_hd.host) {
/*
printf(" cnf_dput: scan send. host(%ul) port(%d)\n", host_ptr->host, host_ptr->cid_port);
*/


		if (!send_info_to(host_ptr->hostid, host_ptr->cidport, tpname, tpsize))
		{
			printf(" Failed to send virtual tuple to (%ld)\n",
				tid_ptr->host);
			return -1;
		}
		}
		host_ptr = host_ptr->next;
	}
	return (status);
}
	
int get_hostInfo(tid_ptr, tsd, tpname)
   tid_list *tid_ptr;
   int tsd;
   char *tpname;
{
    int     sock, status;
    char mapid[MAP_LEN] ;
    u_short this_op = TSH_OP_TIDINFO;
    sng_map *link_pt;
    tsh_tidinfo_it out;
    tsh_tidinfo_ot in;
  
	this_op = htons(this_op) ;
	link_pt = handles[tsd];
/*
printf(" cnf_dget. tsd(%d) tid(%s) cpu(%s)\n",
		tsd, tpname, link_pt->cpu);  
*/
printf(" cnf_dget. get_hostInfo: host(%lu) port(%d)\n",
		tid_ptr->host, tid_ptr->cid_port);
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
	strcpy(out.tpname, tpname);
				/* send data to TSH */
	if (!writen(sock, (char *)&out, sizeof(tsh_tidinfo_it)))
	{
		perror("cnf_tidinfo: send tpname error\n") ;
		close(sock);
		return(TSPUT_ER);
	}
				/* send tuple to TSH */
				/* read result */
	if (!readn(sock, (char *)&in, sizeof(tsh_tidinfo_ot)))
	{
		perror("cnf_tsput: read status error\n") ;
		close(sock);
		return(TSPUT_ER) ;
	}
	close(sock);
	status = (in.status);
/*
printf(" cnf_dput: get_hostinfo status (%d)\n", status);
*/
 	if (status > 0)
	{
		tid_ptr->host = (in.tid_host);
		tid_ptr->cid_port = (in.tid_cid_port);
/*
printf(" cnf_dput: From TSH: tid(%s) info: host(%ul) cidport(%d)\n", tpname,
		tid_ptr->host, tid_ptr->cid_port);
*/
		if (tid_ptr->host > 0) return 1;
		else return 0;
	} else return 0;
}

/* This function sends virtual tuple to remote CID/TSH */
int send_info_to( host, cid_port, tpname, tpsize)
   ulong host;
   int  cid_port;
   char *tpname;
   long  tpsize;
{
	u_short this_op = CID_OP_VT_UPD;  /* update virtual tuple */
	int sock;
	cid_recieve_bc out;
	

/*
printf("Sending virtual tuple(%s) host(%ld) cid_port(%d)\n",
	tpname, host, cid_port);
*/
	this_op = htons(this_op) ;
	if ((sock = get_socket()) == -1)
	{
		perror("connectTsh::get_socket\n") ;
		return(TSPUT_ER);
	}
	if (!do_connect(sock, host , cid_port))
	{
		perror("connect Remote Cid/TSH::do_connect\n") ;
		close(sock);
		return(TSPUT_ER) ;
	}      
	if (!writen(sock, (char *)&this_op, sizeof(u_short)))
	{
		perror("cnf_tsput: Op code send error\n") ;
		close(sock);
		return(TSPUT_ER) ;
	}
	out.bckey[0] = 0;
	strcpy(out.appid, sng_map_hd.appid);
	strcpy (out.name, tpname);
	out.priority = 0;
	out.host = (sng_map_hd.host);
	out.port = (sng_map_hd.cidport); 
	out.opflag = htons(VTPUT_FLAG);
				/* send to CID/TSH */
	if (!writen(sock, (char *)&out, sizeof(cid_send_bc)))
	{
		perror("cnf_tidinfo: send tpname error\n") ;
		close(sock);
		return(TSPUT_ER);
	}
	close(sock);
	return 1;  
}
