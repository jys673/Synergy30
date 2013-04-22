/*----------------------------------------------------------
    cnf_init() - initializes sng_map_hd and sng_map using either the 
    the init file or direct transmission from DAC.  
    The init file's name is constructed from the value
    of the logical name CNF_MODULE suffixed with ".ini".
    RETURNS: nothing.
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"
extern char **environ;

int namtoad(), status2;
char *getlognam();
void cnf_print_map();
void get_cpu_list();

int cnf_init()
{
    FILE *fp;			    /* file pointer of init file */
    char buf[PATH_LEN];		    /* line buffer for input from init file */
    char *bufp;			    /* current index into buf */
    char *bp;			    /* temporary buffer pointer */
    char infile[PATH_LEN];	    /* name of init file */
    sng_map *link_pt; 
    int  idx,i, plist_cnt;
    dac_data_ot header;
    dac_link_ot links;
    dac_data_it stat;
    struct in_addr in;
    host_t *p_q;
    char mapid[TUPLENAME_LEN];

    if (gethostname(local_node, sizeof(local_node)) < 0)
    {
	perror("cnf_int: can't get local hostname"); 
	return(INIT_ER);
    }
// printf("starting app at (%s)\n",local_node);
    namtoad( local_node, local_addr );
    
    /* create the name of the init file from the module name (which
    comes from the CNF_MODULE logical name in the environment, suffixed
    with the value INIT_SUFFIX */

    tid_header = NULL;
    bp = getlognam("CNF_MODULE");
    if ( bp != NULL) { 
    	strcpy(infile, bp); 
    	strcat(infile, ".ini"); 
    	fp = fopen(infile, "r"); 
    }
    if  (( bp == NULL ) || (fp == NULL)) {
	if (!readn(0,(char *)&header, sizeof(dac_data_ot)))
	{
		printf("cnf_init: read from DAC header error.\n");
		close(0);
		return(INIT_ER);
	}
	sng_map_hd.f = ntohs(header.f);
	sng_map_hd.p = ntohs(header.p);
	sng_map_hd.t = ntohs(header.t);
	sng_map_hd.d = ntohs(header.d);
	sng_map_hd.link_cnt = ntohs(header.link_cnt) + 1; /* for dts ys96 */

	if (ntohs(header.protocol) == DAC_PROT_TCP)
	sprintf(sng_map_hd.protocol,"tcp");
	else sng_map_hd.protocol[0] = 0;
	strcpy(sng_map_hd.appid,header.appid);
	strcpy(sng_map_hd.name, header.name);
/*
printf("CNF_INIT: appid(%s) headerName(%s)\n",header.appid, header.name);
*/
	strcpy(sng_map_hd.csl_name,"");	
	idx = ntohs(header.link_cnt);
	sng_map_hd.link_hd = NULL;
	for (i=0; i<idx; i++)
	{
		if (!readn(0,(char *)&links, sizeof(dac_link_ot)))
		{
			printf("conf_init: read link from DAC error\n");
			close(0);
			return(INIT_ER);
		}
		link_pt = (sng_map *)malloc(sizeof(sng_map));
		link_pt->next = sng_map_hd.link_hd;
		sng_map_hd.link_hd = link_pt;
		link_pt->type = links.type;
		link_pt->dirn = links.dirn;
		strcpy(link_pt->ref_name,links.ref_name);
		strcpy(link_pt->obj_name,links.obj_name);
		strcpy(link_pt->phys_name,links.path);
		strcpy(link_pt->login, links.login);
		in.s_addr = links.host;
		strcpy(link_pt->cpu,inet_ntoa(in));
	}
	/* receive cpu_list here. YS f96 */
	get_cpu_list();

	/* now add the direct tuple space object */
	link_pt = (sng_map *)malloc(sizeof(sng_map));
	link_pt->next = sng_map_hd.link_hd;
	sng_map_hd.link_hd = link_pt;
	link_pt->type = 't';
	link_pt->dirn = '*';
	strcpy(link_pt->ref_name, "DynamicTS");
	strcpy(link_pt->obj_name, "DynamicTS");
	strcpy(link_pt->phys_name, "DynamicTS");
	strcpy(link_pt->login, getpwuid(getuid())->pw_name);
	link_pt->cpu[0] = 0; /* not used */
	link_pt->open = TRUE;
	link_pt->sd = get_socket();
	link_pt->ret_port = bind_socket(link_pt->sd, 0);
	link_pt->port = 0; /* To be filled by respective calls */
	/* end of direct tuple space addition */

	stat.status = htons(SUCCESS);
	stat.error = htons(DAC_ER_NOERROR);
	if (!writen(0,(char *)&stat, sizeof(dac_data_it)))
	{
		printf("cnf_init: send ACK error \n");
		close(0);
		return(INIT_ER);
	}
	close(0);
    	if (sng_map_hd.d > 1) cnf_print_map();
    	handles = (sng_map **) malloc(sizeof(sng_map *)*sng_map_hd.link_cnt);
    	for (sng_idx = 0 ; sng_idx < sng_map_hd.link_cnt ; sng_idx++) 
		handles[sng_idx] = NULL ; 
/*
	sng_map_hd.host = sng_gethostid(); 
*/
	sng_map_hd.host = header.host; 
        sprintf(buf, "sng$cid$%s", getpwuid(getuid())->pw_name);
       	if (!(sng_map_hd.cidport = pmd_getmap(buf,sng_map_hd.host,
			(u_short)PMD_PROT_TCP)))
    	{
	    printf("cnf_init_error: cidport lookup failure.\n");
	    exit(1);
    	}

	/* Reset the dts now. Ys96 */
	handles[0] = sng_map_hd.link_hd;
    	sng_idx = 1;	/* reset the open count */

//       printf("cnf_init done. host (%lu) \n",sng_map_hd.host);
        return;
    }
    else { /* Debug Mode */ 
    /* read the first line of the init file */
    if ((bufp = fgets(buf, PATH_LEN, fp)) == NULL)
    {
	printf("cnf_init: empty init file\n");
	return(INIT_ER);
    }

    /* read all the variable initialization lines */
    sscanf(bufp, "%s\n", buf);
    // This line makes debugging master dependent on cds host order: sng_map_hd.host = inet_addr(buf);
    sng_map_hd.host = sng_gethostid(); 
    status2=fscanf(fp, "%s", sng_map_hd.appid);
    status2=fscanf(fp, "%s", sng_map_hd.csl_name);
    status2=fscanf(fp, "%s", sng_map_hd.name);
    status2=fscanf(fp, "%s %s %s", buf, buf, sng_map_hd.protocol);
    status2=fscanf(fp, "%s %s %d", buf,buf,&sng_map_hd.f);
    status2=fscanf(fp,"%s %s %d", buf,buf,&sng_map_hd.p);
    status2=fscanf(fp,"%s %s %d", buf,buf,&sng_map_hd.t);
    status2=fscanf(fp,"%s %s %d",buf,buf,&sng_map_hd.d);
//    printf("CNF_INIT. f(%d) p(%d) t(%d)\n",
//	sng_map_hd.f, sng_map_hd.p, sng_map_hd.t);

    /* Add cpu_list here */
    status2=fscanf(fp, "%d\n", &plist_cnt);
    for (i=0; i<plist_cnt; i++)
    {
        if ((p_q = (host_t *) malloc(sizeof(host_t))) == NULL)
                             exit(E_MALLOC);
	strcpy(p_q->app_id, sng_map_hd.appid);
	status2=fscanf(fp, "%s %s", buf, p_q->login);
	strcpy(p_q->login, getpwuid(getuid())->pw_name);
	p_q->hostid = inet_addr(buf);
        p_q->next = NULL;
	sprintf(mapid, "sng$cid$%s", p_q->login);
	if (!(p_q->cidport = pmd_getmap(mapid,p_q->hostid,
		(u_short)PMD_PROT_TCP)))
	{
	    printf("cnf_init_error: remote cid port lookup failure.\n");
	    exit(1);
	}
        if (i == 0) list_host = end_host = p_q;
        else { 
		end_host->next = p_q;
                end_host = p_q;
        }
    }
    /* Now read arguments */
    bufp = fgets(buf, PATH_LEN, fp);
    idx = 0;
    while (*bufp != ':')
    {
	sscanf(bufp,"%s %s %s",buf,buf,sng_map_hd.args[idx]);
	// printf(" CNF_INIT: host-read(%s)\n", sng_map_hd.args[idx]); 
    	bufp = fgets(buf, PATH_LEN, fp);
	idx++;
    }
    /* Skip ":" */
    bufp = fgets(buf, PATH_LEN, fp);
    sng_map_hd.link_hd = NULL;
    sng_map_hd.link_cnt = 0;
    while (bufp != NULL) 
    {
	link_pt = (sng_map *)malloc(sizeof(sng_map));
	link_pt->next = sng_map_hd.link_hd;
	sng_map_hd.link_hd = link_pt;
	sscanf(bufp,"%c %c %s %s %s %s %s\n",
		&link_pt->type, 
		&link_pt->dirn,
		link_pt->ref_name,
		link_pt->obj_name,
		link_pt->cpu,
		link_pt->phys_name,
		link_pt->login);
	// printf(" CNF_INIT: ref(%s) objnm(%s) host(%s) pname(%s) login(%s)\n",
	// 	link_pt->ref_name,
	//	link_pt->obj_name,
	//	link_pt->cpu,
	//	link_pt->phys_name,
	//	link_pt->login);

	link_pt->open = FALSE;
	link_pt->port = 0;
	link_pt->ret_port = 0;
	bufp = fgets(buf, PATH_LEN, fp);
       	sng_map_hd.link_cnt++;	/* increment number of table entries */
    }
    /* Now add direct tuple space */
    sng_map_hd.link_cnt ++;
    link_pt = (sng_map *)malloc(sizeof(sng_map));
    link_pt->next = sng_map_hd.link_hd;
    sng_map_hd.link_hd = link_pt;
    link_pt->type = 't';
    link_pt->dirn = '*';
    strcpy(link_pt->ref_name, "DynamicTS");
    strcpy(link_pt->obj_name, "DynamicTS");
    strcpy(link_pt->phys_name, "DynamicTS");
    gethostname(link_pt->cpu, sizeof(link_pt->cpu));
    strcpy(link_pt->login, getpwuid(getuid())->pw_name);
    link_pt->open = TRUE;
    link_pt->sd = get_socket();
    link_pt->ret_port = bind_socket(link_pt->sd, 0);
    link_pt->port = 0; /* To be filled by respective calls later */
    /* end of dts ys96 */

    /* close init file */
    fclose(fp);
    if (sng_map_hd.d > 1) cnf_print_map();
    handles = (sng_map **) malloc(sizeof(sng_map *)*sng_map_hd.link_cnt);
    for (sng_idx = 0 ; sng_idx < sng_map_hd.link_cnt ; sng_idx++) 
	handles[sng_idx] = NULL ;

    sprintf(buf, "sng$cid$%s", getpwuid(getuid())->pw_name);
    if (!(sng_map_hd.cidport = pmd_getmap(buf,sng_map_hd.host,
		(u_short)PMD_PROT_TCP)))
    {
    	printf("cnf_init_error: cidport lookup failure.\n");
    	exit(1);
    }
//  printf("cnf_init. hostip(%ul) port(%d) appid(%s)\n",
//	sng_map_hd.host, sng_map_hd.cidport,sng_map_hd.appid);
    /* Reset the dts now. Ys96 */
    handles[0] = sng_map_hd.link_hd;
    sng_idx = 1;	/* reset the open count */
    } 
}

