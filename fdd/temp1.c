
void initFdd()
{
	host_t * p_q; 
	u_short cidport;
	char mapid[MAP_LEN];

	if (gethostlist(dacport, &host_list, &host_end, &clist_cnt)) {
	    p_q = host_list;
	    while (p_q != NULL) {
	        sprintf(mapid,"sng$cid$%s",p_q->login);
		if (!(cidport = pmd_getmap(mapid,p_q->hostid,(u_short)PMD_PROT_TCP)))
		{
		    printf("FDD_error: Remote host lookup failure.\n");
		    exit(1);
		}
		p_q->cidport = cidport;
		p_q->fault_flag = 0;
		p_q = p_q->next;
	    }
	} else {
	    printf("FDD_error: Failed to get host_list.\n");
	    exit(1);
	}
}


/*---------------------------------------------------------------------------
  Prototype   : void startFdd(void)
  Parameters  : -
  Returns     : -
  Called by   : main
  Calls       :
  Notes       :
  Date        : October' 1994
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void startFdd()
{
	host_t *p_q;
	int ccidid;
	char ccid_path[128], kcid_path[128];

	signal(SIGALRM, arlm_handler);
	p_q = host_list;
	while (TRUE) {
	    g_status = 0;
	    if (!p_q->fault_flag) {

		g_status = fdd_ccid(p_q->hostid, p_q->cidport,
					(char *)p_q->app_id, p_q->proc_id);

		if (g_status != 0) {
			printf(" Loosing host (%u) \n", p_q->hostid);
			p_q->fault_flag = 1;
			if (p_q->hostid == dachostid && g_status == 2) {
			    printf("FDD: CID on DAC_host dead...FDD exit!\n");
			    exit(1);
			}
			if (fddchangeproc(dacport, p_q) != 1)
				exit(2);
			if (Tfddretrievetuple(handler_end, p_q) != 1)
				exit(3);
		}
	    }
	    if (p_q->next != NULL) p_q = p_q->next;
	    else {
		p_q = host_list;
/*	        sleep(100*(1/hlist_cnt)); */
		sleep(15-hlist_cnt);
	    }
	}
}


/*---------------------------------------------------------------------------
  Prototype   : int Tfddretrievetuple(handlerinfo_t *handler_list, 
				host_t *deadhost)
  Parameters  : -
  Returns     : -
  Called by   : startFdd
  Calls       :
  Notes       :
  Date        : October' 1994
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

int Tfddretrievetuple(handler_list, deadhost)
handlerinfo_t *handler_list;
host_t *deadhost;
{
        tsh_retrieve_it out;
        handlerinfo_t *p_q;
        int tsh_sock; 
	u_short tshop;
        u_short tsh_port;

        out.host = deadhost->hostid;
	out.proc_id = htonl(deadhost->proc_id);
        p_q = handler_list;
        tshop = htons(TSH_OP_RETRIEVE);

        while (p_q != NULL) {
            if (p_q->type == 't') {
		p_q->host = inet_addr(p_q->inet_addr);
                if ((tsh_port = sngd_getmap(p_q->mapid, p_q->host, p_q->login, PMD_PROT_TCP)) < 0) {
                        perror("DACLIB_fddretrievetuple::sngd_getmap\n");
                        return(0);
                }
                if ((tsh_sock = get_socket()) == -1) {
                        perror("DACLIB_fddretrievetuple::get_socket\n") ;
                        return(0);
                }
printf("-- Automatic Tuple Recovery: host(%u) opcode(%u)\n", p_q->host, 
			ntohs(tshop));
                if (!do_connect(tsh_sock, p_q->host, tsh_port)) {
                        perror("DACLIB_fddretrievetuple::do_connect\n");
                        return(0);
                }
                if (!writen(tsh_sock, (char *)&tshop, sizeof(u_short))) {
                        close(tsh_sock);
                        return(0);
		}
                if(!writen(tsh_sock, (char *)&out, sizeof(tsh_retrieve_it))) {
                        close(tsh_sock);
                        return(0);
                }

                close(tsh_sock);
	    }
            p_q = p_q->next;
        }
        return(1);
}


/*---------------------------------------------------------------------------
  Prototype   : int fdd_ccid(u_long hostid, u_short cidport, char *app_id,
					int procid);
  Parameters  : -
  Returns     : -
  Called by   : startFdd
  Calls       :
  Notes       :
  Date        : January' 1995
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

int fdd_ccid(hostid, cidport, app_id, procid)
u_long hostid;
u_short cidport;
char *app_id;
int procid;
{
        cid_detectproc_ot in;
	cid_detectproc_it out;
        int sock;
        u_short this_op;

	alarm(10);
	if ((sock = get_socket()) == -1) exit(-1);
        if (do_connect(sock, hostid, cidport)) { 
	    this_op = htons(CID_OP_DETECT_PROC);
	    strcpy(out.appid, app_id);
	    out.proc_id =  htonl(procid);
	    if (writen(sock, (char *)&this_op, sizeof(u_short))) 
		if (writen(sock, (char *)&out, sizeof(out))) 
		    if (readn(sock, (char *)&in, sizeof(in))) 
			if (ntohs(in.status) == SUCCESS) {
	    		    alarm(0);
/*
			    printf("FDD: CID & WORKER on host (%u) are fine!\n", hostid);
*/
			    close(sock);
			    return(0);
			} else if (ntohs(in.status) == FAILURE) {
			    alarm(0);
			    close(sock);
                            return(1);
			}
	}
	close(sock);
	return(2);
}


void arlm_handler()
{
        g_status = 1;
        signal(SIGALRM, arlm_handler);
        return;
}
