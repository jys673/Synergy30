/*.........................................................................*/
/*                     FAH.C ------> File Access Handler                   */
/*.........................................................................*/

#include "fah.h"

/*---------------------------------------------------------------------------
  Prototype   : int main(int argc, char **argv)
  Parameters  : argv[1] -  "-s" --> read initialization data from socket [or]
                           "-a" --> read initialization data from command line
		argv[2] -  socket descriptor #
		           application id
		argv[3] -  FAH name
  Returns     : Never returns 
  Called by   : System
  Calls       : initFromsocket, initFromline, start, strcmp, exit
  Notes       : FAH can be started either by CID or from the shell prompt
                by the user. In the former case, initialization data is
		read from the socket (from DAC). Otherwise, data is read
		from command line. The switch -s or -a indicate the option.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification: Modified FAH to read appid, name from command line.
---------------------------------------------------------------------------*/  

int main(argc, argv)
int argc; 
char **argv ;
{
   if (argc < 3)
      exit(1) ;
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

void initFromline2(appid, name, sd)
char *appid; 
char *name;
int sd;
{
   fah_start_it in ;
   fah_start_ot out ;
				/* read application id. & tuple space name */
   if (!readn(sd, (char *)&in, sizeof(in)))
      exit(1) ;
   strcpy(fah.appid, appid) ;
   strcpy(fah.name, name) ;
   
   if (!initCommon())		/* perform other initializations */
    {
       out.port = htons(0) ;
       out.status = htons(FAILURE) ;  out.error = htons(TSH_ER_INSTALL) ;
       writen(sd, (char *)&out, sizeof(out)) ;
       exit(1) ;
    }
				/* report TSH port, status to DAC */
   out.status = htons(SUCCESS) ;  out.error = htons(TSH_ER_NOERROR) ;
   out.port = fah.port ;
   if (!writen(sd, (char *)&out, sizeof(out)))
    {
       unmapFahport() ;
       exit(1) ;
    }
   close(sd) ;
}

/*---------------------------------------------------------------------------
  Prototype   : void initFromline(char *appid, char *name)
  Parameters  : appid - application id
                name  - FAH name
  Returns     : -
  Called by   : main
  Calls       : initCommon, strcpy, exit
  Notes       : Required initialization is performed when FAH is started by
		the user from shell prompt.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification: Modified to initialise from parameters & not from file.
---------------------------------------------------------------------------*/  

void initFromline(appid, name)
char *appid; 
char *name;
{
   strcpy(fah.appid, appid) ;
   strcpy(fah.name, name) ;
				/* perform other initializations */
   if (!initCommon())
      exit(1) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void initFromsocket(int sd)
  Parameters  : sd - socket descriptor of connection with DAC
  Returns     : -
  Called by   : main
  Calls       : initCommon, readn, writen, unmapFahport, strcpy, exit,htons
  Notes       : Required initialization is done when FAH is started by CID.
                Initialization data (application id, tuple space name) is read
		from DAC through the socket passed.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void initFromsocket(sd)
int sd;
{
   fah_start_it in ;
   fah_start_ot out ;
				/* read app-id, component name */
   if (!readn(sd, (char *)&in, sizeof(in)))
      exit(1) ;
   strcpy(fah.appid, in.appid) ;
   strcpy(fah.name, in.name) ;
   
   if (!initCommon())		/* perform other initializations */
    {
       out.port = htons(0) ;
       out.status = htons(FAILURE) ;  out.error = htons(FAH_ER_INSTALL) ;
       writen(sd, (char *)&out, sizeof(out)) ;
       exit(1) ;
    }
				/* report FAH port, status to DAC */
   out.status = htons(SUCCESS) ;  out.error = htons(FAH_ER_NOERROR) ;
   out.port = fah.port ;
   if (!writen(sd, (char *)&out, sizeof(out)))
    {
       unmapFahport() ;
       exit(1) ;
    }
   close(sd) ;
}


/*---------------------------------------------------------------------------
  Prototype   : int initCommon(void)
  Parameters  : -
  Returns     : 1 - initialization success
                0 - initialization failed
  Called by   : initFromline, initFromsocket
  Calls       : signal, getFahport, mapFahport
  Notes       : This function performs required initializations irrespective
                of how FAH is started (DAC/user). 
		'oldsock' is a global variable in which FAH connections are 
		accepted.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int initCommon()
{
   int i ;
				/* allocate memory for FAH table */
   if ((table = (table_t *) malloc(sizeof(table_t) * TABLE_SIZE)) == NULL)
      return 0 ;
   for (i =  0 ; i < TABLE_SIZE ; i++)
      table[i].fp = NULL ;	/* entry is unused */

   if (signal(SIGTERM, sigtermHandler) == (void *)-1)
      return 0 ;
				/* get a port to accept requests */
   if ((oldsock = getFahport()) == -1)
      return 0 ;
				/* map FAH port with PMD */
   if (!mapFahport())
      return 0 ;

   return 1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : void start(void)
  Parameters  : -
  Returns     : -
  Called by   : main
  Calls       : get_connection, readn, close, ntohs
  Notes       : This is the controlling function of FAH that invokes
                appropriate routine based on the operation requested.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void start()
{
  static void (*op_func[])() = {
    OpFopen, OpFclose, OpFgetc, OpFputc, OpFgets, OpFputs, OpFread, 
    OpFwrite, OpFseek, OpFflush, OpExit
    } ;
   
   while (TRUE)
    {				/* read operation on FAH port */
       if ((newsock = get_connection(oldsock, NULL)) == -1)
	{
	   unmapFahport() ;
	   exit(1) ;
	}
       if (!readn(newsock, (char *)&this_op, sizeof(this_op)))
	{
	   close(newsock) ;
	   continue ;
	}
				/* invoke function for operation */
       this_op = ntohs(this_op) ;
       if (this_op >= FAH_OP_MIN && this_op <= FAH_OP_MAX)
	  (*op_func[this_op - FAH_OP_MIN])() ;
       
       close(newsock) ;
    }
}


/*---------------------------------------------------------------------------
  Prototype   : void OpFopen(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, fopen, htons
  Notes       : The file is opened in the mode specified. If there is space
                in the table, the index of the table is returned as the file
		identifier to be used for future references.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpFopen()
{
   fah_fopen_it in ;
   fah_fopen_ot out ;
   int i ;
				/* obtain file path, open mode */
   if (!readn(newsock, (char *)&in, sizeof(in)))
      return ;
				/* locate empty position in table */
   for (i = 0 ; i < TABLE_SIZE ; i++)
      if (table[i].fp == NULL)
	 break ;

   if (i == TABLE_SIZE)
    {				/* no space available */
       out.status = htons(FAILURE) ; 
       out.error = htons(FAH_ER_NOSPACE) ;
       out.fid = htons(0) ;
    }
   else				/* open file and fill slot in table */
      if ((table[i].fp = fopen(in.path, in.mode)) == NULL)
       {
	  out.status = htons(FAILURE) ; 
	  out.error = htons(FAH_ER_FOPEN) ;
	  out.fid = htons(0) ;
       }
      else
       {			/* return fid (index into table) */
	  out.status = htons(SUCCESS) ;
	  out.error = htons(FAH_ER_NOERROR) ;
	  out.fid = htons(i) ;
	  strcpy(table[i].path, in.path) ; 
       }
   writen(newsock, (char *)&out, sizeof(out)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpFclose(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : writen, fclose, htons
  Notes       : The file corresponding to the descriptor is closed.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpFclose()
{
   fah_fclose_it in ;
   fah_fclose_ot out ;
   u_short fid ;
				/* obtain fid */
   if (!readn(newsock, (char *)&in, sizeof(in)))
      return ;
   fid = ntohs(in.fid) ;
				/* check if valid fid */
   if (!(fid < TABLE_SIZE && table[fid].fp != NULL))
    {				
       out.status = htons(FAILURE) ;
       out.error = htons(FAH_ER_NOTOPEN) ;
    }
   else				/* close file */
      if (fclose(table[fid].fp) == EOF)
       {
	  out.status = htons(FAILURE) ; 
	  out.error = htons(FAH_ER_FCLOSE) ;
       }
      else
       {			/* make file identifier slot available */
	  table[fid].fp = NULL ; 
	  out.status = htons(SUCCESS) ;
	  out.error = htons(FAH_ER_NOERROR) ;
       }
   writen(newsock, (char *)&out, sizeof(out)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpFlush(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : writen, fflush, htons
  Notes       : Standard IO buffer is flushed.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpFflush()
{
   fah_fflush_it in ;
   fah_fflush_ot out ;
   u_short fid ;
				/* obtain fid */
   if (!readn(newsock, (char *)&in, sizeof(in)))
      return ;
   fid = ntohs(in.fid) ;
				/* check if valid fid */
   if (!(fid < TABLE_SIZE && table[fid].fp != NULL))
    {
       out.status = htons(FAILURE) ;
       out.error = htons(FAH_ER_NOTOPEN) ;
    }
   else				/* flush stdio buffer */
      if (fflush(table[fid].fp) == EOF)
       {
	  out.status = htons(FAILURE) ; 
	  out.error = htons(FAH_ER_FFLUSH) ;
       }
      else
       {
	  out.status = htons(SUCCESS) ;
	  out.error = htons(FAH_ER_NOERROR) ;
       }
   writen(newsock, (char *)&out, sizeof(out)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpFgetc(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : fgetc, writen, htons
  Notes       : Reads a character from the file.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpFgetc()
{
   fah_fgetc_it in ;
   fah_fgetc_ot out ;
   u_short fid ;
				/* obtain fid */
   if (!readn(newsock, (char *)&in, sizeof(in)))
      return ;
   fid = ntohs(in.fid) ;
				/* check if valid fid */
   if (!(fid < TABLE_SIZE && table[fid].fp != NULL))
    {
       out.status = htons(FAILURE) ;
       out.error = htons(FAH_ER_NOTOPEN) ;
       out.ch = htonl(0) ;
    }
   else
    {				/* read character from file */
       if ((out.ch = fgetc(table[fid].fp)) == EOF)
	{
	   out.status = htons(FAILURE) ; 
	   out.error = htons(FAH_ER_FGETC) ;
	}
       else
	{
	   out.status = htons(SUCCESS) ;
	   out.error = htons(FAH_ER_NOERROR) ;
	}
       out.ch = htonl(out.ch) ;
    }				/* send character through socket */
   writen(newsock, (char *)&out, sizeof(out)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpFputc(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, fputc, htons
  Notes       : The character is stored in the file.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpFputc()
{
   fah_fputc_ot out ;
   fah_fputc_it in ;
   u_short fid ;
				/* obtain fid, character */
   if (!readn(newsock, (char *)&in, sizeof(in)))
      return ;
   fid = ntohs(in.fid) ;
				/* check if valid fid */
   if (!(fid < TABLE_SIZE && table[fid].fp != NULL))
    {
       out.status = htons(FAILURE) ;
       out.error = htons(FAH_ER_NOTOPEN) ;
    }
   else				/* store character in file */
      if (fputc(in.ch, table[fid].fp) == EOF)
       {
	  out.status = htons(FAILURE) ; 
	  out.error = htons(FAH_ER_FPUTC) ;
       }
      else
       {
	  out.status = htons(SUCCESS) ;
	  out.error = htons(FAH_ER_NOERROR) ;
       }
   writen(newsock, (char *)&out, sizeof(out)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpFseek(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, fseek, htons, ntohl
  Notes       : Seek to the position in the file.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpFseek()
{
   fah_fseek_it in ;
   fah_fseek_ot out ;
   u_short fid ;
				/* obtain fid, offset, from where */
   if (!readn(newsock, (char *)&in, sizeof(in)))
      return ;
   fid = ntohs(in.fid) ;
				/* check if valid fid */
   if (!(fid < TABLE_SIZE && table[fid].fp != NULL))
    {
       out.status = htons(FAILURE) ;
       out.error = htons(FAH_ER_NOTOPEN) ;
    }
   else				/* seek to location */
      if (fseek(table[fid].fp, ntohl(in.offset), ntohl(in.from)) == -1)
       {
	  out.status = htons(FAILURE) ; 
	  out.error = htons(FAH_ER_FSEEK) ;
       }
      else
       {
	  out.status = htons(SUCCESS) ;
	  out.error = htons(FAH_ER_NOERROR) ;
       }
   writen(newsock, (char *)&out, sizeof(out)) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpFread(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, fread, malloc, htons, htonl, ntohl, free
  Notes       : Read a set of records from the file.
                *--- malloc error is not checked
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpFread()
{
  fah_fread_it in ;
  fah_fread_ot out ;
  char *buff ;
  int size, nitems, count ;
  u_short fid ;
				/* obtain fid, # of items, item length */
  if (!readn(newsock, (char *)&in, sizeof(in)))
     return ;
  fid = ntohs(in.fid) ;
				/* check if valid fid */
  if (!(fid < TABLE_SIZE && table[fid].fp != NULL))
   {
      out.status = htons(FAILURE) ;
      out.error = htons(FAH_ER_NOTOPEN) ;
      out.nitems = htonl(0) ;
      writen(newsock, (char *)&out, sizeof(out)) ;
      return ;
   }
				/* read data from file */
  size = ntohl(in.size) ;
  nitems = ntohl(in.nitems) ;
  buff = (char *) malloc(size * nitems) ;
  if (!(count = fread(buff, size, nitems, table[fid].fp)))
   {
      out.status = htons(FAILURE) ; 
      out.error = htons(FAH_ER_FREAD) ;
   }
  else
   {
      out.status = htons(SUCCESS) ;
      out.error = htons(FAH_ER_NOERROR) ;
   }
				/* send size of data & then data */
  out.nitems = htonl(count) ;
  if (writen(newsock, (char *)&out, sizeof(out)))
     writen(newsock, (char *)buff, count * size) ;
  free(buff) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpWrite(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, fwrite, ntohl, htonl, htons, malloc, free
  Notes       : Write a set of records to the file.
                *--- malloc error is not checked
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpFwrite()
{
   fah_fwrite_it in ;
   fah_fwrite_ot out ;
   char *buff ;
   int size, nitems, count ;
   u_short fid ;
				/* obtain fid, # items, item length */
   if (!readn(newsock, (char *)&in, sizeof(in)))
      return ;
   fid = ntohs(in.fid) ;
				/* check if valid fid */
   if (!(fid < TABLE_SIZE && table[fid].fp != NULL))
    {
       out.status = htons(FAILURE) ;
       out.error = htons(FAH_ER_NOTOPEN) ;
       out.nitems = htonl(0) ;
       writen(newsock, (char *)&out, sizeof(out)) ;
       return ;
    }
				/* read items from socket */
   size = ntohl(in.size) ;
   nitems = ntohl(in.nitems) ;
   buff = (char *) malloc(size * nitems) ;
   if (!readn(newsock, buff, nitems * size))
    {
       free(buff) ;
       return ;
    }
				/* write items to file */
   if (!(count = fwrite(buff, size, nitems, table[fid].fp)))
    {
       out.status = htons(FAILURE) ; 
       out.error = htons(FAH_ER_FWRITE) ;
    }
   else
    {
       out.status = htons(SUCCESS) ;
       out.error = htons(FAH_ER_NOERROR) ;
    }
   out.nitems = htonl(count) ;
   writen(newsock, (char *)&out, sizeof(out)) ;
   free(buff) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpFgets(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, fgets, ntohl, htonl, htons, malloc, strlen,
                free
  Notes       : Reads a line from the file.
                *--- malloc error is not checked
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpFgets()
{
   fah_fgets_it in ;
   fah_fgets_ot out ;
   char *buff ;
   u_short fid ;
				/* obtain fid, length of string */
   if (!readn(newsock, (char *)&in, sizeof(in)))
      return ;
   fid = ntohs(in.fid) ;
				/* check if valid fid */
   if (!(fid < TABLE_SIZE && table[fid].fp != NULL))
    {
       out.status = htons(FAILURE) ; 
       out.error = htons(FAH_ER_NOTOPEN) ;
       out.len = htonl(0) ;
       writen(newsock, (char *)&out, sizeof(out)) ;
       return ;
    }
				/* read string from file & send it */
   buff = (char *) malloc(ntohl(in.len)) ;
   if (fgets(buff, ntohl(in.len), table[fid].fp) == NULL)
    {
       out.status = htons(FAILURE) ; 
       out.error = htons(FAH_ER_FGETS) ;
       out.len = htonl(0) ;
    }
   else
    {
       out.status = htons(SUCCESS) ;
       out.error = htons(FAH_ER_NOERROR) ;
       out.len = ntohl(strlen(buff)+1) ;
    }
				/* send string length & then string */
   if (writen(newsock, (char *)&out, sizeof(out)))
      writen(newsock, (char *)buff, ntohl(out.len)) ;
   free(buff) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpFputs(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : readn, writen, fputs, malloc, ntohl, htons
  Notes       : Writes a line to the file.
                *--- malloc error is not checked
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpFputs()
{
   fah_fputs_it in ;
   fah_fputs_ot out ;
   char *buff ;
   u_short fid ;
				/* obtain fid, length of string */
   if (!readn(newsock, (char *)&in, sizeof(in)))
      return ;
   fid = ntohs(in.fid) ;
				/* check if valid fid */
   if (!(fid < TABLE_SIZE && table[fid].fp != NULL))
    {
       out.status = htons(FAILURE) ; 
       out.error = htons(FAH_ER_NOTOPEN) ;
       writen(newsock, (char *)&out, sizeof(out)) ;
       return ;
    }
				/* read string from socket & store in file */
   buff = (char *) malloc(ntohl(in.len)) ;
   if (readn(newsock, buff, ntohl(in.len)))
    {
       fputs(buff, table[fid].fp) ;
       out.status = htons(SUCCESS) ;
       out.error = htons(FAH_ER_NOERROR) ;
       writen(newsock, (char *)&out, sizeof(out)) ;
    }
   free(buff) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void OpExit(void)
  Parameters  : -
  Returns     : -
  Called by   : start
  Calls       : htons, writen, unmapFahport, exit
  Notes       : FAH exits when this FAH_OP_EXIT is recieved from DAC. This
                function is called only when FAH is started by DAC.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void OpExit()
{
   fah_exit_ot out ;
   
   out.status = htons(SUCCESS) ;  out.error = htons(FAH_ER_NOERROR) ;
   writen(newsock, (char *)&out, sizeof(out)) ;

   unmapFahport() ;		/* unmap port from PMD */
   exit(0) ;
}
  

/*---------------------------------------------------------------------------
  Prototype   : int getFahport(void)
  Parameters  : -
  Returns     : socket descriptor of the port allocated to FAH
  Called by   : initCommon
  Calls       : get_socket, bind_socket, close
  Notes       : A socket is obtained and bound to a port. The FAH waits for
                requests on this port.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int getFahport()
{
   int sd ;
				/* obtain socket with required properties */
   if ((sd = get_socket()) == -1) 
      return -1 ;		/* bind socket to any port & obtain the port */
   if (!(fah.port = bind_socket(sd, 0)))
    {
       close(sd) ;
       return -1 ;
    }				/* allow connections to FAH on this socket*/
   return sd ;			/* return socket for FAH */
}


/*---------------------------------------------------------------------------
  Prototype   : int mapFahport(void)
  Parameters  : -
  Returns     : 1 - if FAH port is mapped with PMD
                0 - if FAH port could not be mapped with PMD
  Called by   : initCommon
  Calls       : strcpy, strcat, sngd_map
  Notes       : The FAH port is registered with the PMD.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int mapFahport()
{
   char mapid[MAP_LEN] ;

   strcpy(mapid, fah.appid) ;	/* mapid ---> 'appid'$'name' */
   strcat(mapid, "$") ;
   strcat(mapid, fah.name) ;

   return sngd_map(mapid, fah.port, mapid, (u_short)PMD_PROT_TCP) ;
}


/*---------------------------------------------------------------------------
  Prototype   : int unmapFahport(void)
  Parameters  : -
  Returns     : 1 - FAH port unregistered from PMD
                0 - FAH port not unregistered
  Called by   : sigtermHandler, OpExit, initFromsocket
  Calls       : strcat, sngd_unmap
  Notes       : The FAH port is unregistered from the PMD.
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int unmapFahport()
{
   char mapid[MAP_LEN] ;

   strcpy(mapid, fah.appid) ;	/* mapid ---> 'appid'$'name' */
   strcat(mapid, "$") ;
   strcat(mapid, fah.name) ;
   
   return sngd_unmap(mapid, 0, mapid, (u_short)PMD_PROT_TCP) ;
}


/*---------------------------------------------------------------------------
  Prototype   : void sigtermHandler(void)
  Parameters  : -
  Returns     : -
  Called by   : By system on SIGTERM
  Calls       : unmapFahport, exit
  Notes       : This function is invoked when FAH is terminated by the user
                i.e. when FAH is started by the user and not by CID. 
		This is the right way to kill FAH when it is started by user
		(i.e. by sending SIGTERM).
  Date        : April '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

void sigtermHandler()
{
   unmapFahport() ;		/* unmap FAH port from PMD */
   exit(0) ;
}


