/*.........................................................................*/
/*                     TSH.C ------> Tuple Space Handler                   */
/*.........................................................................*/

#include "tsh.h"
  
/*---------------------------------------------------------------------------
  Prototype   : int main(int argc, char **argv)
  Parameters  : argv[1] -  "-s" --> read initialization data from socket [or]
                           "-a" --> read initialization data from command line
		argv[2] -  socket descriptor #
		           application id
		argv[3] -  TSH name
  Returns     : Never returns 
  Called by   : System
  Calls       : initFromsocket, initFromline, start, strcmp, exit
  Notes       : TSH can be started either by CID or from the shell prompt
                by the user. In the former case, initialization data is
		read from the socket (from DAC). Otherwise, data is read
		from the command line. The switch -s or -a indicate the option.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification: Modified TSH to read appid, name from command line.
		February '13, updated by Justin Y. Shi
---------------------------------------------------------------------------*/  

int main(argc, argv)
int argc; 
char **argv ;
{
   if (argc < 3)
   {
      printf("Usage: tsh -[a|s] [appid objname|sock] \n");
      exit(1) ;
   }
				/* executed by DAC i.e. CID */
   if (!strcmp(argv[1], "-s"))
      initFromsocket(atoi(argv[2])) ;
				/* executed by user i.e. shell */
   else if (!strcmp(argv[1], "-a"))
    {
       if (argc < 4)
	  exit(1) ;
       if (argc > 4) initFromline2(argv[2], argv[3], atoi(argv[4])) ;
       else initFromline(argv[2], argv[3]) ;
    }
   else				/* invalid arguments */
      exit(1) ;
     
   start() ;
}


/*---------------------------------------------------------------------------
  Prototype   : void initFromline2(char *appid, char *name, int sock)
  Parameters  : appid - application id
                name  - TSH name
  Returns     : -
  Called by   : main
  Calls       : initCommon, strcpy, exit
  Notes       : Required initialization is performed when TSH is started by
		the user from shell prompt.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification: Modified to initialise from parameters & not from file.
---------------------------------------------------------------------------*/  

int initFromline2(appid, name, sd)
char *appid; 
char *name;
int sd;
{
   tsh_start_it in ;
   tsh_start_ot out ;
				/* read application id. & tuple space name */
   if (!readn(sd, (char *)&in, sizeof(tsh_start_it)))
   {
      close(sd);
      return(-1) ;
   }
   strcpy(tsh.appid, appid) ;
   strcpy(tsh.name, name) ;
   
   if (!initCommon())		/* perform other initializations */
    {
       out.port = htons(0) ;
       out.status = htons(FAILURE) ;  out.error = htons(TSH_ER_INSTALL) ;
       writen(sd, (char *)&out, sizeof(tsh_start_ot)) ;
       close(sd);
       return(-1) ;
    }
				/* report TSH port, status to DAC */
   out.status = htons(SUCCESS) ;  out.error = htons(TSH_ER_NOERROR) ;
   out.port = tsh.port ;
   if (!writen(sd, (char *)&out, sizeof(tsh_start_ot)))
    {
       close (sd);
/*
       unmapTshport() ;
*/
       return(-1) ;
    }
   close(sd) ;
}
/*---------------------------------------------------------------------------
  Prototype   : void initFromline(char *appid, char *name)
  Parameters  : appid - application id
                name  - TSH name
  Returns     : -
  Called by   : main
  Calls       : initCommon, strcpy, exit
  Notes       : Required initialization is performed when TSH is started by
		the user from shell prompt.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification: Modified to initialise from parameters & not from file.
---------------------------------------------------------------------------*/  

