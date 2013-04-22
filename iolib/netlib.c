/*.........................................................................*/
/*               NETLIB.C ------> Network library functions                */
/*.........................................................................*/

#include "synergy.h"
#define TCP_NODELAY 0x1

/*---------------------------------------------------------------------------
  Prototype   : int get_socket(void)
  Parameters  : -
  Returns     : socket descriptor [or] -1 on error
  Calls       : socket, setsockopt, close
  Notes       : A socket is obtained and the following options are set :
                TCP_NODELAY - for TCP to send small packets as soon as 
		              possible
		SO_LINGER   - for system to deliver unsent data on close
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int get_socket()
{
   struct linger so_linger ;
   int sd, tcp_ndelay ;
				/* obtain a socket for TSH */
   if ((sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
      return -1 ;		/* do not delay packet transmission */
   tcp_ndelay = 1 ;
   if (setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (char *)&tcp_ndelay, 
		  sizeof(tcp_ndelay)) == -1)
    {
       close(sd) ;
       return -1 ;
    }				/* send untransmitted data when closed */
#if (!Ultrix)
   so_linger.l_onoff = 1 ;
   so_linger.l_linger = 60 ;	/* value > 0 --- # of secs ignored by call */
   if (setsockopt(sd, SOL_SOCKET, SO_LINGER, (char *)&so_linger, 
		  sizeof(so_linger)) == -1)
    {
       close(sd) ;
       return -1 ;
    }
#endif
   return sd ;			/* return socket created */
}


/*---------------------------------------------------------------------------
  Prototype   : u_short bind_socket(int sd, u_short port)
  Parameters  : sd   - socket descriptor that has to be bound to port
                port - port # [0 if any port]
  Returns     : port (or) 0 if error
  Calls       : bzero, bind, getsockname, listen
  Notes       : The socket descriptor is bound to the port specified or any 
                available port.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

u_short bind_socket(sd, port)
int sd; 
u_short port;
{
   struct sockaddr_in self ;
   int self_len ;
				/* bind socket to any port */
   bzero((char *) &self, sizeof(self)) ;
   self.sin_family = AF_INET ;
   self.sin_addr.s_addr = htonl(INADDR_ANY) ;
   self.sin_port = htons(port) ;
   if (bind(sd, (struct sockaddr *)&self, sizeof(self)) == -1)
      return 0 ;
				/* determine bound port */
   bzero((char *) &self, sizeof(self)) ;
   self_len = sizeof(self) ;
   if (getsockname(sd, (struct sockaddr *)&self, &self_len) == -1)
      return 0 ;
   if (listen(sd, 5) == -1)
      return 0 ;
   
   return self.sin_port ;	/* return port bound */
}


/*---------------------------------------------------------------------------
  Prototype   : int get_connection(int sock, u_long *peer_addr)
  Parameters  : sock      - socket on which to accept connection
                peer_addr - pointer to the location where connected address
		            is to be stored
  Returns     : new socket on connection (or) -1 on error
  Calls       : accept
  Notes       : This function waits for connection and once it is available
                it returns. 
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int get_connection(sock, peer_addr)
int sock; 
u_long *peer_addr;
{
   struct sockaddr_in peer ;
   int peerlen, sd ;
   
   while (TRUE)
    {
       peerlen = sizeof(peer) ;
       sd = accept(sock, (struct sockaddr *)&peer, &peerlen) ;
				/* if EINTR re-issue accept */
       if (sd == -1)
	{
	   if (errno == EINTR)
	      continue ;
	}
       else
	  if (peer_addr != NULL)
	     *peer_addr = peer.sin_addr.s_addr ;
       return sd ;
    }
}


