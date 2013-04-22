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
   else                         /* invalid arguments */
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

   if (!initCommon())           /* perform other initializations */
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

   if (!initCommon())           /* perform other initializations */
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
                                 OpGethostInfo, OpTshSot, OpTshEot,
                                 advOpActivate, advOpCleanupQueue, advOpCleanupSpace,
                                 advOpSendData, advOpReadData,
                                 advOpSetToken, advOpGetToken, OpRmAll} ;
   while (TRUE)
    {                           /* read operation on TSH port */
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
    }                           /* read the tuple */
   if (!readn(newsock, t, ntohl(in.length)))
    {
       free(t) ;
       return ;
    }                           /* create and store tuple in space */
   s = createTuple(in.name, t, ntohl(in.length), ntohs(in.priority)) ;
   if (s == NULL)
    {
       free(t) ;
       out.status = htons((short int)FAILURE) ;
       out.error = htons((short int)TSH_ER_NOMEM) ;
    }
   else
    {                   /* satisfy pending requests, if possible */
       if (!consumeTuple(s)) out.error = htons(storeTuple(s, 0)) ;
       else out.error = htons((short int)TSH_ER_NOERROR) ;

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
          /* separate createRequest from storeRequest. FSun, 06/24/2003 */
          if (!storeRequest(createRequest(in)))
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

   deleteSpace() ;              /* delete all tuples, requests */
   deleteQueue() ;

   unmapTshport() ;             /* unmap TSH port from PMD */
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
       free(s->tuple) ;         /* free tuple, tuple node */
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
       free(q) ;                /* free request node */
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
        {                       /* send tuple to requestor, delete request */
           if (sendTuple(q, s) > 0)
           {
             /* 
              * For handling token tuples (excluding the special terminating tuple). 
              * FSun, 10/01/2003
              */
              if (q->request == TSH_OP_GET || 
                 (q->request == TSH_ADVANCED_OP_GET_TOKEN && !advHasIt(s->name, '!')))
              {
                  deleteRequest(q) ;
                  free(s->tuple);
                  free(s);
                  return 1 ;    /* tuple consumed */
              }
           }
           deleteRequest(q) ;
                                /* check for another pending request */
        } while ((q = findRequest(s->name)) != NULL) ;
    }
   return 0 ;                   /* tuple not consumed */
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
    }                           /* send tuple data */
   if (!writen(sd, s->tuple, s->length))
    {
       close(sd) ;
       return (E_CONNECT);
    }
   close(sd);
   return 1 ;                   /* tuple successfully sent */
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
         break ;
      }
   }
   return high ;                /* return tuple or NULL if no match */
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

        free(s->tuple);
        free(s);
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
         return q ;             /* determine request that matches tuple */
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
{                               /* remove request from queue */
   if (q == tsh.queue_hd)
      tsh.queue_hd = q->next ;
   else
      q->prev->next = q->next ;

   if (q == tsh.queue_tl)
      tsh.queue_tl = q->prev ;
   else
      q->next->prev = q->prev ;

   free(q) ;                    /* free request */
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
      return -1 ;               /* bind socket to any port & obtain the port */
   if (!(tsh.port = bind_socket(sd, 0)))
    {
       close(sd) ;
       return -1 ;
    }                           /* allow connections to TSH on this socket*/
   return sd ;                  /* return socket for TSH */
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
   while (*expr == *name)       /* skip as long as the strings match */
    {
       if (*name++ == '\0')
          return 1 ;
       expr++ ;
    }
   if (*expr == '?')
    {                           /* '?' - skip one character */
       if (*name != '\0')
          return (match(++expr, ++name)) ;
    }
   else if (*expr == '*')
    {
       expr++ ;
       do                       /* '*' - skip 0 or more characters */
        {                       /* try for each of the cases */
           if (match(expr, name))
              return 1 ;
        } while (*name++ != '\0') ;
    }
   return 0 ;                   /* no match found */
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
   deleteSpace() ;              /* delete all tuples, requests */
   deleteQueue() ;

   unmapTshport() ;             /* unmap TSH port from PMD */
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

   if (( q = (queue1_t *)malloc(sizeof(queue1_t))) == NULL) return 0;
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


/*
 * ==========================================================================
 */


/*---------------------------------------------------------------------------
  Prototype   : space1_t *createTuple(char *name, char *t, u_long len, u_short p)
---------------------------------------------------------------------------*/

space1_t *createTuple(name, t, len, p)
char *name;
char *t;
u_long len;
u_short p;
{
   space1_t *s ;

   char var_name[TUPLENAME_LEN];
   char workerid[TUPLENAME_LEN];
   char sclr_opt[TUPLENAME_LEN];
   int  tokens;
                                /* create a new node and store tuple */
   if ((s = (space1_t *)malloc(sizeof(space1_t))) == NULL)
      return NULL ;
   strcpy(s->name, name) ;
   s->length = len ;
   s->tuple = t ;
   s->priority = p ;

   s->next = NULL ;
   s->prev = NULL ;

   if (advHasIt(name, ':') && 
       advHasIt(name, '?') && 
       advHasIt(name, '@'))
    {
       sscanf(name, "%[^#]#%[^?]?%[^@]@%d", 
              var_name, workerid, sclr_opt, &tokens) ;
       s->tokens = tokens ;
       s->scalar = 0 ;
    }

   return s ;                   /* return new tuple */
}


/*---------------------------------------------------------------------------
  Prototype   : short int storeTuple(space1_t *s)
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
        {
           if (advHasIt(s->name, ':') && 
               advHasIt(s->name, '?') && 
               advHasIt(s->name, '@'))
            {
               advScalar(ptr, s) ;
            }
                                /* replace existing tuple */
           if (ptr == tsh.space)
               tsh.space = s ;
           s->next = ptr->next;
           if (ptr->next != NULL)
               ptr->next->prev = s;
           s->prev = ptr->prev;
           if (ptr->prev != NULL)
           ptr->prev->next = s;

           ptr->next = NULL;
           ptr->prev = NULL;
           free(ptr->tuple) ;
           free(ptr) ;

           return ((short int)TSH_ER_OVERRT) ;
        }

       if (ptr->next == NULL) break ;
    }
   if (f == 0) {                /* add tuple to end of space */
       s->next = NULL ;
       s->prev = ptr ;
       if (ptr == NULL) tsh.space = s ;
       else ptr->next = s ;
   } else {
       s->next = tsh.space;
       s->prev = NULL;
       tsh.space = s;
   }

   return((short int)TSH_ER_NOERROR);
}


/*---------------------------------------------------------------------------
  Prototype   : queue1_t *createRequest(tsh_get_it in)
---------------------------------------------------------------------------*/

queue1_t *createRequest(in)
tsh_get_it in;
{
   queue1_t *q ;
                                /* create node for request */
   if ((q = (queue1_t *)malloc(sizeof(queue1_t))) == NULL)
      return NULL ;
   strcpy(q->expr, in.expr) ;
   q->port = in.port ;
   q->cidport = in.cidport ;    /* for dspace. ys'96 */
   q->host = in.host ;
   q->proc_id = in.proc_id;
   q->request = this_op ;

   q->next = NULL ;
   q->prev = NULL ;
   q->full_to_full = 0 ;
   q->reqLen = ntohl(in.len) ;
   q->curLen = 0 ;
/*
   if (q->reqLen > 0)
   {
       if ((q->data = (char *) malloc(q->reqLen)) == NULL)
       {
          free(q) ;
          return NULL ;
       }
   }
*/
   q->data = NULL ;

   return q ;
}


/*---------------------------------------------------------------------------
  Prototype   : int storeRequest(queue1_t *q)
---------------------------------------------------------------------------*/