void initFromline(appid, name)
char *appid; 
char *name;
{
   strcpy(tsh.appid, appid) ;
   strcpy(tsh.name, name) ;
				/* perform other initializations */
   if (!initCommon())
      exit(1) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void initFromsocket(int sd)
  Parameters  : sd - socket descriptor of connection with DAC
  Returns     : -
  Called by   : main
  Calls       : initCommon, readn, writen, unmapTshport, strcpy, exit,htons
  Notes       : Required initialization is done when TSH is started by CID.
                Initialization data (application id, tuple space name) is read
		from DAC through the socket passed.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int initFromsocket(sd)
int sd;
{
   tsh_start_it in ;
   tsh_start_ot out ;
				/* read application id. & tuple space name */
   if (!readn(sd, (char *)&in, sizeof(tsh_start_it)))
   {
      close(sd);
      return(-1) ;
   }
   strcpy(tsh.appid, in.appid) ;
   strcpy(tsh.name, in.name) ;
   
   if (!initCommon())		/* perform other initializations */
    {
       out.port = htons(0) ;
       out.status = htons(FAILURE) ;  out.error = htons(TSH_ER_INSTALL) ;
       writen(sd, (char *)&out, sizeof(tsh_start_ot)) ;
       close(sd);
       return(-1) ;
    }
				/* report TSH port, status to DAC */
   out.status = htons(SUCCESS) ;  out.error = htons(TSH_ER_NOERROR) ;
   out.port = tsh.port ;
   if (!writen(sd, (char *)&out, sizeof(tsh_start_ot)))
    {
       close(sd);
/*
       unmapTshport() ;
*/
       return(-1) ;
    }
   close(sd) ;
}


/*---------------------------------------------------------------------------
  Prototype   : int initCommon(void)
  Parameters  : -
  Returns     : 1 - initialization success
                0 - initialization failed
  Called by   : initFromfile, initFromsocket
  Calls       : signal, getTshport, mapTshport
  Notes       : This function performs required initializations irrespective
                of how TSH is started (DAC/user). 
		'oldsock' is a global variable in which TSH connections are 
		accepted.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int initCommon()
{
   signal(SIGTERM, sigtermHandler);
				/* get a port to accept requests */
   if ((oldsock = getTshport()) == -1)
      return 0 ;
				/* map TSH port with PMD */
   if (!mapTshport())
      return 0 ;
				/* initialize tuple space & request queue */
   tsh.space = NULL ;
   tsh.retrieve = NULL;
   tsh.queue_hd = tsh.queue_tl = NULL ;
   
   return 1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : void start(void)
  Parameters  : -
  Returns     : -
  Called by   : main
  Calls       : get_connection, readn, close, ntohs, appropriate Op-function
  Notes       : This is the controlling function of TSH that invokes
                appropriate routine based on the operation requested.
		The same function 'OpGet' is invoked for both TSH_OP_GET 
		& TSH_OP_READ.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void start()
{
   static void (*op_func[])() = {OpPut, OpGet, OpGet, OpExit, OpRetrieve,
				 OpGethostInfo, OpTshSot, OpTshEot} ;
   
   while (TRUE)
    {				/* read operation on TSH port */
       if ((newsock = get_connection(oldsock, NULL)) == -1)
	{
	   unmapTshport() ;
	   exit(1) ;
	}
       if (!readn(newsock, (char *)&this_op, sizeof(u_short)))
	{
	   close(newsock) ;
	   continue ;
	}
				/* invoke function for operation */
       this_op = ntohs(this_op) ;

       if (this_op >= TSH_OP_MIN && this_op <= TSH_OP_MAX)
	  (*op_func[this_op - TSH_OP_MIN])() ;

       close(newsock) ;
    }
}


/*---------------------------------------------------------------------------
  Prototype   : void OpPut(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : createTuple, consumeTuple, storeTuple, readn, writen, 
                ntohs, ntohl, malloc, free
  Notes       : A tuple is created based on the data received. If there are
                pending requests for this tuple they are processed. If the
		tuple is not consumed by them (i.e. no GET) the tuple is
		stored in the tuple space.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpPut()
{
   tsh_put_it in ;
   tsh_put_ot out ;
   space1_t *s ;
   char *t ;

   out.error =  htons((short int)TSH_ER_NOERROR);
   out.status = htons((short int)SUCCESS);
				/* read tuple length, priority, name */
   if (!readn(newsock, (char *)&in, sizeof(tsh_put_it)))
      return ;
   in.proc_id = ntohl(in.proc_id);
   if (guardf(in.host, in.proc_id)) return;
				/* allocate memory for the tuple */

   if ((t = (char *)malloc(ntohl(in.length))) == NULL)
    {
       free(t) ;
       out.status = htons((short int)FAILURE) ;  
       out.error = htons((short int)TSH_ER_NOMEM) ;
       writen(newsock, (char *)&out, sizeof(tsh_put_ot)) ;
       return ;
    }				/* read the tuple */
   if (!readn(newsock, t, ntohl(in.length)))
    {
       free(t) ;
       return ;
    }				/* create and store tuple in space */
   s = createTuple(in.name, t, ntohl(in.length), ntohs(in.priority)) ;
   if (s == NULL)
    {
       free(t) ;
       out.status = htons((short int)FAILURE) ;  
       out.error = htons((short int)TSH_ER_NOMEM) ;
    }
   else
    {			/* satisfy pending requests, if possible */
       if (!consumeTuple(s)) {
	  out.error = htons(storeTuple(s, 0)) ;
       } else {
          out.error = htons((short int)TSH_ER_NOERROR) ;
       }
       out.status = htons((short int)SUCCESS) ; 
    }
   writen(newsock, (char *)&out, sizeof(tsh_put_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpGet(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : findTuple, deleteTuple, storeRequest, readn, writen, 
                strcpy, htons
  Notes       : This function is called for both TSH_OP_READ and TSH_OP_GET.
                If the tuple is present in the tuple space it is returned,
		or else the request is queued.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/

void OpGet()
{
   tsh_get_it in ;
   tsh_get_ot1 out1 ;
   tsh_get_ot2 out2 ;
   space1_t *s ;
   int request_len; 
				/* read tuple name */
   if (!readn(newsock, (char *)&in, sizeof(tsh_get_it)))
      return ;
   in.proc_id = ntohl(in.proc_id);
   if (guardf(in.host, in.proc_id)) return;
   request_len = ntohl(in.len); /* get user requested length */
				/* locate tuple in tuple space */
   if ((s = findTuple(in.expr)) == NULL)
    {
       out1.status = htons(FAILURE) ; 
       if (request_len != -1) /* -1: async read/get. Do not queue */
       { 
          if (!storeRequest(in))
	     out1.error = htons(TSH_ER_NOMEM) ;
          else
	     out1.error = htons(TSH_ER_NOTUPLE) ;
       } 
       writen(newsock, (char *)&out1, sizeof(tsh_get_ot1)) ;
       return ;
    }
				/* report that tuple exists */
   out1.status = htons(SUCCESS) ; out1.error = htons(TSH_ER_NOERROR) ;
   if (!writen(newsock, (char *)&out1, sizeof(tsh_get_ot1)))
      return ;
				/* send tuple name, length and priority */
   strcpy(out2.name, s->name) ;
   if ((s->length > request_len) && (request_len != 0)) 
        out2.length = in.len;
   else out2.length = htonl(s->length) ;
   out2.priority = htons(s->priority) ;
   if (!writen(newsock, (char *)&out2, sizeof(tsh_get_ot2)))
      return ;	
				/* send the tuple */
   if (!writen(newsock, s->tuple, ntohl(out2.length) /*s->length*/))
      return ;		

   if (this_op == TSH_OP_GET)
      deleteTuple(s, &in) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpExit(void)
  Parameters  : -
  Returns     : -
  Called by   : tsh_start
  Calls       : deleteSpace, deleteQueue, unmapTshport, writen, exit
  Notes       : This function clears up the tuple space when TSH_OP_EXIT
                is sent by DAC.
                This operation is received only when TSH is started by CID.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpExit()
{
   tsh_exit_ot out ;
				/* report successful TSH exit */
   out.status = htons(SUCCESS) ;  out.error = htons(TSH_ER_NOERROR) ;
   writen(newsock, (char *)&out, sizeof(tsh_exit_ot)) ;
   
   deleteSpace() ;		/* delete all tuples, requests */
   deleteQueue() ;
   
   unmapTshport() ;		/* unmap TSH port from PMD */
   exit(NORMAL_EXIT) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void deleteSpace(void)
  Parameters  : -
  Returns     : -
  Called by   : OpExit, sigtermHandler
  Calls       : free
  Notes       : This function frees all the memory associated with tuple
                space. It's invoked when the TSH has to exit.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void deleteSpace()
{
   space1_t *s ;
   space2_t *p_q;
   
   while (tsh.space != NULL)
    {
       s = tsh.space ;
       tsh.space = tsh.space->next ;
       free(s->tuple) ;		/* free tuple, tuple node */
       free(s) ;
    }
   while (tsh.retrieve != NULL) 
    {
       p_q = tsh.retrieve;
       tsh.retrieve = tsh.retrieve->next;
       free(p_q->tuple);
       free(p_q);
    }
}


/*---------------------------------------------------------------------------
  Prototype   : void deleteQueue(void)
  Parameters  : -
  Returns     : -
  Called by   : sigtermHandler, OpExit
  Calls       : free
  Notes       : This function frees all memory associated with the pending
                requests. It's invoked when the TSH has to exit.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void deleteQueue()
{
   queue1_t *q ;
   
   while (tsh.queue_hd != NULL)
    {
       q = tsh.queue_hd ;
       tsh.queue_hd = tsh.queue_hd->next ;
       free(q) ;		/* free request node */
    }		
}


/*---------------------------------------------------------------------------
  Prototype   : int consumeTuple(space1_t *s)
  Parameters  : s - pointer to tuple that has to be consumed
  Returns     : 1 - tuple consumed
                0 - tuple not consumed
  Called by   : OpPut
  Calls       : findRequest, sendTuple, deleteRequest
  Notes       : If there is a pending request that matches this tuple, it
                is sent to the requestor (served FIFO). If there were only
		pending TSH_OP_READs (or no pending requests) then the tuple 
		is considered not consumed. If a TSH_OP_GET was encountered
		then the tuple is considered consumed.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification: FSUN 10/94. Move the tuple consumed by a request to retrieve 
		list, For FDD.
---------------------------------------------------------------------------*/  

int consumeTuple(s)
space1_t *s;
{
   queue1_t *q ;
   space2_t *p_q;
				/* check whether request pending */
   if ((q = findRequest(s->name)) != NULL)
    {	
       do
	{		/* send tuple to requestor, delete request */
	   if (sendTuple(q, s) > 0) 
	      if (q->request == TSH_OP_GET)
	      {
		  deleteRequest(q) ;
                  /* add the tuple into backup queue. FSUN 10/94. */
                  p_q = tsh.retrieve;
                  while (p_q != NULL) {
                        if (p_q->host == q->host && 
				p_q->proc_id == q->proc_id) {
                          strcpy(p_q->name, s->name);
                          p_q->port = q->port;
			  p_q->cidport = q->cidport;  /* for dspace ys'96 */
/*
printf(" TSH captured host(%ul) port(%d) tpname(%s)\n", q->host, q->cidport,
			s->name);
*/
			  total_fetched ++; 
                          p_q->length = s->length;
                          p_q->priority = s->priority;
                          free(p_q->tuple);
                          p_q->tuple = s->tuple;
                          return 1;
                        }
                        p_q = p_q->next;
                  }
/*
printf(" TSH captured host(%ul) port(%d) tpname(%s)\n", q->host, q->cidport,
			s->name);
*/
		  total_fetched ++; 
/*
printf(" TSH. fetched (%d)\n", total_fetched);
*/
        	  p_q = (space2_t *)malloc(sizeof(space2_t));
        	  p_q->host = q->host;
        	  p_q->port = q->port;
		  p_q->cidport = q->cidport;  /* for dspace ys'96 */
/*
printf(" TSH captured new host(%ul) port(%d) tpname(%s)\n", q->host, q->cidport,
			s->name);
*/
        	  p_q->proc_id = q->proc_id;
        	  strcpy(p_q->name, s->name);
        	  p_q->length = s->length;
        	  p_q->priority = s->priority;
        	  p_q->fault = 0;
        	  p_q->tuple = s->tuple;
        	  p_q->next = tsh.retrieve;
        	  tsh.retrieve = p_q;
        	  free(s);
		  return 1 ;	/* tuple consumed */
	      }	
	      deleteRequest(q) ;
				/* check for another pending request */
	} while ((q = findRequest(s->name)) != NULL) ;
    }
   return 0 ;			/* tuple not consumed */
}


/*---------------------------------------------------------------------------
  Prototype   : space1_t *createTuple(char *name, u_long length, 
                                                           u_short priority)
  Parameters  : name     - tuple name
                length   - length of tuple
		priority - priority of the tuple
  Returns     : pointer to a tuple made of the input [or] NULL if no memory
  Called by   : OpPut
  Calls       : malloc, strcpy
  Notes       : This function creates a tuple and fills up the attributes.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

space1_t *createTuple(name, tuple, length, priority)
char *name; 
char *tuple; 
u_long length; 
u_short priority;
{
   space1_t *s ;
				/* create a new node and store tuple */
   if ((s = (space1_t *) malloc(sizeof(space1_t))) == NULL)
      return NULL ;
   strcpy(s->name, name) ;
   s->length = length ;
   s->tuple = tuple ;
   s->priority = priority ;
   
   return s ;			/* return new tuple */
}


/*---------------------------------------------------------------------------
  Prototype   : short int storeTuple(space1_t *s)
  Parameters  : s - pointer to tuple that has to be stored
  Returns     : -
  Called by   : OpPut
  Calls       : strcmp, free
  Notes       : The tuple is stored in the tuple space. If another tuple
                exists with the same name it is replaced.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification: Made FIFO from LIFO.
---------------------------------------------------------------------------*/  

short int storeTuple(s, f)
space1_t *s;
int f;
{  
   space1_t *ptr ;
				/* check if tuple already there */
   for (ptr = tsh.space ; ptr != NULL ; ptr = ptr->next)
    {
       if (!strcmp(ptr->name, s->name))
	{			/* overwrite existing tuple */
	   free(ptr->tuple) ;
	   ptr->tuple = s->tuple ;
	   ptr->length = s->length ;
	   ptr->priority = s->priority ;
	   free(s) ;
	   
	   return ((short int)TSH_ER_OVERRT) ;
	}
       if (ptr->next == NULL)
	  break ;
    }			
        if (f == 0) {           /* add tuple to end of space */
                s->next = NULL ;
                s->prev = ptr ;
                if (ptr == NULL)
                        tsh.space = s ;
                else
                        ptr->next = s ;
        } else {        /* add tuple retrieved to header of space */
                s->next = tsh.space;
                s->prev = NULL;
                tsh.space = s;
        }
   return((short int)TSH_ER_NOERROR);
}


/*---------------------------------------------------------------------------
  Prototype   : space1_t *findTuple(char *expr)
  Parameters  : expr - wildcard expression (*, ?)
  Returns     : pointer to tuple in the tuple space
  Called by   : OpGet
  Calls       : match
  Notes       : The tuple matching the wildcard expression & with the 
                highest priority of all the matches is determined and 
		returned.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

space1_t *findTuple(expr)
char *expr;
{
   space1_t *high = NULL, *s ;
				/* search tuple space for highest priority */
   for (s = tsh.space ; s != NULL ; s = s->next)
   {
      if (match(expr, s->name) && ((high == NULL) || 
				   (s->priority > high->priority)))
      {
	 high = s ;
      }
   }
   return high ;		/* return tuple or NULL if no match */
}


/*---------------------------------------------------------------------------
  Prototype   : void deleteTuple(space1_t *s)
  Parameters  : s - pointer to tuple to be deleted from tuple space
  Returns     : -
  Called by   : OpGet
  Calls       : free
  Notes       : The tuple is removed from the tuple space.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification: Added Fault Toloerance Function. FSUN 10/94.
---------------------------------------------------------------------------*/  
void deleteTuple(s, r)
space1_t *s;
tsh_get_it *r;
{
        space2_t *p_q;

        if (s == tsh.space)             /* remove tuple from space */
                tsh.space = s->next ;
        else
                s->prev->next = s->next ;

        if (s->next != NULL) s->next->prev = s->prev  ;

        /* add the tuple into backup queue. FSUN 10/94. */
        p_q = tsh.retrieve;
        while (p_q != NULL) {
                if (p_q->host == r->host && p_q->proc_id == r->proc_id) {
                        strcpy(p_q->name, s->name);
        		p_q->port = r->port;
			p_q->cidport = r->cidport;  /* for dspace. ys'96 */
/*
printf(" TSH: Captured host(%ul) port(%d) tpname(%s)\n", p_q->host,
			p_q->cidport, p_q->name);
*/
                        p_q->length = s->length;
                        p_q->priority = s->priority;
                        free(p_q->tuple);
                        p_q->tuple = s->tuple;
                        return;
                }
                p_q = p_q->next;
        }
        p_q = (space2_t *)malloc(sizeof(space2_t));
        p_q->host = r->host;
        p_q->port = r->port;
	p_q->cidport = r->cidport;   /* for dspace. ys'96 */
	p_q->proc_id = r->proc_id;
        strcpy(p_q->name, s->name);
/*
printf(" TSH: Captured new host(%ul) port(%d) tpname(%s)\n", p_q->host,
			p_q->cidport, p_q->name);
*/
        p_q->length = s->length;
        p_q->priority = s->priority;
        p_q->fault = 0;
        p_q->tuple = s->tuple;
	p_q->next = tsh.retrieve;
	tsh.retrieve = p_q; 
	free(s);
}


/*---------------------------------------------------------------------------
  Prototype   : int sendTuple(queue1_t *q, space1_t *s)
  Parameters  : q - pointer to pending request entry in the queue
                s - pointer to the tuple to be sent
  Returns     : 1 - tuple successfully sent
                0 - tuple not sent
  Called by   : consumeTuple
  Calls       : get_socket, do_connect, writen, close
  Notes       : The tuple is sent to the host/port specified in the request.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int sendTuple(q, s)
queue1_t *q; 
space1_t *s;
{
   tsh_get_ot2 out ;
   int sd ;
				/* connect to the requestor */
   if ((sd = get_socket()) == -1)
   {
	printf(" Cannot getsocket for tuple object. Call system operator. \n");
	exit (E_SOCKET);
   }

   if (!do_connect(sd, q->host, q->port))
    {
       close(sd) ;
       return (E_CONNECT);
    }
 				/* send tuple name, length, priority */
   strcpy(out.name, s->name) ;
   out.priority = htons(s->priority) ;
   out.length = htonl(s->length) ;
   if (!writen(sd, (char *)&out, sizeof(tsh_get_ot2)))
    {
       close(sd) ;
       return (E_CONNECT);
    }				/* send tuple data */
   if (!writen(sd, s->tuple, s->length))
    {
       close(sd) ;
       return (E_CONNECT);
    }
   close(sd);
   return 1 ;			/* tuple successfully sent */
}


/*---------------------------------------------------------------------------
  Prototype   : queue_t *findRequest(char *name)
  Parameters  : name - tuple name
  Returns     : pointer to a request for this tuple 
                NULL if no request for this tuple 
  Called by   : consumeTuple
  Calls       : match
  Notes       : If there is a pending request for the specified tuple, it is
                returned.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

queue1_t *findRequest(name)
char *name;
{	
   queue1_t *q ;
				/* search pending request queue */
   for (q = tsh.queue_hd ; q != NULL ; q = q->next)
   {
      if (match(q->expr, name))	
      {
	 return q ;		/* determine request that matches tuple */
      }
   }
   
   return NULL ;
}