/*---------------------------------------------------------------------------
  Prototype   : int do_connect(int sd, u_long host, u_short port)
  Parameters  : sd   - socket descriptor
                host - host to which to connect
		port - port to which to connect
  Returns     : 1 - connection established
                0 - connection not possible
  Calls       : bzero, connect
  Notes       : A connection is made to the specified host/port using the
                given socket descriptor.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int do_connect(sd, host, port)
int sd; 
u_long host; 
u_short port;
{
   struct sockaddr_in addr ;
				/* connect to the requesting host */
   bzero((char *)&addr, sizeof(addr)) ;
   addr.sin_family = AF_INET ;
   addr.sin_addr.s_addr = host ;
   addr.sin_port = port ;
   if (connect(sd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
      return 0 ;
   return 1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int readn(int sd, char *ptr, int nbytes)
  Parameters  : sd     - socket descriptor
                ptr    - pointer to location where read data has to be placed
		nbytes - # of bytes to be read
  Returns     : 1 - specified # of bytes read
                0 - specified # of bytes not read
  Calls       : read
  Notes       : The specified # of bytes are read. If an EOF is encountered
                or if the read fails, 0 is returned.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int readn(sd, ptr, nbytes)
int sd; 
char *ptr; 
int nbytes;
{
   int nleft, nread ;
   
   nleft = nbytes ;
   while (nleft > 0 )		/* keep reading until all data read */
    {
       nread = read(sd, ptr, nleft) ;
       if (nread <= 0)
	  return 0 ;
       nleft -= nread ;
       ptr += nread ;
    }
   return 1 ;			/* all data successfully read */
}


/*---------------------------------------------------------------------------
  Prototype   : int writen(int sd, char *ptr, int nbytes)
  Parameters  : sd     - socket descriptor
                ptr    - pointer to location from where data has to be written
		nbytes - # of bytes to be written
  Returns     : 1 - specified # of bytes written
                0 - specified # of bytes not written
  Calls       : write
  Notes       : The specified # of bytes are written. If an EOF is encountered
                or if the write fails, 0 is returned.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int writen(sd, ptr, nbytes)
int sd; 
char *ptr; 
int nbytes;
{
   int nleft, nwritten ;
   
   nleft = nbytes ;
   while (nleft > 0)		/* keep writing until all data sent */
    {
       nwritten = write(sd, ptr, nleft) ;
       if (nwritten <= 0)
	  return 0 ;
       nleft -= nwritten ;
       ptr += nwritten ;
    }
   return 1 ;			/* all data successfully written */
}

u_long sng_gethostid()
{
  struct hostent *host ;
  struct in_addr hostid ;
  char name[100] ;

  gethostname(name, sizeof(name)) ;
  if ((host = gethostbyname(name)) != NULL)
     return *(u_long *)(host->h_addr_list[0]) ;
  printf("gethostid failure (gethostbyname,gethostname)\n");
  exit(0);
}


/*---------------------------------------------------------------------------
  Prototype   : long getRealTuple(u_short r_this_op, 
				       char *appid,
                                       char *name, 
				       char *tpvalue)
  Parameters  : -
  Returns     : -
  Called by   : 
  Calls       :
  Notes       :
  Date        : April '96
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

int getRealTuple(r_this_op, appid, name, tpvalue)
u_short r_this_op;
char *appid;
char *name;
char *tpvalue;
{
   int sock, status;
   tsh_get_it out;
   tsh_get_ot1 in1, out1;
   tsh_get_ot2 in2;
   u_long r_hostid;
   int r_Tcidport;

   sscanf(tpvalue, "sng$bc$%d$%lu\0", &r_Tcidport, &r_hostid);

/*
printf(" getRtuple: name(%s) appid(%s) host(%ul) port(%d) buf(%s)\n", name,
        appid, r_hostid, r_Tcidport, tpvalue);
*/

   if (!(sock = get_socket()))
   {
        perror(" getRtuple: Cannot get socket. Exiting...\n");
        exit(0);
   }
   if (!do_connect(sock, r_hostid, r_Tcidport))
   {
        perror(" getRtuple: cannot connect to remote CID. lp0 ...\n");
        close(sock);
        exit(0);
   }
   if (!writen(sock, (char *)&r_this_op, sizeof(u_short)))
   {
        perror(" getRtuple: Cannot writen. lp01 ...\n");

        close(sock);
        exit(0);
   }
   strcpy(out.appid,appid);
   strcpy(out.expr,name);
   out.host = 0;
   out.port = 0;
   out.len = (0);      /* -1: async read/get. Do not queue */
   out.proc_id = (getpid());
   if (!writen(sock, (char *)&out, sizeof(tsh_get_it)))
   {
        perror(" getRtuple: erro writen. lp1 \n");
        close(sock);
        exit(0);
   }
   if (!readn(sock, (char *)&in1, sizeof(tsh_get_ot1)))
   {
        perror("getRtuple: error readn. status lp2...\n");
        close(sock);
        exit(0);
   }

/*
   printf(" getRtuple. Got back from remote CID. status(%d) (%d)\n",
                        ntohs(in1.status), SUCCESS);
*/
   if (ntohs(in1.status) != SUCCESS)
   {
        perror("getRealTuple Failure. Exitting...\n");
        close(sock);
        exit (0);
   }
                      /* read tuple details from another CID */
   if (!readn(sock, (char *)&in2, sizeof(tsh_get_ot2)))
   {
        perror("getRtuple: error readn. lp3...\n");
        close(sock);
        exit(0);
   }
   if (!readn(sock, tpvalue, ntohl(in2.length)))
   {
        perror("getRtuple: error readn (getRealTuple) loop... \n");
        close(sock);
        exit(0);
   }
/*
printf("getRtuple. len(%d) name(%s) actlen(%d) \n", ntohl(in2.length),
	in2.name, strlen(tpvalue));
*/


/*
printf("Obtained the real tuple (%s) appid(%s)\n", in2.name, in2.appid);
*/
   out1.status = htons(SUCCESS);
   out1.error = htons(TSH_ER_NOERROR) ;

   if (!writen(sock, (char *)&out1, sizeof(tsh_get_ot1)))
   {
        perror(" Error in dget delivery. tpname loop ...\n");
	close(sock);
	exit(0);
   }
   close(sock);
   if (tpvalue == NULL)            /* Failed to get real tuple */
   {
	   printf("CNF_DGET/READ:: (%lu) (%d) lied about (%s)\n",
			r_hostid, r_Tcidport, in2.name);
           exit (1);
   }
   return (ntohl(in2.length));
}
