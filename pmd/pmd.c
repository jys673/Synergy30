/*.........................................................................*/
/*                     PMD.C ------> Port Mapper Daemon                    */
/*.........................................................................*/

#include "pmd.h"

/*---------------------------------------------------------------------------
  Prototype   : int main(void)
  Parameters  : -
  Returns     : Never returns
  Called by   : System
  Calls       : initOther, initSocket, start
  Notes       : The initialization routines and the main PMD function are 
                invoked.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification: February '13, updated by Justin Y. Shi
---------------------------------------------------------------------------*/  

int main()
{
   initOther() ;		/* initialise the data structures */
   initSocket() ;		/* create socket, bind */
   start() ;			/* accept connections/requests */
}


/*---------------------------------------------------------------------------
  Prototype   : void initOther(void)
  Parameters  : -
  Returns     : -
  Called by   : main
  Calls       : signal, fprintf, malloc, time, exit
  Notes       : Memory is allocated for PMD table and handler for SIGTERM is
                installed. When PMD has to be killed, it can be sent SIGTERM.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void initOther()
{
  int i ;
				/* to handle exit of PMD */
  if (signal(SIGTERM, sigtermHandler) == (void *)-1)
   {
      fprintf(stderr, "initOther::signal\n") ;
      exit(1) ;
   }				/* allocate for PMD table */
  if ((table = (table_t *)malloc(sizeof(table_t) * TABLE_SIZE)) == NULL)
    {
      fprintf(stderr, "initOther::malloc\n") ;
      exit(1) ;
    }				
  time(&start_time) ;		/* get PMD start time */
  for (i = 0 ; i < TABLE_SIZE ; i++)
    table[i].used = UNUSED ;	/* initialize table entries */
}


/*---------------------------------------------------------------------------
  Prototype   : void initSocket(void)
  Parameters  : -
  Returns     : -
  Called by   : main
  Calls       : get_socket, bind_socket, fprintf, exit
  Notes       : A well known port (PMD_PORT) is bound to a socket, to accept
                connections. 'oldsock' is the global variable in which PMD
		accepts connections.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void initSocket()
{				/* get socket to accept connections */
   if ((oldsock = get_socket()) == -1)
    {
       fprintf(stderr, "initSocket::get_socket\n") ;
       exit(1) ;
    }				
				/* bind socket to well known port*/
   if (!bind_socket(oldsock, (u_short)PMD_PORT))
    {
       fprintf(stderr, "PMD already running. \n") ;
       exit(1) ;
    }				
}  


/*---------------------------------------------------------------------------
  Prototype   : void start(void)
  Parameters  : -
  Returns     : -
  Called by   : main
  Calls       : get_connection, fprintf, exit, readn, close, ntohs, logOps,
                appropriate Op-function
  Notes       : This is the controlling function of PMD that invokes
                appropriate routine based on the operation requested.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void start()
{
   static void (*op_func[])() = {
      OpNull, OpMap, OpUnmap, OpGetMap, OpGetTable, OpPurgeTable, OpExit
      } ;
   
   while (TRUE)
    {				/* get a connection for request */
       if ((newsock = get_connection(oldsock, &peer)) == -1)
	{
	   fprintf(stderr, "start::get_connection\n") ;
	   exit(1) ;
	}	
				/* read operation code */
       if (!readn(newsock, (char *)&this_op, sizeof(this_op)))
	{
	   close(newsock) ;
	   continue ;
	}
       this_op = ntohs(this_op) ;
				/* validate operation & process */
       if (this_op >= PMD_OP_MIN && this_op <= PMD_OP_MAX)
	  (*op_func[this_op - PMD_OP_MIN])() ;
       else
	  this_op = PMD_OP_UNKNOWN ;
       logOps() ;
       close(newsock) ;
    }
}


