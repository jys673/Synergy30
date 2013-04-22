/*---------------------------------------------------------------------
    cnf_close(id) - close all internal data structures according to type. 
    RETURNS:  nothing.
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"
 
int cnf_close(id)
    int  id;
{
	sng_map *link_pt;
	int sd;	
	u_short	this_op;
	char    mapid[MAP_LEN];	
	tsh_exit_ot tin;
	fah_fclose_ot fin;
	fah_fclose_it fout;
	fah_fgets_it out;


	link_pt = handles[id];
	if (sng_map_hd.d > 0) 
		printf("cnf_close: closing \"%s\"\n", link_pt->ref_name);
    	switch(link_pt->type) {
    	case 't':
/* Let cid to take care of the killings 
		sd = get_socket();
		do_connect(sd,inet_addr(link_pt->cpu),
			link_pt->port);
		this_op = htons(TSH_OP_EXIT); 
		if (!writen(sd, (char *)&this_op, sizeof(u_short)))
		{
			printf("cnf_close: Op Send Error\n");
			return(CLOSE_ER);
		}	
		if (!readn(sd, (char *)&tin, sizeof(tsh_exit_ot)))
		{
			printf("cnf_close: Read status error\n");
			return(CLOSE_ER);
		}
		close(link_pt->sd );
*/
       		break;
    	case 'p':
		if (link_pt->dirn == '<')
		{
			sprintf(mapid,"%s$%s",sng_map_hd.appid,
				link_pt->obj_name);
			sngd_unmap(mapid, 0, link_pt->login, PMD_PROT_TCP);
		} else {
			printf("cnf_close(p). Sending EOF \n");
			out.len = htonl(0);
			if (!writen(link_pt->sd,(char *)&out,sizeof(fah_fgets_it)))
			{
				printf("cnf_close (p): send length error\n");
				return(CLOSE_ER);
			}
/* No need to read sync. YS. 9/22/96 
			if (!readn(link_pt->sd,(char *)&out, sizeof(fah_fgets_it)))
			{
				printf("cnf_close (p): read sync error\n");
				return(WRITE_ER);
			}
*/
		}
		break;
	case 'f':
		sd = get_socket();
		do_connect(sd,inet_addr(link_pt->cpu),
			link_pt->port);
		this_op = htons(FAH_OP_FCLOSE); /* close */
		if (!writen(sd, (char *)&this_op, sizeof(u_short)))
		{
			printf("cnf_close: Op Send Error\n");
			return(CLOSE_ER);
		}	
		fout.fid = htons(link_pt->sd);
		if (!writen(sd, (char *)&fout, sizeof(fah_fclose_it)))
		{
			printf("cnf_close: send fid error\n");
			return(CLOSE_ER);
		}
		if (!readn(sd, (char *)&fin, sizeof(fah_fclose_ot)))
		{
			printf("cnf_close: Read status error\n");
			return(CLOSE_ER);
		}
		close(link_pt->sd );
		break;
    	}
	link_pt->open = FALSE;
	return;
}