/*---------------------------------------------------------------------------
  Prototype   : void deleteRequest(queue1_t *q)
  Parameters  : q - pointer to request in queue
  Returns     : -
  Called by   : OpPut
  Calls       : free
  Notes       : The specified request is removed from the pending requests
                queue.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void deleteRequest(q)
queue1_t *q;
{				/* remove request from queue */
   if (q == tsh.queue_hd)
      tsh.queue_hd = q->next ;
   else
      q->prev->next = q->next ;
   
   if (q == tsh.queue_tl)
      tsh.queue_tl = q->prev ;
   else
      q->next->prev = q->prev ;
   
   free(q) ;			/* free request */
}


/*---------------------------------------------------------------------------
  Prototype   : int storeRequest(tsh_get_it in)
  Parameters  : expr - name (wildcard expression) of the requested tuple
                host - address of the reque*stor
		port - port at which the tuple has to be delivered
		cidport - the cid of the requester's host 
  Returns     : 1 - request stored
                0 - no space to store request
  Called by   : OpGet
  Calls       : malloc, strcpy
  Notes       : A request based on the parameters is placed in the pending
                request queue.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification: FSUN 10/94. Added proc_id in the request stored for FDD.
---------------------------------------------------------------------------*/  

int storeRequest(in)
tsh_get_it in;
{
   queue1_t *q ;
				/* create node for request */
   if ((q = (queue1_t *)malloc(sizeof(queue1_t))) == NULL)
      return 0 ;
   strcpy(q->expr, in.expr) ;
   q->port = in.port ;
   q->cidport = in.cidport ;  /* for dspace. ys'96 */
   q->host = in.host ;
   q->proc_id = in.proc_id;
   q->request = this_op ;
   q->next = NULL ;
   q->prev = tsh.queue_tl ;
				/* store request in queue */
   if (tsh.queue_tl == NULL)
      tsh.queue_hd = q ;
   else
      tsh.queue_tl->next = q ;
   tsh.queue_tl = q ;
   return 1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int getTshport(void)
  Parameters  : -
  Returns     : socket descriptor of the port allocated to TSH
  Called by   : initCommon
  Calls       : get_socket, bind_socket, close
  Notes       : A socket is obtained and bound to a port. The TSH waits for
                requests on this port.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int getTshport()
{
   int sd ;
				/* obtain socket with required properties */
   if ((sd = get_socket()) == -1) 
      return -1 ;		/* bind socket to any port & obtain the port */
   if (!(tsh.port = bind_socket(sd, 0)))
    {
       close(sd) ;
       return -1 ;
    }				/* allow connections to TSH on this socket*/
   return sd ;			/* return socket for TSH */
}


/*---------------------------------------------------------------------------
  Prototype   : int match(char *expr, char *name)
  Parameters  : expr - wildcard expression
                name - string to be matched with the wildcard
  Returns     : 1 - expr & name match
                0 - expr & name do not match
  Called by   : match, findRequest, findTuple
  Calls       : match
  Notes       : The expression can contain wild card characters '*' and '?'.
                I like this function.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int match(expr, name)
char *expr; 
char *name;
{			
   while (*expr == *name)	/* skip as long as the strings match */
    {
       if (*name++ == '\0')
	  return 1 ;
       expr++ ;
    }			
   if (*expr == '?')
    {				/* '?' - skip one character */
       if (*name != '\0') 
	  return (match(++expr, ++name)) ;
    }			
   else if (*expr == '*')
    {
       expr++ ;
       do			/* '*' - skip 0 or more characters */
	{			/* try for each of the cases */
	   if (match(expr, name))
	      return 1 ;
	} while (*name++ != '\0') ;
    }
   return 0 ;			/* no match found */
}