/*---------------------------------------------------------------------------
  Prototype   : void OpNull(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : writen, htons, htonl
  Notes       : This operation is available to test whether the PMD is up.
                The # of entries in its table and also the time from when it 
		is up are returned.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpNull()
{
   pmd_null_ot out ;
   int i ;
   
   for (i = 0, out.count = 0 ; i < TABLE_SIZE ; i++)
      if (table[i].used == USED)
	 out.count++ ;		/* count the number of entries */

   out.status = htons(SUCCESS) ;  out.error = htons(PMD_ER_NOERROR) ;
   out.count = htons(out.count) ;
   out.start_time = htonl(start_time) ;
				/* return PMD started-time also */
   writen(newsock, (char *)&out, sizeof(out)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpMap(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : getSlot, readn, writen, ntohs, htons, ntohl, strcpy
  Notes       : The mapid, protocol, port, pid are stored in PMD table. 
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpMap()
{
  pmd_map_it in ;
  pmd_map_ot out ;
  int index ;

  if (!readn(newsock, (char *)&in, sizeof(in)))
     return ;			/* read input for operation */
  
  if ((index = getSlot(in.mapid, ntohs(in.protocol))) != -1)
   {
      table[index].used = USED ;
      strcpy(table[index].mapid, in.mapid) ;
      table[index].protocol = ntohs(in.protocol) ;
      table[index].port = in.port ; /* retain in NW order */
      table[index].pid = ntohl(in.pid) ;
      
      out.status = htons(SUCCESS) ; out.error = htons(PMD_ER_NOERROR) ;
      writen(newsock, (char *)&out, sizeof(out)) ;
      return ;
   }				/* store mapping in the table */

  out.status = htons(FAILURE) ; out.error = htons(PMD_ER_NOSPACE) ;
  writen(newsock, (char *)&out, sizeof(out)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpUnmap(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : unmapPid, unmapMapid, readn, writen, ntohl, ntohs, htons
  Notes       : If a pid > 0 is provided, then all entries corresponding
                to that pid is removed else entries for mapid are removed.
                The # of entries removed from PMD table is returned.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpUnmap()
{
  pmd_unmap_it in ;
  pmd_unmap_ot out ;
  
  if (!readn(newsock, (char *)&in, sizeof(in)))
     return ;			/* read input for operation */
  
  if (ntohl(in.pid) > 0)	/* remove all entries for pid */
     out.count = unmapPid(ntohl(in.pid)) ;
  else				/* remove entry for mapid */
     out.count = unmapMapid(in.mapid, ntohs(in.protocol)) ;
  
  if (out.count > 0)
   {
      out.status = htons(SUCCESS) ;
      out.error = htons(PMD_ER_NOERROR) ;
   }
  else
   {
      out.status = htons(FAILURE) ;
      out.error = htons(PMD_ER_NOENTRY) ;
   }
				/* return the # of entries unmapped */
  out.count = htons(out.count) ;
  writen(newsock, (char *)&out, sizeof(out)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpGetMap(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, htons, ntohs, strcmp
  Notes       : The port # corresponding to mapid, protocol is returned.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpGetMap()
{
  pmd_getmap_it in ;
  pmd_getmap_ot out ;
  int i ;

  if (!readn(newsock, (char *)&in, sizeof(in)))
     return ;			/* read input for operation */
/*
printf(" pmd: received (%s)\n", in.mapid);
*/

  for (i = 0 ; i < TABLE_SIZE ; i++)
   {
      if ((table[i].used == USED) && (!strcmp(table[i].mapid, in.mapid))
	  && (table[i].protocol == ntohs(in.protocol)))
       {
	  out.port = table[i].port ;
	  out.status = htons(SUCCESS) ; out.error = htons(PMD_ER_NOERROR) ;
/*
printf(" Pmd found a match. (%s) port(%d) \n",in.mapid, out.port);
*/

	  writen(newsock, (char *)&out, sizeof(out)) ;
	  return ;
       }			/* look for requested mapid+protocol */
   }
/*
printf(" pmd found no match. \n");
*/
  out.port = 0 ;		/* no match found in table */
  out.status = htons(FAILURE) ; out.error = htons(PMD_ER_NOENTRY) ;
  writen(newsock, (char *)&out, sizeof(out)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpGetTable(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : sendTable, writen, htons, ntohs
  Notes       : The # of entries in the table is first sent to the requestor
                so that resources can be allocated for the table.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpGetTable()
{
   pmd_gettable_ot1 out ;
   int i ;
   
   for (i = 0, out.count = 0 ; i < TABLE_SIZE ; i++)
      if (table[i].used == USED)
	 out.count++ ;		/* count number of entries in table */
   
   if (out.count > 0)
    {				/* entries present */
       out.status = htons(SUCCESS) ;
       out.error = htons(PMD_ER_NOERROR) ;
    }				
   else
    {				/* entries not present - table empty */
       out.status = htons(FAILURE) ;
       out.error = htons(PMD_ER_NOENTRY) ;
    }				
   out.count = htons(out.count) ;
   if (!writen(newsock, (char *)&out, sizeof(out)))
      return ;			/* send initial data - # of entries */
   
   if (ntohs(out.count) > 0)	/* send the entries in the table */
      sendTable() ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpPurgeTable(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : writen, htons
  Notes       : All entries in the table are deleted. The count of the 
                entries deleted is returned.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpPurgeTable()
{
   pmd_purgetable_ot out ;
   int i ;

   for (i = 0, out.count = 0 ; i < TABLE_SIZE ; i++)
      if (table[i].used == USED)
       {
	  out.count++ ;
	  table[i].used = UNUSED ;
       }			/* make all entries unused */

   out.status = htons(SUCCESS) ;  out.error = htons(PMD_ER_NOERROR) ;
   out.count = htons(out.count) ;
   writen(newsock, (char *)&out, sizeof(out)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpExit(void)
  Parameters  : -
  Returns     : -
  Called by   : pmd_start
  Calls       : free, writen, htons, exit
  Notes       : This request can possibly made only from SAC to terminate 
                the PMD.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpExit()
{
   pmd_exit_ot out ;
   
   free(table) ;
   out.status = htons(SUCCESS) ;  out.error = htons(PMD_ER_NOERROR) ;
   writen(newsock, (char *)&out, sizeof(out)) ;
   
   exit(0) ;
}


/*---------------------------------------------------------------------------
  Prototype   : int getSlot(char *mapid, u_short protocol)
  Parameters  : mapid    - logical name for the porto                
                protocol - tcp/udp used for communicating with the port
  Returns     : index into PMD table to store the mapping [or]
                -1 if mapping cannot be stored
  Called by   : OpMap
  Calls       : strcmp
  Notes       : If an entry exists already for the mapping it will be over-
                written.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int getSlot(mapid, protocol)
char *mapid; 
u_short protocol;
{
   int i, index ;
   
   for (i = 0, index = -1 ; i < TABLE_SIZE ; i++)
    {
       if ((table[i].used == UNUSED) && (index == -1))
	  index = i ;		/* look for an empty slot */
       
       if (!strcmp(table[i].mapid, mapid) && (table[i].protocol == protocol))
	{
	   index = i ;
	   break ;		/* check whether entry already present */
	}			/* to be over-written */
    }				
   return index ;
}


/*---------------------------------------------------------------------------
  Prototype   : int unmapPid(u_long pid)
  Parameters  : pid - pid for which all entries must be deleted
  Returns     : # of entries deleted from PMD table
  Called by   : OpUnmap
  Calls       : -
  Notes       : This is available to cleanup PMD table when a process
                terminates.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int unmapPid(pid)
u_long pid;
{
   int i, count ;

   for (i = 0, count = 0 ; i < TABLE_SIZE ; i++)
    {
       if ((table[i].used == USED) && (table[i].pid == pid))
	{
	   table[i].used = UNUSED ;
	   count++ ;		/* delete all entries matching pid */
	}
    }
   return count ;
}


/*---------------------------------------------------------------------------
  Prototype   : int unmapMapid(char *mapid, u_short protocol)
  Parameters  : mapid    - mapid of entry to be deleted
                protocol - tcp/udp
  Returns     : 1 - entry present and deleted
                0 - entry not present
  Called by   : OpUnmap
  Calls       : strcpy
  Notes       : An entry in the PMD table is identified by the 2- tuple
                <mapid, protocol>
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int unmapMapid(mapid, protocol)
char *mapid; 
u_short protocol;
{
   int i, count ;
   
   for (i = 0, count = 0 ; i < TABLE_SIZE ; i++)
    {
       if ((table[i].used == USED) && (!strcmp(table[i].mapid, mapid))
	   && (protocol == table[i].protocol))
	{			
	   table[i].used = UNUSED ;
	   count = 1 ;
	   break ;		/* delete entry matching parameters */
	}
    }
   return count ;
}


/*---------------------------------------------------------------------------
  Prototype   : void sendTable(void)
  Parameters  : -
  Returns     : - 
  Called by   : OpGetTable
  Calls       : writen, htons
  Notes       : All the entries <mapid, protocol, port> in the table are sent
                to the requestor.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void sendTable()
{
   pmd_gettable_ot2 out ;
   int i ;
   
   for (i = 0 ; i < TABLE_SIZE ; i++)
    {				/* send valid entries in the table */
       if (table[i].used == USED)
	{			
	   strcpy(out.mapid, table[i].mapid) ;
	   out.protocol = htons(table[i].protocol) ;
	   out.port = table[i].port ;
	   if (!writen(newsock, (char *)&out, sizeof(out)))
	      return ;
	}
    }			
}

  
/*---------------------------------------------------------------------------
  Prototype   : void sigtermHandler(void)
  Parameters  : -
  Returns     : -
  Called by   : System on SIGTERM
  Calls       : exit, free
  Notes       : The PMD can be terminated by sending SIGTERM signal.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void sigtermHandler()
{
   free(table) ;
   exit(0) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void logOps(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : fopen, fclose, fwrite, rename
  Notes       : The requested operation is logged in a file. When the file
                is full it is moved to a backup file and again entries start
		from the beginning.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void logOps()
{
   struct {
      u_short op ;
      u_long peer ;
      long tloc ;
   } rec ;
   static int ops_cnt = OPS_MAX ;
   FILE *fp ;
   
   rec.op = this_op ;
   rec.peer = peer ;
   time(&rec.tloc) ;

   if (ops_cnt == OPS_MAX)
    {				/* save old entries */
       rename(OPS_FILE, OPS_FILE_OLD) ;
       if ((fp = fopen(OPS_FILE, "w")) == NULL)
	  return ;
       ops_cnt = 0 ;
    }				
   else				/* if cannot append, over-write file */
      if ((fp = fopen(OPS_FILE, "a")) == NULL)
	 if ((fp = fopen(OPS_FILE, "w")) == NULL)
	    return ;
	 else
	    ops_cnt = 0 ;
   
   fwrite(&rec, sizeof(rec), 1, fp ) ;
   fclose(fp) ;
   ops_cnt++ ;
}
