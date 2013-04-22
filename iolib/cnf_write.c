/*---------------------------------------------------------------------
    cnf_write() - send a 'record' to file (or mailbox or decnet channel)
    from buffer (starting at address buff).  bytes is the number of bytes
    to send.  id is the index into cnf_map global data structure where
    the actual channel number or file pointer is stored.
    RETURNS:  status.
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"
 
int cnf_write(id, buf, bytes)
    int   id;		    /* index into cnf_map for channel #/ptr */
    int   bytes;	    /* number of bytes to send/write */
    char  buf[];	    /* address of message to send */
{
	sng_map *link_pt;
	char mapid[MAP_LEN];
	int sock;
	fah_fgets_it out;

	link_pt = handles[id];
	if (link_pt == NULL)
	{
		printf("cnf_write: Wrong Object Handle.\n");
		return(WRITE_ER);
	}
	if (sng_map_hd.d >2) {
		printf("cnf_write: dest: %s msg: |%s|", link_pt->ref_name,
	    buf);
	}
	switch (link_pt->type) {
	case 'p': 
		if (sng_map_hd.d > 1)
			printf(" cnf_write:(%s) on (%s) to(%s)\n",
			buf,link_pt->ref_name,link_pt->cpu);
		if (link_pt->port == -1) /* 1st time call */
		{
			sprintf(mapid,"%s$%s",sng_map_hd.appid,link_pt->obj_name);
			link_pt->port = sngd_getmap(mapid,
				inet_addr(link_pt->cpu),
				link_pt->login,
				PMD_PROT_TCP);
			while (link_pt->port <=0 )
			{
				sleep(5);
				link_pt->port = sngd_getmap(mapid,
					inet_addr(link_pt->cpu),
					link_pt->login,
					PMD_PROT_TCP);
			}		
			link_pt->sd = get_socket();
printf(" cnf_write (%s) to (%s) \n",link_pt->ref_name, link_pt->cpu);
                	do_connect(link_pt->sd, inet_addr(link_pt->cpu),
                        	link_pt->port);
		}
		out.len = htonl(bytes);
		if (!writen(link_pt->sd,(char *)&out,sizeof(out)))
		{
			printf("cnf_write: send length error\n");
			close(link_pt->sd);
			return(WRITE_ER);
		}
		if (!readn(link_pt->sd,(char *)&out, sizeof(out)))
		{
			printf("cnf_write: send sync error\n");
			close(link_pt->sd);
			return(WRITE_ER);
		}
		if (!writen(link_pt->sd,buf,bytes))
		{
			printf("cnf_write: send buff error\n");
			close(link_pt->sd);
			return(WRITE_ER);
		}
/*
		close(sock);
*/
		break;
	default:
		printf("cnf_write: illegal type (%c)", link_pt->type);
		return(WRITE_ER);
	}
    	return(1);
}