/*---------------------------------------------------------------------------
  Prototype   : int mapTshport(void)
  Parameters  : -
  Returns     : 1 - if TSH port is mapped with PMD
                0 - if TSH port could not be mapped with PMD
  Called by   : initCommon
  Calls       : strcpy, strcat, sngd_map
  Notes       : The TSH port is registered with the PMD.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int mapTshport()
{

   strcpy(mapid, tsh.appid) ;
   strcat(mapid, "$") ;
   strcat(mapid, tsh.name) ;

   return sngd_map(mapid, tsh.port, mapid, (u_short)PMD_PROT_TCP) ;
}


/*---------------------------------------------------------------------------
  Prototype   : int unmapTshport(void)
  Parameters  : -
  Returns     : 1 - TSH port unregistered from PMD
                0 - TSH port not unregistered
  Called by   : sigtermHandler, OpExit, initFromsocket
  Calls       : get_socket, do_connect, ntohs, writen, readn, close, strcpy,
                strcat, htonl
  Notes       : The TSH port is unregistered from the PMD.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int unmapTshport()
{

   strcpy(mapid, tsh.appid) ;
   strcat(mapid, "$") ;
   strcat(mapid, tsh.name) ;
   
   return sngd_unmap(mapid, 0, mapid, (u_short)PMD_PROT_TCP) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void sigtermHandler(void)
  Parameters  : -
  Returns     : -
  Called by   : By system on SIGTERM
  Calls       : deleteSpace, deleteQueue, unmapTshport, exit
  Notes       : This function is invoked when TSH is terminated by the user
                i.e. when TSH is started by the user and not by CID. 
		This is the right way to kill TSH when it is started by user
		(i.e. by sending SIGTERM).
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void sigtermHandler()
{
   deleteSpace() ;		/* delete all tuples, requests */
   deleteQueue() ;
   
   unmapTshport() ;		/* unmap TSH port from PMD */
   exit(0) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpRetrieve()
  Parameters  : -
  Returns     : -
  Called by   :
  Calls       :
  Notes       :
  Date        : October '94
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void OpRetrieve()
{
        tsh_retrieve_it in;
        space1_t *s;
        space2_t *p_q, *p_p;
	int find = 0;

        if (!readn(newsock, (char *)&in, sizeof(tsh_retrieve_it)))
                return ;
	in.proc_id = ntohl(in.proc_id);
printf("TSH: Got OpRetrieve???? \n");
        p_q = tsh.retrieve;
        while (p_q != NULL) {
                if (p_q->host == in.host && p_q->proc_id == in.proc_id) {
                        s = createTuple(p_q->name, p_q->tuple, p_q->length, 
						p_q->priority);
                        if (!consumeTuple(s)) storeTuple(s, 1);
                        p_q->fault = 1;
			p_q->tuple = NULL;
			find = 1;
			return;
                }
                p_q = p_q->next;
        }
        p_p = (space2_t *)malloc(sizeof(space2_t));
        p_p->host = in.host;
	p_p->proc_id = in.proc_id;
        p_p->fault = 1;
        p_p->next = tsh.retrieve;
	tsh.retrieve = p_p;
}


