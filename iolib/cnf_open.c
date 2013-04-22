/*---------------------------------------------------------------------
    cnf_open() - lookup a pipe or tuple space object in sng_map structure,
    open a channel to the physical address for that ref_name.
    RETURNS:   an integer handle, if successful.  This is used like
    a usual Unix file handle.
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"
 
int cnf_open(local_name, mode)
char *local_name;	/* local_name to find in cnf_map */
char *mode;		/* open modes: r,w,a,r+,w+,a+. Only for FILEs */
{
    int 	id, sock, found, loclen, chan, maplen, len;
    char 	*tmp;
    char	buf[128];
    sng_map	*link_pt;
    fah_fopen_ot in ;
    fah_fopen_it out ;
    u_short	this_op = FAH_OP_FOPEN;

    if (init_sw == 0)
    {
	init_sw = 1;
	cnf_init();
    }
    if (sng_map_hd.d > 1) printf(" cnf_open: (%s)\n",local_name);
    loclen = strlen(local_name);
    id = 0;
    found = FALSE;
    link_pt = sng_map_hd.link_hd;
    while ((link_pt != NULL) && (!found))
    {
	maplen = strlen(link_pt->ref_name);
	len = (loclen < maplen) ? loclen : maplen;
	if (strncmp(local_name, link_pt->ref_name, len) == 0)  
	{
	    	found = TRUE;
		break;
	}
	else link_pt = link_pt->next;
    }
    if (!found)
    {
	printf("cnf_open: unknown local name: '%s'\n", local_name);
	return(OPEN_ER);
    } 
    chan = sng_idx;
    handles[chan] = link_pt;
    if (sng_map_hd.d > 1)
    printf("cnf_open: opening: %s", link_pt->ref_name);

    switch (link_pt->type) {
    case 't': /* tuple space */
	/* get tsd's port number */
	sprintf(buf,"%s$%s",sng_map_hd.appid,link_pt->obj_name);
        link_pt->port = 
		sngd_getmap(buf,inet_addr(link_pt->cpu),
			link_pt->login, (u_short)PMD_PROT_TCP);
	/* setup return port */ 
	link_pt->sd = get_socket();
        link_pt->ret_port = bind_socket(link_pt->sd,0); /* no need register */
	if (sng_map_hd.d > 0) 
	{ 
		printf("**TS mapid (%s) login(%s)\n",buf,link_pt->login);
        	printf(" TS port is %d \n", link_pt->port );
		printf(" CNF_OPEN. ts return port (%u) \n",link_pt->ret_port); 
	}
     break; 
    case 'p':  /* Pipe */
	sprintf(buf,"%s$%s",sng_map_hd.appid,link_pt->obj_name);
	if (link_pt->dirn == '>')
	{
		link_pt->port = -1;  /* delay connection til first wrt */
/*
		link_pt->port = sngd_getmap(buf,inet_addr(link_pt->cpu),
				link_pt->login, (u_short)PMD_PROT_TCP);
		while (link_pt->port <=0 )
		{
			sleep(5);
			link_pt->port = sngd_getmap(buf,inet_addr(link_pt->cpu),				link_pt->login, (u_short)PMD_PROT_TCP);
		}		
		link_pt->sd = get_socket();
printf(" cnf_open (%s) to (%s)\n", local_name, link_pt->cpu);
                do_connect(link_pt->sd, inet_addr(link_pt->cpu),
                        link_pt->port);
*/
	} /* of output comm */
	else { /* input */
     		if (sng_map_hd.d>1)  
			printf(" cnf_open(%s) of module(%s) on(%s):\n",
			link_pt->ref_name,
			sng_map_hd.name,link_pt->cpu);
     		sock = get_socket(); 
     		link_pt->port = bind_socket( sock, 0 ); 
		/* register this new port number to local PMD */
		sngd_map(buf, link_pt->port, link_pt->login, PMD_PROT_TCP);
		link_pt->sd = sock;
		link_pt->ret_port = -1;
/*
		link_pt->sd = get_connection(sock,0);
		close(sock);
*/
		if (sng_map_hd.d>1) printf("Pipe open. succ:(%s:%d)\n",
			link_pt->ref_name,link_pt->port);
	} /* of input */
	break;
    case 'f':
        /* get mappinf from PMD where FAH is running */
        sprintf(buf,"%s$%s",sng_map_hd.appid,link_pt->obj_name);
        link_pt->port =
             sngd_getmap(buf, inet_addr(link_pt->cpu),
		link_pt->login, (u_short)PMD_PROT_TCP); 
	while (link_pt->port <= 0)
	{
		sleep(5);
        	link_pt->port =
             		sngd_getmap(buf, inet_addr(link_pt->cpu), 
			link_pt->login, (u_short)PMD_PROT_TCP);
	}
        /* get socket and connect to FAH */
        if ((sock = get_socket()) == -1)
        {
                printf("connectFAH::get_socket\n") ;
		close(sock);
                return(FOPEN_ER) ;
        }
        if (!do_connect(sock, inet_addr(link_pt->cpu), link_pt->port))
        {
                printf("connectFAH::do_connect\n") ;
		close(sock);
                return(FOPEN_ER) ;
        }
        this_op = htons(this_op);
        if (!writen(sock,(char *)&this_op, sizeof(u_short)))
        {
                printf("FAH Open: OP Write Error\n");
		close(sock);
                return(FOPEN_ER);
        }
        strcpy(out.path, link_pt->phys_name) ;
        strcpy(out.mode,mode) ;
printf(" File open path(%s) mode (%s)\n",out.path, out.mode);
        if (!writen(sock, (char *)&out, sizeof(fah_fopen_it)))
        {
                perror("\nFopen: Parameter Write Error\n") ;
		close(sock);
                return(FOPEN_ER) ;
        }
        /* read result */
        if (!readn(sock, (char *)&in, sizeof(fah_fopen_ot)))
        {
                perror("\nOpFopen::readn\n") ;
		close(sock);
                return(FOPEN_ER) ;
        }
	if (!ntohs(in.status))
	{
		printf("cnf_open(f) failure. Check the path! \n");
		close(sock);
		return(FOPEN_ER);
	}
        link_pt->sd = ntohs(in.fid);   /* save the remote fid */
	break;
    default:
	printf("Wrong open object type. Check with a System Operator\n");
	return(OPEN_ER);
    }
    link_pt->open = TRUE;  
    sng_idx++;
    return(chan);
} 
