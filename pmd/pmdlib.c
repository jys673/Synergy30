/*.........................................................................*/
/*                PMDLIB.C ------> PMD library functions                   */
/*.........................................................................*/

#include "synergy.h"

/*---------------------------------------------------------------------------
  Prototype   : int pmd_unmap(char *mapid, int pid, u_short protocol)
  Parameters  : mapid    - logical name to be mapped
                pid      - process id
		protocol - udp/tcp
  Returns     : 1 - successful unmapping from PMD
                0 - unmapping from PMD failed
  Calls       : get_socket, do_connect, writen, readn, htons, htonl, close,
                gethostid, strcpy, ntohs
  Notes       : Connection is made to the PMD of the m/c and the mapid is
                unregistered. 
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int pmd_unmap(mapid, pid, protocol)
char *mapid; 
int pid; 
u_short protocol;
{
   pmd_unmap_it out ;
   pmd_unmap_ot in ;
   int sd ;
   u_short op ;
				/* connect to PMD */
   if ((sd = get_socket()) == -1)
      return 0 ;
   if (!do_connect(sd, gethostid(), htons(PMD_PORT)))
    {
       close(sd) ;
       return 0 ;
    }
   op = htons(PMD_OP_UNMAP) ;	/* send UNMAP operation code */
   if (!writen(sd, (char *)&op, sizeof(op)))
    {
       close(sd) ;
       return 0 ;
    }				/* prepare data to be sent */
   strcpy(out.mapid, mapid) ;
   out.pid = htonl(pid) ;
   out.protocol = htons(protocol) ;
				/* send un-mapping data to PMD */
   if (!writen(sd, (char *)&out, sizeof(out)))
    {
       close(sd) ;
       return 0 ;
    }				/* read PMD response */
   if (!readn(sd, (char *)&in, sizeof(in)))
    {
       close(sd) ;
       return 0 ;
    }
   close(sd) ;
   if (in.status == ntohs(SUCCESS))
      return 1 ;
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int pmd_map(char *mapid, u_short port, u_short protocol)
  Parameters  : mapid    - logical name to be mapped
		port     - port corresponding to the logical name
		protocol - udp/tcp
  Returns     : 1 - successful mapping with PMD
                0 - mapping with PMD failed
  Calls       : get_socket, do_connect, writen, readn, htons, htonl, close,
                gethostid, strcpy, ntohs
  Notes       : Connection is made to the PMD of the m/c and the mapid is
                registered. 
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int pmd_map(mapid, port, protocol)
char *mapid; 
u_short port; 
u_short protocol;
{
   pmd_map_it out ;
   pmd_map_ot in ;
   int sd ;
   u_short op ;
				/* connect to PMD */
   if ((sd = get_socket()) == -1)
      return 0 ;
   if (!do_connect(sd, gethostid(), htons(PMD_PORT)))
    {
       close(sd) ;
       return 0 ;
    }
   op = htons(PMD_OP_MAP) ;	/* send MAP operation code */
   if (!writen(sd, (char *)&op, sizeof(op)))
    {
       close(sd) ;
       return 0 ;
    }				/* prepare data to be sent */
   strcpy(out.mapid, mapid) ;
   out.pid = htonl(getpid()) ;
   out.port = port ;
   out.protocol = htons(protocol) ;
				/* send mapping data to PMD */
   if (!writen(sd, (char *)&out, sizeof(out)))
    {
       close(sd) ;
       return 0 ;
    }				/* read PMD response */
   if (!readn(sd, (char *)&in, sizeof(in)))
    {
       close(sd) ;
       return 0 ;
    }
   close(sd) ;
   if (in.status == ntohs(SUCCESS))
      return 1 ;		/* indicate whether mapping suceeded/failed */
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int pmd_getmap(char *mapid, u_long host, u_short protocol)
  Parameters  : mapid    - logical name for which the port is needed
		host     - m/c in which PMD has to be connected
		protocol - udp/tcp
  Returns     : port # [or] -1 on error
  Calls       : get_socket, do_connect, writen, readn, htons, ntohs, close,
                strcpy
  Notes       : Connection is made to the PMD of the host specified and the
                port corresponding to mapid is obtained for the protocol.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int pmd_getmap(mapid, host, protocol)
char *mapid; 
u_long host; 
u_short protocol;
{
   pmd_getmap_it out ;
   pmd_getmap_ot in ;
   int sd ;
   u_short op ;
				/* connect to PMD */
   if ((sd = get_socket()) == -1)
      return 0 ;
   if (!do_connect(sd, host, htons(PMD_PORT)))
    {
       close(sd) ;
       return 0 ;
    }
   op = htons(PMD_OP_GET_MAP) ;	/* send GET_MAP operation code */
   if (!writen(sd, (char *)&op, sizeof(op)))
    {
       close(sd) ;
       return 0 ;
    }				/* prepare data to be sent */
   strcpy(out.mapid, mapid) ;
   out.protocol = htons(protocol) ;
				/* send mapid, protocol to PMD */
   if (!writen(sd, (char *)&out, sizeof(out)))
    {
       close(sd) ;
       return 0 ;
    }				/* read PMD response */
   if (!readn(sd, (char *)&in, sizeof(in)))
    {
       close(sd) ;
       return 0 ;
    }
   close(sd) ;			/* return the port got from PMD */
   if (in.status == ntohs(SUCCESS))
      return in.port ;
   return -1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int ping_pmd(u_long host)
  Parameters  : host - host in which PMD runs
  Returns     : 1 - PMD is up
                0 - PMD is down
  Calls       : get_socket, do_connect, writen, readn, htons, close, ntohs
  Notes       : Connection is made to the PMD of the m/c and checked whether
                it's running.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int ping_pmd(host)
u_long host;
{
   pmd_null_ot in ;
   int sd ;
   u_short op ;
   
   if ((sd = get_socket()) == -1)
      return 0 ;
   op = htons(PMD_OP_NULL) ;	/* send NULL operation code */
   if (do_connect(sd, host, htons(PMD_PORT)))
      if (writen(sd, (char *)&op, sizeof(op)))
	 if (readn(sd, (char *)&in, sizeof(in)))
	    if (ntohs(in.status) == SUCCESS)
	     {
		close(sd) ;
		return 1 ;
	     }
   close(sd) ;
   return 0 ;
}