/*-----------------------------------------------------------------------*/
/*  Convert an internet name to its dotted address                       */
/*-----------------------------------------------------------------------*/
int namtoad( name, destdots )
    char *name, *destdots;
{
    struct in_addr  sbuf;
    char *res;
    struct  hostent  *komvos;

    res = NULL;
    if  ( ( komvos = gethostbyname( name ) ) == NULL ) {
        printf("Host under the name *%s* was not found \n", name);
        return(INIT_ER);
    }
    memcpy( (char *)&sbuf, komvos->h_addr, komvos->h_length );
    memset( (char *)destdots, 0, 128 );
    if ( ( res = inet_ntoa( sbuf ) ) == NULL ) {
        printf("Name *%s* couldn't be converted \n", name );
        perror(" ");
        return(INIT_ER);
    } else {
        strcpy( destdots, res );
    }
}

/*---------------------------------------------------------------------
    getlognam() - translate a logical name.
 ---------------------------------------------------------------------*/
char *getlognam(name)
        char *name;
{
        int l;     
        char **p;
        char hostname[MAP_LEN];

        if (strcmp(name,"SYS$NODE") == 0) { /* get node id */
                if (gethostname(hostname,sizeof(hostname)) == -1)
                        printf(" Cannot obtain host name??\n");
                else    return(hostname);
        } else {
                for (p = environ; *p != (char *) 0; p++)
                        if ((l = pcmp(name,*p)) >= 0 &&
                                *(*p+l) == '=')
                        return(*p+l+1);
                return((char *)0);
        }
}