int storeRequest(q)
queue1_t *q;
{
   if (q == NULL) return 0 ;
                                /* store request in queue */
   q->prev = tsh.queue_tl ;
   if (tsh.queue_tl == NULL)
      tsh.queue_hd = q ;
   else
      tsh.queue_tl->next = q ;
   tsh.queue_tl = q ;

   return 1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : void deleteTerminator(char *workerid)
---------------------------------------------------------------------------*/

void deleteTerminator(workerid)
char *workerid ;
{
   char *token_name ;
   space1_t *s ;

   token_name  = (char *)malloc(TUPLENAME_LEN) ;
   sprintf(token_name, "token#%s!", workerid);
   /* Find and remove the special terminating tuple for this workerid */
   if ((s = findTuple(token_name)) != NULL)
    {
       if (s == tsh.space)
           tsh.space = s->next ;
       else
           s->prev->next = s->next ;
       if (s->next!= NULL)
           s->next->prev = s->prev ;
       free(s) ;
    }
   free(token_name) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void advOpActivate(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : 
  Notes       : 
  Date        : June '03
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void advOpActivate()
{
   tsh_put_it in ;
   tsh_put_ot out ;
                                /* send tuple name, priority, length */
   if (!readn(newsock, (char *)&in, sizeof(tsh_put_it)))
    {
       return ;
    }
   in.proc_id  = ntohl(in.proc_id);
   if (guardf(in.host, in.proc_id))
    {
       return ;
    }
   out.error  = htons((short int)TSH_ER_NOERROR) ;
   out.status = htons((short int)SUCCESS) ;
   writen(newsock, (char *)&out, sizeof(tsh_put_ot)) ;

   deleteTerminator(in.name) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void advOpCleanupQueue(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : 
  Notes       : 
  Date        : June '03
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void advOpCleanupQueue()
{
   tsh_put_it in ;
   tsh_put_ot out ;

   char *token_name ;
   space1_t *s ;
                                /* send tuple name, priority, length */
   if (!readn(newsock, (char *)&in, sizeof(tsh_put_it)))
    {
       return ;
    }
   in.proc_id  = ntohl(in.proc_id);
   if (guardf(in.host, in.proc_id))
    {
       return ;
    }
   out.error  = htons((short int)TSH_ER_NOERROR) ;
   out.status = htons((short int)SUCCESS) ;
   writen(newsock, (char *)&out, sizeof(tsh_put_ot)) ;

   token_name  = (char *)malloc(TUPLENAME_LEN) ;
   sprintf(token_name, "token#%s!", in.name);

   /* Create a special terminating tuple for this workerid */
   s = createTuple(token_name, "!", 4, 1) ;
   if (s == NULL)
    {
       free(token_name) ;
       exit(TSH_ER_NOMEM) ;
    }

   if (!consumeTuple(s)) storeTuple(s, 0) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void advOpCleanupSpace(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : 
  Notes       : 
  Date        : June '03
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void advOpCleanupSpace()
{
   tsh_put_it in ;
   tsh_put_ot out ;

   char wid[100], *var, *sid ;
   space1_t *s, *t ;
                                /* send tuple name, priority, length */
   if (!readn(newsock, (char *)&in, sizeof(tsh_put_it)))
    {
       return ;
    }
   in.proc_id  = ntohl(in.proc_id);
   if (guardf(in.host, in.proc_id))
    {
       return ;
    }
   out.error  = htons((short int)TSH_ER_NOERROR) ;
   out.status = htons((short int)SUCCESS) ;
   writen(newsock, (char *)&out, sizeof(tsh_put_ot)) ;

   sprintf(wid, "%s*", in.name) ;

   s = tsh.space ;
   var = (char *)malloc(100) ;
   sid = (char *)malloc(TUPLENAME_LEN) ;
   while (s)
   {
       sscanf(s->name, "%[^#]#%s", var, sid) ;
       if (match(wid, sid))
        {
           t = s ;
           s = t->next ;

           if (t == tsh.space)
               tsh.space = t->next ;
           else
               t->prev->next = t->next ;
           if (t->next!= NULL)
               t->next->prev = t->prev ;

           free(t->tuple) ;
           free(t) ;
        }
       else
        {
           s = s->next ;
        }
    }
   free(var) ;
   free(sid) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void advOpSendData(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : 
  Notes       : 
  Date        : June '03
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void advOpSendData()
{
   tsh_put_it in ;
   tsh_put_ot out ;
   space1_t *s ;
   char *t ;
                                /* send tuple name, priority, length */
   if (!readn(newsock, (char *)&in, sizeof(tsh_put_it)))
    {
       return ;
    }
   in.proc_id  = ntohl(in.proc_id);
   if (guardf(in.host, in.proc_id))
    {
       return ;
    }
                                /* allocate memory for the tuple */
   if ((t = (char *)malloc(ntohl(in.length))) == NULL)
    {
       free(t) ;
       out.error  = htons((short int)TSH_ER_NOMEM) ;
       out.status = htons((short int)FAILURE) ;
       writen(newsock, (char *)&out, sizeof(tsh_put_ot)) ;
       return ;
    }
                                /* read the tuple */
   if (!readn(newsock, t, ntohl(in.length)))
    {
       free(t) ;
       return ;
    }
                                /* create a tuple */
   s = createTuple(in.name, t, ntohl(in.length), ntohs(in.priority)) ;
   if (s == NULL)
    {
       free(t) ;
       out.error  = htons((short int)TSH_ER_NOMEM) ;
       out.status = htons((short int)FAILURE) ;
       writen(newsock, (char *)&out, sizeof(tsh_put_ot)) ;
       return ;
    }
   out.error  = htons(storeTuple(s, 0)) ;
   out.status = htons((short int)SUCCESS) ;
   writen(newsock, (char *)&out, sizeof(tsh_put_ot)) ;

   advFindRequest(s) ;          /* find and satisfy pending requests */
}


/*---------------------------------------------------------------------------
  Prototype   : void advOpReadData(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : 
  Notes       : 
  Date        : June '03
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void advOpReadData()
{
   tsh_get_it  in ;
   tsh_get_ot1 out1 ;
   tsh_get_ot2 out2 ;
   queue1_t *q;
                                /* read the name of requested tuple */
   if (!readn(newsock, (char *)&in, sizeof(tsh_get_it)))
    {
       return ;
    }
   in.proc_id  = ntohl(in.proc_id);
   if (guardf(in.host, in.proc_id))
    {
       return ;
    }
                                /* create a request */
   q = createRequest(in) ;
   if (q == NULL)
    {
       out1.error = htons(TSH_ER_NOMEM) ;
       out1.status = htons(FAILURE) ;
       writen(newsock, (char *)&out1, sizeof(tsh_get_ot1)) ;
       return ;
    }

   advFindTuple(q) ;            /* find all matching tuples */

   if (q->curLen >= q->reqLen)
    {
       out1.error = htons(TSH_ER_NOERROR) ;
       out1.status = htons(SUCCESS) ;
       writen(newsock, (char *)&out1, sizeof(tsh_get_ot1)) ;

       strcpy(out2.name, q->expr) ;
       out2.priority = htons(q->priority) ;
       out2.length = htonl(q->reqLen) ;
       writen(newsock, (char *)&out2, sizeof(tsh_get_ot2)) ;

       writen(newsock, q->data , q->reqLen) ;

       if (q->full_to_full == 0) free(q->data) ;
       else q->data == NULL ;
       free(q) ;
       return ;
    }

   if (storeRequest(q))
    {
       out1.error = htons(TSH_ER_NOTUPLE) ;
    }
   else
    {
       out1.error = htons(TSH_ER_NOMEM) ;
    }
   out1.status = htons(FAILURE) ;
   writen(newsock, (char *)&out1, sizeof(tsh_get_ot1)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void advOpSetToken(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : 
  Notes       : 
  Date        : June '03
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void advOpSetToken()
{
   tsh_put_it in ;
   tsh_put_ot out ;
   char *t ;
   int dimmCtr;
                                /* send tuple name, priority, length */
   if (!readn(newsock, (char *)&in, sizeof(tsh_put_it)))
    {
       return ;
    }
   in.proc_id  = ntohl(in.proc_id);
   if (guardf(in.host, in.proc_id))
    {
       return ;
    }
                                /* allocate memory for the tuple */
   if ((t = (char *)malloc(ntohl(in.length))) == NULL)
    {
       free(t) ;
       out.error  = htons((short int)TSH_ER_NOMEM) ;
       out.status = htons((short int)FAILURE) ;
       writen(newsock, (char *)&out, sizeof(tsh_put_ot)) ;
       return ;
    }
                                /* read the tuple */
   if (!readn(newsock, t, ntohl(in.length)))
    {
       free(t) ;
       return ;
    }

   out.error  = htons((short int)TSH_ER_NOMEM) ;
   dimmCtr= advDimms(t, '(');  /* create the token tuples */
   switch (dimmCtr)
    {
       case 0:
           printf("Error: when dimmCtr == 0, it shouldn't get here at all!\n");
           break;
       case 1:
           out.error = htons(createTokenTuples1(in, t));
           break;
       case 2:
           out.error = htons(createTokenTuples2(in, t));
           break;
       case 3:
           printf("Error: function createTokenTuples3 is yet to be built!\n");
           break;
       default:
           printf("Error: function does NOT exist for dimmCtr=%d!\n", dimmCtr);
           break;
    }
   out.status = htons((short int)SUCCESS) ;
   writen(newsock, (char *)&out, sizeof(tsh_put_ot)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void advOpGetToken(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : 
  Notes       : 
  Date        : June '03
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

void advOpGetToken()
{
   tsh_get_it  in ;
   tsh_get_ot1 out1 ;
   tsh_get_ot2 out2 ;
   space1_t *s ;
                                /* read the name of requested tuple */
   if (!readn(newsock, (char *)&in, sizeof(tsh_get_it)))
    {
       return ;
    }
   in.proc_id  = ntohl(in.proc_id);
   if (guardf(in.host, in.proc_id))
    {
       return ;
    }
                                /* locate tuple in tuple space */
   strcat(in.expr, "*");
   if ((s = findTuple(in.expr)) != NULL)
    {
       out1.error = htons(TSH_ER_NOERROR) ;
       out1.status = htons(SUCCESS) ;
       writen(newsock, (char *)&out1, sizeof(tsh_get_ot1)) ;

       strcpy(out2.name, s->name) ;
       out2.priority = htons(s->priority) ;
       out2.length = htonl(s->length) ;
       writen(newsock, (char *)&out2, sizeof(tsh_get_ot2)) ;

       writen(newsock, s->tuple, s->length) ;

       /* 
        * Removing a matched token tuple unless it is the special terminating tuple.
        */
       if (!advHasIt(s->name, '!')) deleteTuple(s, &in) ;

       return ;
    }
   if (storeRequest(createRequest(in)))
    {
       out1.error = htons(TSH_ER_NOTUPLE) ;
    }
   else
    {
       out1.error = htons(TSH_ER_NOMEM) ;
    }
   out1.status = htons(FAILURE) ;
   writen(newsock, (char *)&out1, sizeof(tsh_get_ot1)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : int advFindRequest(space1_t *s)
---------------------------------------------------------------------------*/

int advFindRequest(s)
space1_t *s;
{
   queue1_t *q, *t ;
   int dimmCtr ;
                                /* check whether request pending */
   q = tsh.queue_hd ;
   while (q)
   {
       if (!advMatch(q->expr, s->name))
        {
           q = q->next ;
           continue;
        }

       q->priority= s->priority;
       if (advHasIt(s->name, ':') && advHasIt(s->name, '?') && advHasIt(s->name, '@'))
        {
           if (q->data == NULL)
            {
               if ((q->data = (char *) malloc(q->reqLen)) == NULL)
                {
                   free(q) ;
                   exit(-1) ;
                }
            }
/*
           if (s->scalar >= s->tokens)
            {
*/
               memcpy(q->data, s->tuple, s->length);
               q->curLen = s->length;
/*
            }
*/
        }
       else
       if (advIsFull(s->name))
        {
           if (advIsFull(q->expr))
            {
               /* full tuple => full request */
               q->curLen = s->length;
/*
               memcpy(q->data, s->tuple, s->length);
*/
               q->data = s->tuple ;
               q->full_to_full = 1 ;
            }
           else
            {
               if (q->data == NULL)
                {
                   if ((q->data = (char *) malloc(q->reqLen)) == NULL)
                    {
                       free(q) ;
                       exit(-1) ;
                    }
                }
               /* full tuple => part request */
               q->curLen = q->reqLen;
               dimmCtr= advDimms(q->expr, '[');
               switch (dimmCtr)
                {
                   case 0:
                       printf("Error: when dimmCtr == 0, it shouldn't get here at all!\n");
                       break;
                   case 1:
                       FullTupleToPartReq1(s, q);
                       break;
                   case 2:
                       FullTupleToPartReq2(s, q);
                       break;
                   case 3:
                       printf("Error: function FullTupleToPartReq3 is yet to be built!\n");
                       break;
                   default:
                       printf("Error: function does NOT exist for dimmCtr=%d!\n", dimmCtr);
                       break;
                }
            }
        }
       else
        {
           if (advIsFull(q->expr))
            {
               if (q->data == NULL)
                {
                   if ((q->data = (char *) malloc(q->reqLen)) == NULL)
                    {
                       free(q) ;
                       exit(-1) ;
                    }
                }
               /* part tuple => full request */
               q->curLen += s->length;
               dimmCtr= advDimms(s->name, '[');
               switch (dimmCtr)
                {
                   case 0:
                       printf("Error: when dimmCtr == 0, it shouldn't get here at all!\n");
                       break;
                   case 1:
                       PartTupleToFullReq1(s, q);
                       break;
                   case 2:
                       PartTupleToFullReq2(s, q);
                       break;
                   case 3:
                       printf("Error: function PartTupleToFullReq3 is yet to be built!\n");
                       break;
                   default:
                       printf("Error: function does NOT exist for dimmCtr=%d!\n", dimmCtr);
                       break;
                }
            }
           else
            {
               if (q->data == NULL)
                {
                   if ((q->data = (char *) malloc(q->reqLen)) == NULL)
                    {
                       free(q) ;
                       exit(-1) ;
                    }
                }
               /* part tuple => part request */
               dimmCtr= advDimms(s->name, '[');
               switch (dimmCtr)
                {
                   case 0:
                       printf("Error: when dimmCtr == 0, it shouldn't get here at all!\n");
                       break;
                   case 1:
                       PartTupleToPartReq1(s, q);
                       break;
                   case 2:
                       PartTupleToPartReq2(s, q);
                       break;
                   case 3:
                       printf("Error: function PartTupleToPartReq3 is yet to be built!\n");
                       break;
                   default:
                       printf("Error: function does NOT exist for dimmCtr=%d!\n", dimmCtr);
                       break;
                }
            }
        }

       if (q->curLen >= q->reqLen)
        {
           advResolveRequest(q) ;
                                /* delete the resolved request */
           t = q ;
           q = t->next ;

           if (t == tsh.queue_hd)
              tsh.queue_hd = t->next ;
           else
              t->prev->next = t->next ;
           if (t == tsh.queue_tl)
              tsh.queue_tl = t->prev ;
           else
              t->next->prev = t->prev ;

           if (t->full_to_full == 0) free(t->data) ;
           else t->data == NULL ;
           free(t) ;
        }
       else
        {
           q = q->next ;
        }
    }
   return 0 ;                   /* tuple not consumed */
}


/*---------------------------------------------------------------------------
  Prototype   : int advFindTuple(queue1_t *q)
---------------------------------------------------------------------------*/

int advFindTuple(q)
queue1_t *q;
{
   space1_t *s ;
   int dimmCtr ;
                                /* search tuple space */
   s = tsh.space ;
   while (s)
   {
       if (!advMatch(q->expr, s->name))
        {
           s = s->next ;
           continue;
        }

       q->priority= s->priority;
       if (advHasIt(q->expr, ':') && advHasIt(q->expr, '?') && advHasIt(q->expr, '@'))
        {
           if (q->data == NULL)
            {
               if ((q->data = (char *) malloc(q->reqLen)) == NULL)
                {
                   free(q) ;
                   exit(-1) ;
                }
            }
/*
           if (s->scalar >= s->tokens)
            {
*/
               memcpy(q->data, s->tuple, s->length);
               q->curLen = s->length;
/*
            }
*/
        }
       else
       if (advIsFull(q->expr))
        {
           if (advIsFull(s->name))
            {
               /* full request <= full tuple */
               q->curLen = s->length;
/*
               memcpy(q->data, s->tuple, s->length);
*/
               q->data = s->tuple ;
               q->full_to_full = 1 ;
            }
           else
            {
               if (q->data == NULL)
                {
                   if ((q->data = (char *) malloc(q->reqLen)) == NULL)
                    {
                       free(q) ;
                       exit(-1) ;
                    }
                }
               /* full request <= part tuple */
               q->curLen += s->length;
               dimmCtr= advDimms(s->name, '[');
               switch (dimmCtr)
                {
                   case 0:
                       printf("Error: when dimmCtr == 0, it shouldn't get here at all!\n");
                       break;
                   case 1:
                       PartTupleToFullReq1(s, q);
                       break;
                   case 2:
                       PartTupleToFullReq2(s, q);
                       break;
                   case 3:
                       printf("Error: function PartTupleToFullReq3 is yet to be built!\n");
                       break;
                   default:
                       printf("Error: function does NOT exist for dimmCtr=%d!\n", dimmCtr);
                       break;
                }
            }
        }
       else
        {
           if (advIsFull(s->name))
            {
               if (q->data == NULL)
                {
                   if ((q->data = (char *) malloc(q->reqLen)) == NULL)
                    {
                       free(q) ;
                       exit(-1) ;
                    }
                }
               /* part request <= full tuple */
               q->curLen = q->reqLen;
               dimmCtr= advDimms(q->expr, '[');
               switch (dimmCtr)
                {
                   case 0:
                       printf("Error: when dimmCtr == 0, it shouldn't get here at all!\n");
                       break;
                   case 1:
                       FullTupleToPartReq1(s, q);
                       break;
                   case 2:
                       FullTupleToPartReq2(s, q);
                       break;
                   case 3:
                       printf("Error: function FullTupleToPartReq3 is yet to be built!\n");
                       break;
                   default:
                       printf("Error: function does NOT exist for dimmCtr=%d!\n", dimmCtr);
                       break;
                }
            }
           else
            {
               if (q->data == NULL)
                {
                   if ((q->data = (char *) malloc(q->reqLen)) == NULL)
                    {
                       free(q) ;
                       exit(-1) ;
                    }
                }
               /* part request <= part tuple */
               dimmCtr= advDimms(q->expr, '[');
               switch (dimmCtr)
                {
                   case 0:
                       printf("Error: when dimmCtr == 0, it shouldn't get here at all!\n");
                       break;
                   case 1:
                       PartTupleToPartReq1(s, q);
                       break;
                   case 2:
                       PartTupleToPartReq2(s, q);
                       break;
                   case 3:
                       printf("Error: function PartTupleToPartReq3 is yet to be built!\n");
                       break;
                   default:
                       printf("Error: function does NOT exist for dimmCtr=%d!\n", dimmCtr);
                       break;
                }
            }
        }
       s = s->next ;
   }

   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int advMatch(char *expr, char *name)
---------------------------------------------------------------------------*/

int advMatch(expr, name)
char *expr;
char *name;
{
   while (*expr == *name)       /* skip as long as the strings match */
    {
       ++expr;
       ++name;
       /* 
        * '[' is the char right after id in matrix tuple name
        * '?' is the char right after id in scalar tuple name
        */
       if ((*expr == '[' || *expr == '?' || *expr == '\0') && 
           (*name == '[' || *name == '?' || *name == '\0'))
       {
           return 1 ;
       }
    }
   return 0 ;                   /* no match found */
}


/*---------------------------------------------------------------------------
  Prototype   : int advHasIt(char *str, char c)
---------------------------------------------------------------------------*/

int advHasIt(str, c)
char *str;
char c;
{
   while (*str != '\0')
    {
       if (*str == c) return 1 ;

       str++ ;
    }
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int advScalar(space1_t *o, space1_t *s)
---------------------------------------------------------------------------*/

int advScalar(o, s)
space1_t *o ;
space1_t *s ;
{
   char var_type[TUPLENAME_LEN];
   char var_name[TUPLENAME_LEN];
   char sclr_opt[TUPLENAME_LEN];
   int tokens;

   int iVar    = 0;
   long lvar   = 0;
   float fvar  = 0.0;
   double dvar = 0.0;

   sscanf(s->name, "%[^:]:%[^?]?%[^@]@%d", var_type, var_name, sclr_opt, &tokens) ;

   if (!strcmp(sclr_opt, "MAX"))
    {
       if (!strcmp(var_type, "int"))
        {
           if (*((int *)(s->tuple)) < *((int *)(o->tuple)))
               *((int *)(s->tuple)) = *((int *)(o->tuple));
        }
       else
       if (!strcmp(var_type, "long"))
        {
           if (*((long *)(s->tuple)) < *((long *)(o->tuple)))
               *((long *)(s->tuple)) = *((long *)(o->tuple));
        }
       else
       if (!strcmp(var_type, "float"))
        {
           if (*((float *)(s->tuple)) < *((float *)(o->tuple)))
               *((float *)(s->tuple)) = *((float *)(o->tuple));
        }
       else
       if (!strcmp(var_type, "double"))
        {
           if (*((double *)(s->tuple)) < *((double *)(o->tuple)))
               *((double *)(s->tuple)) = *((double *)(o->tuple));
        }
       o->scalar++;
    }
   else
   if (!strcmp(sclr_opt, "MIN"))
    {
       if (!strcmp(var_type, "int"))
        {
           if (*((int *)(s->tuple)) > *((int *)(o->tuple)))
               *((int *)(s->tuple)) = *((int *)(o->tuple));
        }
       else
       if (!strcmp(var_type, "long"))
        {
           if (*((long *)(s->tuple)) > *((long *)(o->tuple)))
               *((long *)(s->tuple)) = *((long *)(o->tuple));
        }
       else
       if (!strcmp(var_type, "float"))
        {
           if (*((float *)(s->tuple)) > *((float *)(o->tuple)))
               *((float *)(s->tuple)) = *((float *)(o->tuple));
        }
       else
       if (!strcmp(var_type, "double"))
        {
           if (*((double *)(s->tuple)) > *((double *)(o->tuple)))
               *((double *)(s->tuple)) = *((double *)(o->tuple));
        }
       o->scalar++;
    }

   return 0;
}


/*---------------------------------------------------------------------------
  Prototype   : int advIsFull(char *name)
---------------------------------------------------------------------------*/

int advIsFull(name)
char *name;
{
   while (*name != '\0')
    {
       if (*name == '[') return 0 ;

       name++ ;
    }
   return 1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int advDimms(char *expr, char septr)
---------------------------------------------------------------------------*/

int advDimms(expr, septr)
char *expr;
char septr;
{
   int dimms = 0;
   while (*expr != '\0')
    {
       if (*expr == septr) ++dimms;

       expr++ ;
    }
   return dimms ;
}


/*---------------------------------------------------------------------------
  Prototype   : int advResolveRequest(queue1_t *q)
---------------------------------------------------------------------------*/

int advResolveRequest(q)
queue1_t *q ;
{
   int sd ;
   tsh_get_ot2 out ;
                                /* connect to the requestor */
   if ((sd = get_socket()) == -1)
   {
        printf(" Cannot getsocket for tuple object. Call system operator. \n");
        exit (E_SOCKET);
   }
   if (!do_connect(sd, q->host, q->port))
    {
       close(sd) ;
       exit (E_CONNECT);
    }
                                /* send tuple name, length, priority */
   strcpy(out.name, q->expr) ;
   out.priority = htons(q->priority) ;
   out.length = htonl(q->reqLen) ;
   if (!writen(sd, (char *)&out, sizeof(tsh_get_ot2)))
    {
       close(sd) ;
       exit (E_CONNECT);
    }                           /* send tuple data */
   if (!writen(sd, q->data , q->reqLen))
    {
       close(sd) ;
       exit (E_CONNECT);
    }
   close(sd);

   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int PartTupleToPartReq1(space1_t *s, queue1_t *q)
---------------------------------------------------------------------------*/

int PartTupleToPartReq1(s, q)
space1_t *s;
queue1_t *q;
{
    char *var_s, *id_s, *var_q, *id_q, *st, *qt;
    int start0_s, stop0_s, step0_s;
    int start0_q, stop0_q, step0_q;
    int start0, stop0, step0 ;
    int bytes_s, bytes_q, bytes ;
    int x0, y0_q, y0_s;

    var_s = (char *)malloc(100) ;
    id_s  = (char *)malloc(10) ;
    var_q = (char *)malloc(100) ;
    id_q  = (char *)malloc(10) ;

    sscanf(s->name, "%[^#]#%[^[][%d~%d,%d]@%d", var_s, id_s, 
           &start0_s, &stop0_s, &step0_s, &bytes_s);
    sscanf(q->expr, "%[^#]#%[^[][%d~%d,%d]@%d", var_q, id_q, 
           &start0_q, &stop0_q, &step0_q, &bytes_q);

    if (start0_q >= stop0_s || stop0_q <= start0_s || step0_q != step0_s || 
        bytes_q != bytes_s)
     {
        free(var_s) ;
        free(id_s) ;
        free(var_q) ;
        free(id_q) ;
        return -1;
     }

    step0 = step0_q ;
    bytes = bytes_q ;

    /*
     * In case of "jump", check if the two are in the same pace.
     */
    if (start0_q > start0_s)
     {
        for (x0 = start0_s; x0 < stop0_s; x0 +=step0)
         {
            if (x0 == start0_q) break;
         }
         if (x0 != start0_q)
         {
            free(var_s) ;
            free(id_s) ;
            free(var_q) ;
            free(id_q) ;
            return -1;
         }
     }
    else
    if (start0_q < start0_s)
     {
        for (x0 = start0_q; x0 < stop0_q; x0 +=step0)
         {
            if (x0 == start0_s) break;
         }
         if (x0 != start0_s)
         {
            free(var_s) ;
            free(id_s) ;
            free(var_q) ;
            free(id_q) ;
            return -1;
         }
     }

    /* Now, we can say there are some common data between the two. */

    start0 = start0_q > start0_s? start0_q : start0_s ;
    stop0  = stop0_q  < stop0_s ? stop0_q  : stop0_s ;

    /* part tuple => part request */
    for (x0 = start0, y0_q =x0 - start0_q, y0_s =x0 - start0_s; 
         x0 < stop0; 
         x0 +=step0, y0_q ++, y0_s ++)
     {
         qt = q->data  + y0_q * bytes;
         st = s->tuple + y0_s * bytes;

         memcpy(qt, st, bytes);
         q->curLen += bytes;
     }

    free(var_s) ;
    free(id_s) ;
    free(var_q) ;
    free(id_q) ;
    return 0;
}


/*---------------------------------------------------------------------------
  Prototype   : int PartTupleToPartReq2(space1_t *s, queue1_t *q)
---------------------------------------------------------------------------*/

int PartTupleToPartReq2(s, q)
space1_t *s;
queue1_t *q;
{
    char *var_s, *id_s, *var_q, *id_q, *st, *qt;
    int start0_s, stop0_s, step0_s;
    int start0_q, stop0_q, step0_q;
    int start0, stop0, step0 ;
    int start1_s, stop1_s, step1_s;
    int start1_q, stop1_q, step1_q;
    int start1, stop1, step1 ;
    int bytes_s, bytes_q, bytes ;
    int x0, y0_q, y0_s, x1, y1_q, y1_s;

    var_s = (char *)malloc(100) ;
    id_s  = (char *)malloc(10) ;
    var_q = (char *)malloc(100) ;
    id_q  = (char *)malloc(10) ;

    sscanf(s->name, "%[^#]#%[^[][%d~%d,%d][%d~%d,%d]@%d", var_s, id_s, 
           &start0_s, &stop0_s, &step0_s, &start1_s, &stop1_s, &step1_s, &bytes_s);
    sscanf(q->expr, "%[^#]#%[^[][%d~%d,%d][%d~%d,%d]@%d", var_q, id_q, 
           &start0_q, &stop0_q, &step0_q, &start1_q, &stop1_q, &step1_q, &bytes_q);

    if (start0_q >= stop0_s || stop0_q <= start0_s || step0_q != step0_s || 
        start1_q >= stop1_s || stop1_q <= start1_s || step1_q != step1_s || 
        bytes_q != bytes_s)
    {
        free(var_s) ;
        free(id_s) ;
        free(var_q) ;
        free(id_q) ;
        return -1;
    }

    step0 = step0_q ;
    step1 = step1_q ;
    bytes = bytes_q ;

    /*
     * In case of "jump", check if the two are in the same pace.
     */
    if (start0_q > start0_s)
     {
        for (x0 = start0_s; x0 < stop0_s; x0 +=step0)
         {
            if (x0 == start0_q) break;
         }
         if (x0 != start0_q)
         {
            free(var_s) ;
            free(id_s) ;
            free(var_q) ;
            free(id_q) ;
            return -1;
         }
     }
    else
    if (start0_q < start0_s)
     {
        for (x0 = start0_q; x0 < stop0_q; x0 +=step0)
         {
            if (x0 == start0_s) break;
         }
         if (x0 != start0_s)
         {
            free(var_s) ;
            free(id_s) ;
            free(var_q) ;
            free(id_q) ;
            return -1;
         }
     }

    if (start1_q > start1_s)
     {
        for (x1 = start1_s; x1 < stop1_s; x1 +=step1)
         {
            if (x1 == start1_q) break;
         }
         if (x1 != start1_q)
         {
            free(var_s) ;
            free(id_s) ;
            free(var_q) ;
            free(id_q) ;
            return -1;
         }
     }
    else
    if (start1_q < start1_s)
     {
        for (x1 = start1_q; x1 < stop1_q; x1 +=step1)
         {
            if (x1 == start1_s) break;
         }
         if (x1 != start1_s)
         {
            free(var_s) ;
            free(id_s) ;
            free(var_q) ;
            free(id_q) ;
            return -1;
         }
     }

    /* Now, we can say there are some common data between the two. */

    start0 = start0_q > start0_s? start0_q : start0_s ;
    stop0  = stop0_q  < stop0_s ? stop0_q  : stop0_s ;
    start1 = start1_q > start1_s? start1_q : start1_s ;
    stop1  = stop1_q  < stop1_s ? stop1_q  : stop1_s ;

    /* part tuple => part request */
    for (x0 = start0, y0_q =x0 - start0_q, y0_s =x0 - start0_s; 
         x0 < stop0; 
         x0 +=step0, y0_q ++, y0_s ++)
     {
        for (x1 = start1, y1_q =x1 - start1_q, y1_s =x1 - start1_s; 
             x1 < stop1; 
             x1 +=step1, y1_q ++, y1_s ++)
         {
            qt = q->data  + (y0_q *((stop1_q - start1_q - 1) / step1_q + 1) + y1_q) * bytes ;
            st = s->tuple + (y0_s *((stop1_s - start1_s - 1) / step1_s + 1) + y1_s) * bytes ;

            memcpy(qt, st, bytes);
            q->curLen += bytes;
         }
     }

    free(var_s) ;
    free(id_s) ;
    free(var_q) ;
    free(id_q) ;
    return 0;
}


/*---------------------------------------------------------------------------
  Prototype   : int FullTupleToPartReq1(space1_t *s, queue1_t *q)
---------------------------------------------------------------------------*/

int FullTupleToPartReq1(s, q)
space1_t *s;
queue1_t *q;
{
    char *var, *id, *st, *qt;
    int start0, stop0, step0;
    int bytes;
    int x0, y0;

    var = (char *)malloc(100) ;
    id  = (char *)malloc(10) ;

    sscanf(q->expr, "%[^#]#%[^[][%d~%d,%d]@%d", var, id, 
           &start0, &stop0, &step0, &bytes);

    /* full tuple => part request */
    for (x0 = start0, y0 =0; x0 < stop0; x0 +=step0, y0 ++)
     {
            qt = q->data  + y0 * bytes;
            st = s->tuple + x0 * bytes;

            memcpy(qt, st, bytes);
     }

    free(var) ;
    free(id) ;
    return 0;
}


/*---------------------------------------------------------------------------
  Prototype   : int FullTupleToPartReq2(space1_t *s, queue1_t *q)
---------------------------------------------------------------------------*/

int FullTupleToPartReq2(s, q)
space1_t *s;
queue1_t *q;
{
    char *type, *var, *id, *st, *qt;
    int dimmSize0, dimmSize1;
    int start0, stop0, step0;
    int start1, stop1, step1;
    int bytes;
    int x0, y0, x1, y1;

int size1;

    type= (char *)malloc(100) ;
    var = (char *)malloc(100) ;
    id  = (char *)malloc(10) ;

    sscanf(q->expr, "%[^(](%d)(%d):%[^#]#%[^[][%d~%d,%d][%d~%d,%d]@%d", 
           type, &dimmSize0, &dimmSize1, var, id, 
           &start0, &stop0, &step0, &start1, &stop1, &step1, &bytes);

    /* full tuple => part request */
/*
    for (x0 = start0, y0 =0; x0 < stop0; x0 +=step0, y0 ++)
     {
        for (x1 = start1, y1 =0; x1 < stop1; x1 +=step1, y1 ++)
         {
            qt = q->data  + (y0 *((stop1 - start1 - 1) / step1 + 1) + y1) * bytes ;
            st = s->tuple + (x0 * dimmSize1 + x1) * bytes ;

            memcpy(qt, st, bytes);
         }
     }
*/
    size1 = ((stop1 - start1 - 1) / step1 + 1) * bytes;
    for (x0 = start0, y0 =0; x0 < stop0; x0 +=step0, y0 ++)
     {

        qt = q->data  + y0 * size1;
        st = s->tuple + (x0 * dimmSize1 + start1) * bytes;

        memcpy(qt, st, size1);
     }

    free(var) ;
    free(id) ;
    return 0;
}


/*---------------------------------------------------------------------------
  Prototype   : int PartTupleToFullReq1(space1_t *s, queue1_t *q)
---------------------------------------------------------------------------*/

int PartTupleToFullReq1(s, q)
space1_t *s;
queue1_t *q;
{
    char *var, *id, *st, *qt;
    int start0, stop0, step0;
    int bytes;
    int x0, y0;

    var = (char *)malloc(100) ;
    id  = (char *)malloc(10) ;

    sscanf(s->name, "%[^#]#%[^[][%d~%d,%d]@%d", var, id, 
           &start0, &stop0, &step0, &bytes);

    /* part tuple => full request */
    for (x0 = start0, y0 =0; x0 < stop0; x0 +=step0, y0 ++)
     {
            qt = q->data  + x0 * bytes;
            st = s->tuple + y0 * bytes;

            memcpy(qt, st, bytes);
     }

    free(var) ;
    free(id) ;
    return 0;
}


/*---------------------------------------------------------------------------
  Prototype   : int PartTupleToFullReq2(space1_t *s, queue1_t *q)
---------------------------------------------------------------------------*/

int PartTupleToFullReq2(s, q)
space1_t *s;
queue1_t *q;
{
    char *type, *var, *id, *st, *qt;
    int dimmSize0, dimmSize1;
    int start0, stop0, step0;
    int start1, stop1, step1;
    int bytes;
    int x0, y0, x1, y1;

int size1;

    type= (char *)malloc(100) ;
    var = (char *)malloc(100) ;
    id  = (char *)malloc(10) ;

    sscanf(s->name, "%[^(](%d)(%d):%[^#]#%[^[][%d~%d,%d][%d~%d,%d]@%d", 
           type, &dimmSize0, &dimmSize1, var, id, 
           &start0, &stop0, &step0, &start1, &stop1, &step1, &bytes);

    /* part tuple => full request */
/*
    for (x0 = start0, y0 =0; x0 < stop0; x0 +=step0, y0 ++)
     {
        for (x1 = start1, y1 =0; x1 < stop1; x1 +=step1, y1 ++)
         {
            qt = q->data  + (x0 * dimmSize1 + x1) * bytes;
            st = s->tuple + (y0 *((stop1 - start1 - 1) / step1 + 1) + y1) * bytes;

            memcpy(qt, st, bytes);
         }
     }
*/
    size1 = ((stop1 - start1 - 1) / step1 + 1) * bytes;
    for (x0 = start0, y0 =0; x0 < stop0; x0 +=step0, y0 ++)
     {

        qt = q->data  + (x0 * dimmSize1 + start1) * bytes;
        st = s->tuple + y0 * size1;

        memcpy(qt, st, size1);
     }

    free(var) ;
    free(id) ;
    return 0;
}


/*---------------------------------------------------------------------------
  Prototype   : int createTokenTuples1(tsh_put_it in, char *d)
---------------------------------------------------------------------------*/

static int _batch_flag = 0;

int createTokenTuples1(in, d)
tsh_put_it in ;
char *d ;
{
    char *order;
    char *index0, *chunk_jump0;
    int lStart0, lStop0, lStep0, size0;
    int tStart0, tStop0, tStep0;
    int x0, y0;
    char *token_name, *t, *workerid ;
    space1_t *s ;
    space1_t *tmpQhd=NULL, *tmpQtl=NULL ;
    int cntr =0 ;
 
    token_name  = (char *)malloc(TUPLENAME_LEN) ;
    workerid    = (char *)malloc(TUPLENAME_LEN) ;
    order       = (char *)malloc(1) ;
    index0      = (char *)malloc(20) ;
    chunk_jump0 = (char *)malloc(1) ;

    sscanf(d, "%[!=](%[^:]:%d~%d,%d:%[#^]%d)", order,
           index0, &lStart0, &lStop0, &lStep0, chunk_jump0, &size0);

    if (*chunk_jump0 == '#')
     {
        for (x0 = lStart0; x0 < lStop0; x0 +=size0)
         {
            tStart0 = x0;
            tStop0  = (x0+size0)<lStop0? (x0+size0) : lStop0 ;

            sprintf(token_name, "%s(%d)", in.name, cntr++) ;
            t = (char *)malloc(255);
            sprintf(t, "%d@(%s:%d~%d,%d)", _batch_flag,
                    index0, tStart0, tStop0, lStep0);

            s = createTuple(token_name, t, 255, ntohs(in.priority)) ;
            if (s == NULL)
             {
                free(token_name) ;
                free(workerid) ;
                free(order) ;
                free(index0) ;
                free(chunk_jump0) ;
                free(t) ;
                return -1 ;
             }
/*
            s->prev = tmpQtl ;
            if (tmpQtl == NULL)
                tmpQhd = s ;
            else
                tmpQtl->next = s ;
            tmpQtl = s ;
*/

if (!consumeTuple(s)) storeTuple(s, 0) ;

         }
     }
    else
    if (*chunk_jump0 == '^')
     {
        for (x0 = lStart0; x0 < size0; x0 +=lStep0)
         {
            tStart0 = x0 ;

            sprintf(token_name, "%s(%d)", in.name, cntr++) ;
            t = (char *)malloc(255);
            sprintf(t, "%d@(%s:%d~%d,%d)", _batch_flag,
                    index0, tStart0, lStop0, lStep0);

            s = createTuple(token_name, t, 255, ntohs(in.priority)) ;
            if (s == NULL)
             {
                free(token_name) ;
                free(workerid) ;
                free(order) ;
                free(index0) ;
                free(chunk_jump0) ;
                free(t) ;
                return -1 ;
             }
/*
            s->prev = tmpQtl ;
            if (tmpQtl == NULL)
                tmpQhd = s ;
            else
                tmpQtl->next = s ;
            tmpQtl = s ;
*/

if (!consumeTuple(s)) storeTuple(s, 0) ;

         }
     }
    else
     {
        free(token_name) ;
        free(workerid) ;
        free(order) ;
        free(index0) ;
        free(chunk_jump0) ;
        printf("Error: invalid flag chunk_jump (= %s)!\n", chunk_jump0);
        return -1 ;
     }
/*
    t = (char *)malloc(255);
    s = tmpQhd ;
    while (s)
     {
       tmpQhd = s->next ;

       s->next = NULL ;
       s->prev = NULL ;

       sprintf(t, "%d@%s", cntr, s->tuple) ;
       sprintf(s->tuple, "%s", t);

       if (!consumeTuple(s)) storeTuple(s, 0) ;

       s = tmpQhd ;
     }
    free(t) ;
*/
_batch_flag++;

    free(token_name) ;
    free(workerid) ;
    free(order) ;
    free(index0) ;
    free(chunk_jump0) ;
    return cntr ;
}


/*---------------------------------------------------------------------------
  Prototype   : int createTokenTuples2(tsh_put_it in, char *d)
---------------------------------------------------------------------------*/

int createTokenTuples2(in, d)
tsh_put_it in ;
char *d ;
{
    char *order;
    char *index0, *chunk_jump0;
    char *index1, *chunk_jump1;
    int lStart0, lStop0, lStep0, size0;
    int tStart0, tStop0, tStep0;
    int oStart0, oStop0, oStep0;
    int lStart1, lStop1, lStep1, size1;
    int tStart1, tStop1, tStep1;
    int x0, y0, x1, y1, xx; 
    char *token_name, *t, *workerid ;
    space1_t *s ;
    space1_t *tmpQhd=NULL, *tmpQtl=NULL ;
    int cntr =0 ;

    token_name  = (char *)malloc(TUPLENAME_LEN) ;
    workerid    = (char *)malloc(TUPLENAME_LEN) ;
    order       = (char *)malloc(1) ;
    index0      = (char *)malloc(20) ;
    chunk_jump0 = (char *)malloc(1) ;
    index1      = (char *)malloc(20) ;
    chunk_jump1 = (char *)malloc(1) ;

    sscanf(d, "%[!=](%[^:]:%d~%d,%d:%[#^]%d)(%[^:]:%d~%d,%d:%[#^]%d)", order,
           index0, &lStart0, &lStop0, &lStep0, chunk_jump0, &size0, 
           index1, &lStart1, &lStop1, &lStep1, chunk_jump1, &size1);

    if (*order == '=')
     {
            tStart0 = lStart0; 
            for (x0 = lStart0; x0 < lStop0; ) 
             { 
                x0 += size0 ; 
                tStop0 = (x0 > lStop0? lStop0 : x0) ; 

                oStart0 = tStart0; 
                oStop0 = tStop0; 

                tStart1 = lStart1; 
                for (x1 = lStart1; x1 < lStop1;) 
                 { 
                    x1 += size1 ; 
                    tStop1 = (x1 > lStop1? lStop1 : x1) ; 

                    sprintf(token_name, "%s(%d)", in.name, cntr++) ; 
                    t = (char *)malloc(255); 
                    sprintf(t, "%d@(%s:%d~%d,%d)(%s:%d~%d,%d)", _batch_flag, 
                            index0, tStart0, tStop0, lStep0, 
                            index1, tStart1, tStop1, lStep1); 

                    s = createTuple(token_name, t, 255, ntohs(in.priority)) ;
                    if (s == NULL) 
                     { 
                        free(token_name) ; 
                        free(workerid) ;
                        free(order) ;
                        free(index0) ; 
                        free(chunk_jump0) ; 
                        free(index1) ; 
                        free(chunk_jump1) ; 
                        free(t) ; 
                        return -1 ; 
                     } 
/*
                    s->prev = tmpQtl ;
                    if (tmpQtl == NULL)
                        tmpQhd = s ;
                    else
                        tmpQtl->next = s ;
                    tmpQtl = s ;
*/
       if (!consumeTuple(s)) storeTuple(s, 0) ;

                    if (x1 < lStop1) tStart1 = x1; 

                    tStop0 = tStart0; 
                    tStart0 -= size0; 
                    if (tStart0 < lStart0) break; 
                 } 


                if (x0 < lStop0) tStart0 = x0; 
             } 

            tStart0 = oStart0; 
            tStop0 = oStop0; 
            for (xx = lStart1; xx < lStop1; ) 
             { 
                xx += size1 ; 
                if (xx > lStop1) break ; 
                tStop0 = oStop0; 

                tStart1 = xx; 
                for (x1 = xx; x1 < lStop1;) 
                 { 
                    x1 += size1 ; 
                    tStop1 = (x1 > lStop1? lStop1 : x1) ; 

                    sprintf(token_name, "%s(%d)", in.name, cntr++) ; 
                    t = (char *)malloc(255); 
                    sprintf(t, "%d@(%s:%d~%d,%d)(%s:%d~%d,%d)", _batch_flag, 
                            index0, tStart0, tStop0, lStep0, 
                            index1, tStart1, tStop1, lStep1); 

                    s = createTuple(token_name, t, 255, ntohs(in.priority)) ; 
                    if (s == NULL) 
                     { 
                        free(token_name) ; 
                        free(workerid) ;
                        free(order) ;
                        free(index0) ; 
                        free(chunk_jump0) ; 
                        free(index1) ; 
                        free(chunk_jump1) ; 
                        free(t) ; 
                        return -1 ; 
                     } 
/*
                    s->prev = tmpQtl ;
                    if (tmpQtl == NULL)
                        tmpQhd = s ;
                    else
                        tmpQtl->next = s ;
                    tmpQtl = s ;
*/
       if (!consumeTuple(s)) storeTuple(s, 0) ;

                    if (x1 < lStop1) tStart1 = x1; 

                    tStop0 = tStart0; 
                    tStart0 -= size0; 
                    if (tStart0 < lStart0) break; 
                 } 

                tStart0 = oStart0; 
             }
     }
    else
    if (*chunk_jump0 == '#')
     {
        if (*chunk_jump1 == '#')
         {
             for (x0 = lStart0; x0 < lStop0; x0 +=size0)
              {
                 tStart0 = x0;
                 tStop0  = (x0+size0)<lStop0? (x0+size0) : lStop0 ;

                 for (x1 = lStart1; x1 < lStop1; x1 +=size1)
                  {
                     tStart1 = x1;
                     tStop1  = (x1+size1)<lStop1? (x1+size1) : lStop1 ;

                     sprintf(token_name, "%s(%d)", in.name, cntr++) ;
                     t = (char *)malloc(255);
                     sprintf(t, "%d@(%s:%d~%d,%d)(%s:%d~%d,%d)", _batch_flag, 
                             index0, tStart0, tStop0, lStep0, 
                             index1, tStart1, tStop1, lStep1);

                     s = createTuple(token_name, t, 255, ntohs(in.priority)) ;
                     if (s == NULL)
                      {
                         free(token_name) ;
                         free(workerid) ;
                         free(order) ;
                         free(index0) ;
                         free(chunk_jump0) ;
                         free(index1) ;
                         free(chunk_jump1) ;
                         free(t) ;
                         return -1 ;
                      }
/*
                     s->prev = tmpQtl ;
                     if (tmpQtl == NULL)
                         tmpQhd = s ;
                     else
                         tmpQtl->next = s ;
                     tmpQtl = s ;
*/
       if (!consumeTuple(s)) storeTuple(s, 0) ;

                  }
              }
         }
        else
        if (*chunk_jump1 == '^')
         {
             for (x0 = lStart0; x0 < lStop0; x0 +=size0)
              {
                 tStart0 = x0;
                 tStop0  = (x0+size0)<lStop0? (x0+size0) : lStop0 ;

                 for (x1 = lStart1; x1 < size1; x1 +=lStep1)
                  {
                     tStart1 = x1 ;

                     sprintf(token_name, "%s(%d)", in.name, cntr++) ;
                     t = (char *)malloc(255);
                     sprintf(t, "%d@(%s:%d~%d,%d)(%s:%d~%d,%d)", _batch_flag, 
                             index0, tStart0, tStop0, lStep0, 
                             index1, tStart1, lStop1, lStep1);

                     s = createTuple(token_name, t, 255, ntohs(in.priority)) ;
                     if (s == NULL)
                      {
                         free(token_name) ;
                         free(workerid) ;
                         free(order) ;
                         free(index0) ;
                         free(chunk_jump0) ;
                         free(index1) ;
                         free(chunk_jump1) ;
                         free(t) ;
                         return -1 ;
                      }
/*
                     s->prev = tmpQtl ;
                     if (tmpQtl == NULL)
                         tmpQhd = s ;
                     else
                         tmpQtl->next = s ;
                     tmpQtl = s ;
*/
       if (!consumeTuple(s)) storeTuple(s, 0) ;

                  }
              }
         }
        else
         {
            free(token_name) ;
            free(workerid) ;
            free(order) ;
            free(index0) ;
            free(chunk_jump0) ;
            free(index1) ;
            free(chunk_jump1) ;
            printf("Error: invalid flag chunk_jump (= %s)!\n", chunk_jump1);
            return -1 ;
         }
     }
    else
    if (*chunk_jump0 == '^')
     {
        if (*chunk_jump1 == '#')
         {
             for (x0 = lStart0; x0 < size0; x0 +=lStep0)
              {
                 tStart0 = x0 ;

                 for (x1 = lStart1; x1 < lStop1; x1 +=size1)
                  {
                     tStart1 = x1;
                     tStop1  = (x1+size1)<lStop1? (x1+size1) : lStop1 ;

                     sprintf(token_name, "%s(%d)", in.name, cntr++) ;
                     t = (char *)malloc(255);
                     sprintf(t, "%d@(%s:%d~%d,%d)(%s:%d~%d,%d)", _batch_flag, 
                             index0, tStart0, lStop0, lStep0, 
                             index1, tStart1, tStop1, lStep1);

                     s = createTuple(token_name, t, 255, ntohs(in.priority)) ;
                     if (s == NULL)
                      {
                         free(token_name) ;
                         free(workerid) ;
                         free(order) ;
                         free(index0) ;
                         free(chunk_jump0) ;
                         free(index1) ;
                         free(chunk_jump1) ;
                         free(t) ;
                         return -1 ;
                      }
/*
                     s->prev = tmpQtl ;
                     if (tmpQtl == NULL)
                         tmpQhd = s ;
                     else
                         tmpQtl->next = s ;
                     tmpQtl = s ;
*/
       if (!consumeTuple(s)) storeTuple(s, 0) ;

                  }
              }
         }
        else
        if (*chunk_jump1 == '^')
         {
             for (x0 = lStart0; x0 < size0; x0 +=lStep0)
              {
                 tStart0 = x0 ;

                 for (x1 = lStart1; x1 < size1; x1 +=lStep1)
                  {
                     tStart1 = x1 ;

                     sprintf(token_name, "%s(%d)", in.name, cntr++) ;
                     t = (char *)malloc(255);
                     sprintf(t, "%d@(%s:%d~%d,%d)(%s:%d~%d,%d)", _batch_flag, 
                             index0, tStart0, lStop0, lStep0, 
                             index1, tStart1, lStop1, lStep1);

                     s = createTuple(token_name, t, 255, ntohs(in.priority)) ;
                     if (s == NULL)
                      {
                         free(token_name) ;
                         free(workerid) ;
                         free(order) ;
                         free(index0) ;
                         free(chunk_jump0) ;
                         free(index1) ;
                         free(chunk_jump1) ;
                         free(t) ;
                         return -1 ;
                      }
/*
                     s->prev = tmpQtl ;
                     if (tmpQtl == NULL)
                         tmpQhd = s ;
                     else
                         tmpQtl->next = s ;
                     tmpQtl = s ;
*/
       if (!consumeTuple(s)) storeTuple(s, 0) ;

                  }
              }
         }
        else
         {
            free(token_name) ;
            free(workerid) ;
            free(order) ;
            free(index0) ;
            free(chunk_jump0) ;
            free(index1) ;
            free(chunk_jump1) ;
            printf("Error: invalid flag chunk_jump (= %s)!\n", chunk_jump1);
            return -1 ;
         }
     }
    else
     {
        free(token_name) ;
        free(workerid) ;
        free(order) ;
        free(index0) ;
        free(chunk_jump0) ;
        free(index1) ;
        free(chunk_jump1) ;
        printf("Error: invalid flag chunk_jump (= %s)!\n", chunk_jump0);
        return -1 ;
     }
/*
    t = (char *)malloc(255);
    s = tmpQhd ;
    while (s)
     {
       tmpQhd = s->next ;

       s->next = NULL ;
       s->prev = NULL ;

       sprintf(t, "%d@%s", cntr, s->tuple) ;
       sprintf(s->tuple, "%s", t);

       if (!consumeTuple(s)) storeTuple(s, 0) ;

       s = tmpQhd ;
     }
    free(t) ;
*/
_batch_flag++;

    free(token_name) ;
    free(workerid) ;
    free(order) ;
    free(index0) ;
    free(chunk_jump0) ;
    free(index1) ;
    free(chunk_jump1) ;
    return cntr ;
}

void OpRmAll()
{
   space1_t *s;
   space1_t *p;
   tsh_get_ot1 out1 ;

   s = p = tsh.space;
   while (s != NULL)
   {
	s = s->next;
	free(p->tuple);
	free(p);
	p = s;
   }
   tsh.space = NULL; 
   // Removed all existing tuples 
   /* report success */
   out1.status = htons(SUCCESS) ; out1.error = htons(TSH_ER_NOERROR) ;
   if (!writen(newsock, (char *)&out1, sizeof(tsh_get_ot1)))
      printf("TSH RmAll ACK failure\n");
   return ;
}