/*---------------------------------------------------------------------------
  Prototype   : void guardf(u_long hostid, int procid)
  Parameters  : -
  Returns     : -
  Called by   :
  Calls       :
  Notes       :
  Date        : October '94
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

int guardf(hostid, procid)
u_long hostid;
int procid;
{
        space2_t *p_q;

        p_q = tsh.retrieve;
        while (p_q != NULL) {
                if (p_q->fault == 1 && p_q->host == hostid
				&& p_q->proc_id == procid)
                        return(1);
		p_q = p_q->next;
	}
        return(0);
}

/*---------------------------------------------------------------------------
  Prototype   : void OpTshSot()
  Parameters  : -
  Returns     : -
  Called by   : 
  Calls       :
  Notes       : 
  Date        : Nov.' 96 
  Coded by    : Yuan Shi 
  Modification: 
---------------------------------------------------------------------------*/

void OpTshSot()
{
/*
        space2_t *p_q, *m;

        p_q = tsh.retrieve;
        while (p_q != NULL) {
		m = p_q;
                p_q = p_q->next;
		free(m);
        }
	tsh.retrieve = NULL;
*/
	queue1_t *q, *p;

	q = tid_q;
	while (q!= NULL)
	{
		p = q;
		q = q->next;
		free (p);
	}
	EOT = 0;
	total_fetched = 0;
	TIDS = 0;
	tid_q = NULL;
	return;
}