pcmp(s1,s2)
        char *s1,*s2;
{
        int i=0;

        while (*s1 != '\0') {
                i++;
                if (*s1++ != *s2++) return(-1);
        }
        return(i);
}

void get_cpu_list()
{
        dac_getcpulist_ot in;
        host_t *p_q;
        int i, j, k;
	char mapid[TUPLENAME_LEN];

        i = j = k = 0;
        /* while connected to DAC */
        if (readn(0, (char *)&in, sizeof(dac_getcpulist_ot))) 
	{
           hlist_cnt = ntohs(in.hlist_cnt);
	   /* process in blocks of BUFFE_SIZE */
           while (hlist_cnt > BUFFE_SIZE * j) {
                j++;
                for ( ; i < BUFFE_SIZE * j && i < hlist_cnt; i ++) 
		{
                   if ((p_q = (host_t *) malloc(sizeof(host_t))) == NULL)
                                    exit(E_MALLOC);
                   k = i - BUFFE_SIZE * (j - 1);
		   strcpy(p_q->app_id, in.buffer[k].app_id);
                   p_q->hostid = (in.buffer[k].hostid);
                   p_q->proc_id = (in.buffer[k].proc_id);
                   strcpy(p_q->login, in.buffer[k].login);
                   p_q->next = NULL;
		   sprintf(mapid, "sng$cid$%s", p_q->login);
		   if (!(p_q->cidport = pmd_getmap(mapid,p_q->hostid,
			(u_short)PMD_PROT_TCP)))
		   {
		    printf("cnf_init_error: remote cid port lookup failure.\n");
		    exit(1);
		   }
                   if (i == 0) list_host = end_host = p_q;
                   else { 
			end_host->next = p_q;
                        end_host = p_q;
                   }
                }
                if (hlist_cnt > BUFFE_SIZE * j)
                   if (!readn(0, (char *)&in, sizeof(dac_getcpulist_ot)))
                      printf(" Failed to read cpu_list from DAC.\n");
           } 
        } else printf(" Failed to read cpu_list from DAC. \n");
	return;
}