/*---------------------------------------------------------------------------
  Prototype   : void OpTshEot()
  Parameters  : -
  Returns     : -
  Called by   : 
  Calls       :
  Notes       : Marks the end of task tuples. Receives total task count.
  Date        : Nov.' 96 
  Coded by    : Yuan Shi 
  Modification: 
---------------------------------------------------------------------------*/

void OpTshEot()
{
	tsh_retrieve_it in;
	if (!readn(newsock, (char *)&in, sizeof(tsh_retrieve_it))) return;
	TIDS = ntohl(in.proc_id);
	EOT = 1;
	/* check if any queued gts' */
	if (total_fetched == TIDS) send_tidinfo();
	return;
}

/*---------------------------------------------------------------------------
  Prototype   : void OpGethostInfo()
  Parameters  : -
  Returns     : -
  Called by   : 
  Calls       :
  Notes       : 
  Date        : Oct.' 96 
  Coded by    : Feijian Sun
  Modification: Revised to send all tids at once. Yuan Shi 11-24-96 
---------------------------------------------------------------------------*/

void OpGethostInfo()
{
        tsh_tidinfo_it in;
        tsh_tidinfo_ot out;
	tsh_get_it in2;
        space2_t *p_q;

	/* Send status */
/*
printf("TSH. GethostInfo. TIDS(%d) fetched(%d) EOT(%d)\n", TIDS, total_fetched, EOT);
*/
	out.status = EOT ? htons(TIDS): 0;
	if (total_fetched < TIDS) out.status = 0;
        if (!writen(newsock, (char *)&out, sizeof(tsh_tidinfo_ot)))
		return ;
	if (!out.status)
	{
		/* Read the return address */
		if (!readn(newsock, (char *)&in2, sizeof(tsh_get_it)))
			return;
		store_req(in2);
		return;
	} else  /* Send list of tids */ 
	{
		tid_send(newsock);
	} 
	return;
}

int store_req(in)
tsh_get_it in;
{
   queue1_t *q;

   if (( q = (queue1_t *) malloc(sizeof(queue1_t))) == NULL) return 0;
   q->port = in.port;
   q->host = in.host;
   q->next = tid_q;
   tid_q = q;
   return 1;
}


send_tidinfo()
{
  queue1_t *q;
  tsh_tidinfo_ot out;
  int sd;

  q = tid_q;
  while (q != NULL)
  {
	if ((sd = get_socket()) == -1)
	{
		perror("Tsh. Cannot get socket.\n");
		exit (E_SOCKET);
	}
	if (!do_connect(sd, q->host, q->port))
	{
		perror("Tsh. Cannot connect? \n");
		exit (E_SOCKET);
	}
	/* send total tids for queued requests */
	out.status = htons(TIDS);
	if (!writen(sd, (char *)&out, sizeof(tsh_tidinfo_ot))) return;
	tid_send(sd);
	q = q->next;
  }
  return;
}


tid_send(sd)
int sd;
{
  tsh_tidinfo_ot out;
  space2_t *p_q;

  p_q = tsh.retrieve;
  while (p_q != NULL) {
	out.status = 1;
	out.tid_host = (p_q->host);
	out.tid_cid_port = (p_q->cidport);
	strcpy(out.tid, p_q->name);

/*
printf(" TSH: TID (%s) host(%ul) port(%d)\n", p_q->name, p_q->host, p_q->cidport);
*/
       	if (!writen(sd, (char *)&out, sizeof(tsh_tidinfo_ot))) return ;
        p_q = p_q->next;
  }
  return;

}

