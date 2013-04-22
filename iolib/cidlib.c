/*.........................................................................*/
/*                CIDLIB.C ------> CID library functions                   */
/*               February '13, updated by Justin Y. Shi                    */
/*.........................................................................*/

#include "synergy.h"

#define SEGMENT       5000	/* block size for receiving/sending file */

int connectCid(/*char **/) ;
int sendFile(/*int, char *, long*/) ;
int receiveFile(/*int, char *, long*/) ;

/*---------------------------------------------------------------------------
  Prototype   : int ping_cid(char *host)
  Parameters  : host - IP address in dot format [decimal]
  Returns     : 1 - CID is up [or] 
                0 - CID is down
  Calls       : connectCid, htons, ntohs, writen, readn, close
  Notes       : The corrsponding CID is sent CID_OP_NULL and tested if it 
                exists.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int ping_cid(host, login)
char *host;
char *login;
{
   cid_null_ot in ;
   u_short op ;
   int sd ;
				/* connect to CID */
   if ((sd = connectCid(host,login)) == -1)
      return 0 ;
				/* send op-code and read response */
   op = htons(CID_OP_NULL) ;
   if (writen(sd, (char *)&op, sizeof(u_short)))
      if (readn(sd, (char *)&in, sizeof(cid_null_ot)))
	 if (ntohs(in.status) == SUCCESS)
	  {
	     close(sd) ;
	     return 1 ;
	  }
   close(sd) ;
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int add_user(char *host, char *login)
  Parameters  : host - IP address in dot format [decimal]
                login - login id of the user in that m/c
  Returns     : 1 - login is added to user-list of synergy in host [or]
                0 - login is not added to user-list of synergy in host
  Calls       : connectCid, ntohs, htons, readn, writen, close
  Notes       : The CID in host is contacted to add login to it's user-list.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int add_user(host, login)
char *host; 
char *login;
{
   cid_adduser_it out ;
   cid_adduser_ot in ;
   u_short op ;
   int sd ;
				/* connect to CID */
   if ((sd = connectCid(host,login)) == -1)
      return 0 ;
				/* send op-code, login and read response */
   op = htons(CID_OP_ADD_USER) ;
   strcpy(out.login, login) ;
   if (writen(sd, (char *)&op, sizeof(u_short)))
      if (writen(sd, (char *)&out, sizeof(cid_adduser_it)))
	 if (readn(sd, (char *)&in, sizeof(cid_adduser_ot)))
	    if (ntohs(in.status) == SUCCESS)
	     {
		close(sd) ;
		return 1 ;
	     }
   close(sd) ;
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int delete_user(char *host, char *login)
  Parameters  : host - IP address in dot format [decimal]
                login - login id of the user in that m/c
  Returns     : 1 - login is deleted from user-list of synergy in host [or]
                0 - login is not deleted from user-list of synergy in host
  Calls       : connectCid, ntohs, htons, readn, writen, close
  Notes       : The CID in host is contacted to remove the user from the 
                synergy user-list for the host.
                valid synergy user.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int delete_user(host, login)
char *host; 
char *login;
{
   cid_deleteuser_it out ;
   cid_deleteuser_ot in ;
   u_short op ;
   int sd ;
				/* connect to CID */
   if ((sd = connectCid(host,login)) == -1)
      return 0 ;
				/* send op-code, login and read response */
   op = htons(CID_OP_DELETE_USER) ;
   strcpy(out.login, login) ;
   if (writen(sd, (char *)&op, sizeof(u_short)))
      if (writen(sd, (char *)&out, sizeof(cid_deleteuser_it)))
	 if (readn(sd, (char *)&in, sizeof(cid_deleteuser_ot)))
	    if (ntohs(in.status) == SUCCESS)
	     {
		close(sd) ;
		return 1 ;
	     }
   close(sd) ;
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int verify_user(char *host, char *login)
  Parameters  : host - IP address in dot format [decimal]
                login - login id of the user in that m/c
  Returns     : 1 - login is a valid user of synergy in host [or]
                0 - login is not a valid user of synergy in host
  Calls       : connectCid, ntohs, htons, readn, writen, close
  Notes       : The CID in host is contacted to verify whether login is a
                valid synergy user.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int verify_user(host, login)
char *host; 
char *login;
{
   cid_verifyuser_it out ;
   cid_verifyuser_ot in ;
   u_short op ;
   int sd ;
				/* connect to CID */
   if ((sd = connectCid(host,login)) == -1)
      return 0 ;
				/* send op-code, login and read response */
   op = htons(CID_OP_VERIFY_USER) ;
   strcpy(out.login, login) ;
   if (writen(sd, (char *)&op, sizeof(u_short)))
      if (writen(sd, (char *)&out, sizeof(cid_verifyuser_it)))
	 if (readn(sd, (char *)&in, sizeof(cid_verifyuser_ot)))
	    if (ntohs(in.status) == SUCCESS)
	     {
		close(sd) ;
		return 1 ;
	     }
   close(sd) ;
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int file_put(char *host, char *login, char *local_path, 
                                                           char *remote_path)
  Parameters  : host - IP address in dot format [decimal]
                login - login id of the user in that m/c
		local_path  - full path of file to be transferred
		remote_path - full path of file in remote m/c
  Returns     : 1 - file successfully tranferred [or]
                0 - file transfer error
  Calls       : fopen, strcpy, getpwuid, htons, ntohs, htonl, connectCid,
                writen, readn, sendFile, fclose
  Notes       : The file at local_path is transferred to remote_path in
                the specified host.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int file_put(host, login, local_path, remote_path)
char *host; 
char *login; 
char *local_path; 
char *remote_path;
{
   cid_fileput_it out ;
   cid_fileput_ot in ;
   struct stat buff ;
   struct passwd *p_passwd ;
   u_short op ;
   int sd ;

   if ((sd = connectCid(host,login)) == -1)
      return 0 ;
				/* fill operation parameters */
   strcpy(out.login, login) ;
   p_passwd = getpwuid(getuid()) ;
   strcpy(out.rlogin, p_passwd->pw_name) ;
   strcpy(out.path, remote_path) ;
   stat(local_path, &buff) ;
   out.len = htonl(buff.st_size) ;
   op = htons(CID_OP_FILE_PUT) ;
				/* send request and send file */
   if (writen(sd, (char *)&op, sizeof(u_short)))
      if (writen(sd, (char *)&out, sizeof(cid_fileput_it)))
	 if (sendFile(sd, local_path, buff.st_size))
	    if (readn(sd, (char *)&in, sizeof(cid_fileput_ot)))
	       if (ntohs(in.status) == SUCCESS)
		{
		   close(sd) ;
		   return 1 ;
		}
   close(sd) ;
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int file_get(char *host, char *login, char *local_path, 
                                                           char *remote_path)
  Parameters  : host - IP address in dot format [decimal]
                login - login id of the user in that m/c
		local_path  - full path of file to be received
		remote_path - full path of file in remote m/c
  Returns     : 1 - file successfully tranferred [or]
                0 - file transfer error
  Calls       : fopen, strcpy, getpwuid, htons, ntohs, ntohl, connectCid,
                writen, readn, receiveFile, fclose
  Notes       : The file at remote_path in specified host is transferred to 
	        local_path.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int file_get(host, login, local_path, remote_path)
char *host; 
char *login; 
char *local_path; 
char *remote_path;
{
   cid_fileget_it out ;
   cid_fileget_ot in ;
   struct passwd *p_passwd ;
   u_short op ;
   int sd ;
				/* connect to CID */
   if ((sd = connectCid(host,login)) == -1)
      return 0 ;
				/* fill operation parameters */
   strcpy(out.login, login) ;
   p_passwd = getpwuid(getuid()) ;
   strcpy(out.rlogin, p_passwd->pw_name) ;
   strcpy(out.path, remote_path) ;
   op = htons(CID_OP_FILE_GET) ;
				/* send request and receive file */
   if (writen(sd, (char *)&op, sizeof(u_short)))
      if (writen(sd, (char *)&out, sizeof(cid_fileget_it)))
	 if (readn(sd, (char *)&in, sizeof(cid_fileget_ot)))
	    if (ntohs(in.status) == SUCCESS)
	       if (receiveFile(sd, local_path, ntohl(in.len)))
		{
		   close(sd) ;
		   return 1 ;
		}
   close(sd) ;
   return 0 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int file_remove(char *host, char *login, char *path)
  Parameters  : host - IP address in dot format [decimal]
                login - login id of the user in that m/c
		path  - full path of file to be removed
  Returns     : 1 - file successfully removed [or]
                0 - file removal error
  Calls       : strcpy, getpwuid, htons, ntohs, connectCid, writen, readn
  Notes       : The file at path is removed in specified host.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int file_remove(host, login, path)
char *host; 
char *login; 
char *path;
{
   cid_remove_ot in ;
   cid_remove_it out ;
   struct passwd *p_passwd ;
   u_short op ;
   int sd ;
				/* connect to CID */
   if ((sd = connectCid(host,login)) == -1)
      return 0 ;
				/* fill operation parameters */
   strcpy(out.login, login) ;
   p_passwd = getpwuid(getuid()) ;
   strcpy(out.rlogin, p_passwd->pw_name) ;
   strcpy(out.path, path) ;
   op = htons(CID_OP_REMOVE) ;
				/* send request & get response */
   if (writen(sd, (char *)&op, sizeof(u_short)))
      if (writen(sd, (char *)&out, sizeof(cid_remove_it)))
	 if (readn(sd, (char *)&in, sizeof(cid_remove_ot)))
	    if (ntohs(in.status) == SUCCESS)
	     {
		close(sd) ;
		return 1 ;
	     }
   close(sd) ;
   return 0 ;
}
   

/*---------------------------------------------------------------------------
  Prototype   : int sendFile(int sd, char *path, long size)
  Parameters  : sd   - socket descriptor of connection to CID
                path - path of file to be transferred
		size - file size
  Returns     : 1 - file successfully transferred [or]
                0 - file transfer error
  Called by   : file_put
  Calls       : malloc, free, fread, writen
  Notes       : The file is sent in blocks of size SEGMENT using sd.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int sendFile(sd, path, size)
int sd; 
char *path; 
long size;
{
   FILE *fp ;
   char *data ;
   int toread ;

   if ((data = (char *) malloc(SEGMENT)) == NULL)
      return 0 ;
				/* open file to be transferred */
   if ((fp = fopen(path, "r")) == NULL)
    {
       free(data) ;
       return 0 ;
    }
   while (size > 0)
    {				/* read file and send data */
       toread = (size >= SEGMENT) ? SEGMENT : size ;
       if (fread(data, 1, toread, fp) == toread)
	  if (writen(sd, data, toread))
	   {
	      size -= toread ;
	      continue ;
	   }
       free(data) ;
       fclose(fp) ;
       return 0 ;
    }
   free(data) ;
   fclose(fp) ;
   return 1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int receiveFile(int sd, char *path, long size)
  Parameters  : sd   - socket descriptor of connection to CID
                path - destination file path in local m/c
		size - file size
  Returns     : 1 - file successfully received [or]
                0 - file transfer error
  Called by   : file_put
  Calls       : malloc, free, fwrite, readn
  Notes       : The file is received in blocks of size SEGMENT using sd.
  Date        : August '93
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int receiveFile(sd, path, size)
int sd; 
char *path; 
long size;
{   
   FILE *fp ;
   char *data ;
   int towrite ;

   if ((data = (char *) malloc(SEGMENT)) == NULL)
      return 0 ;
				/* open file for storage */
   if ((fp = fopen(path, "w")) == NULL)
    {
       free(data) ;
       return 0 ;
    }
   while (size > 0)
    {				/* receive data and store in file */
       towrite = (size >= SEGMENT) ? SEGMENT : size ;
       if (readn(sd, data, towrite))       
	  if (fwrite(data, 1, towrite, fp) == towrite)
	   {
	      size -= towrite ;
	      continue ;
	   }
       free(data) ;
       fclose(fp) ;
       unlink(path) ;
       return 0 ;
    }
   free(data) ;
   fclose(fp) ;
   return 1 ;
}


/*---------------------------------------------------------------------------
  Prototype   : int connectCid(char *host, char *login)
  Parameters  : host - IP address in dot format [decimal]
  Returns     : socket descriptor of connection to CID [or] -1 on error
  Called by   : pingCid, verify_user, file_put, file_get, file_remove
  Calls       : get_socket, do_connect, inet_addr
  Notes       : Connection is made to the CID in the specified m/c.
  Date        : August '93 
  Coded by    : N. Isaac Rajkumar
  Modification:
---------------------------------------------------------------------------*/  

int connectCid(host,login)
char *host;
char *login;
{
   int sock ;
   u_long cid_host ;
   struct hostent *hostT ;
   u_short cidport;
   char mapid[MAP_LEN];

                                /* determine address of CID */
   if (isdigit(host[0]))
      cid_host = inet_addr(host) ;
   else
    {
       if ((hostT = gethostbyname(host)) == NULL)
        {
           printf("connectCid::gethostbyname\n") ;
	   return -1;
        }
       cid_host = *((long *)hostT->h_addr_list[0]) ;
    }
                                
   if ((sock = get_socket()) == -1)
    {
       	printf("connectCid::get_socket\n") ;
	return -1;
    }
   sprintf(mapid,"sng$cid$%s",login);
   if (!(cidport = pmd_getmap(mapid, cid_host, (u_short)PMD_PROT_TCP)))
    {
        printf("\t\tPMD down (%s).\n",host);
	return -1;
    }
   if (!do_connect(sock, cid_host, cidport))
    {
       printf("\t\tCid down (%s)\n",host) ;
       return -1 ;
    }
   return sock ;
}

/* add new functions here */ 


/*---------------------------------------------------------------------------
  Prototype   : int saveappmsg(char *host, char *app_name, u_short dacport, 
					u_char status, char *login)
  Parameters  : host - IP address in dot format [decimal]
  Returns     : 
  Called by   :
  Calls       :
  Notes       :
  Date        : August' 1994
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

int saveappmsg(host, login, out)
char *host;
char *login;
cid_saveappmsg_it out;
{
	u_short	op ;
	int cidsock;

printf("Before connecting CID (%s) login(%s)\n", host, login);
fflush(stdout);

	if ((cidsock = connectCid(host,login)) == -1)
	{
		printf(" Cannot reach CID \n");
		close(cidsock);
		return 0;
	}
	op = htons(CID_OP_SAVE_APPMSG);
	if (writen(cidsock, (char *)&op, sizeof(u_short)))
	{
		if (!writen(cidsock, (char *)&out, sizeof(cid_saveappmsg_it)))
		{
			printf("Error : CIDLIB SaveAppMsg_Writen_In\n");
			close(cidsock);
			return 0;
		}
	} else {
		printf("Error : CIDLIB SaveAppMsg_Send_Op\n");
		close(cidsock);
		return 0;
	}
	close(cidsock);
	return 1;
}


/*---------------------------------------------------------------------------
  Prototype   : int changeappmsg(char *host, char *app_name, u_char status, char *login)
  Parameters  : host - IP address in dot format [decimal]
  Returns     :
  Called by   :
  Calls       :
  Notes       :
  Date        : August' 1994
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

int changeappmsg(host, login, out)
char *host;
char *login;
cid_changeappmsg_it out;
{
	int cidsock;
	u_short op;

        if ((cidsock = connectCid(host,login)) == -1)
        {
                printf(" Cannot reach CID \n");
		close(cidsock);
                return 0;
        }
        op = htons(CID_OP_CHANG_APPMSG);
        if (writen(cidsock, (char *)&op, sizeof(u_short)))
	{	
        	if (!writen(cidsock, (char *)&out, sizeof(cid_changeappmsg_it)))
		{
                	printf("Error : CIDLIB ChangeAppMsg_Writen_In\n");
			close(cidsock);
			return 0;
		}
        } else {
                printf("Error : CIDLIB ChangeAppMsg_Send_Op\n");
                close(cidsock);
                return 0;
        }
        close(cidsock);
        return 1;
}


/*---------------------------------------------------------------------------
  Prototype   : int getapplist(char *host, appmsg_t **list_p, appmsg_t **end_p,
					int *list_cnt, char *login)
  Parameters  : host - IP address in dot format [decimal]
  Returns     :
  Called by   :
  Calls       :
  Notes       :
  Date        : August' 1994
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

int getapplist(host, login, list_p, end_p, list_cnt)
char *host;
char *login;
appmsg_t **list_p, **end_p;
int *list_cnt;
{
	cid_getapplist_ot in;
	appmsg_t *p_q;
	int i, j, k;
        int cidsock;
        u_short op ;

        if ((cidsock = connectCid(host,login)) == -1)
        {
/*
                printf("CID absent. Please start it. \n");
*/
		close(cidsock);
                return 0;
        }
        op = htons(CID_OP_GET_APPLIST);
        if (writen(cidsock, (char *)&op, sizeof(u_short)))
        {
		i = j = k = 0;
 		if (!readn(cidsock, (char *)&in, sizeof(cid_getapplist_ot))) {
                	printf("Error : CIDLIB GetAppList_Readn_In\n");
			close(cidsock);
                	return 0;
        	}
		in.list_cnt = ntohs(in.list_cnt);
		(*list_cnt) = in.list_cnt;
		if  (in.list_cnt > 0) {
		    while ((*list_cnt) > BUFFE_SIZE * j) {
			j++;
        		for ( ; i < BUFFE_SIZE * j && i < (*list_cnt); i ++) {
               			if ((p_q = (appmsg_t *) malloc(sizeof(appmsg_t))) 
						== NULL)
                        		exit(E_MALLOC);
				k = i - BUFFE_SIZE * (j - 1);
				strcpy(p_q->app_id, (in.buffer[k]).app_id);
                		p_q->dacport = (in.buffer[k]).dacport;
                		p_q->app_status = (in.buffer[k]).app_status;
				p_q->num_process = ntohs((in.buffer[k]).num_process);
				p_q->next = NULL;
                		strcpy(p_q->start_time, (in.buffer[k]).start_time);
                		strcpy(p_q->complete_time, (in.buffer[k]).complete_time);

				if (i == 0)
					(*list_p) = (*end_p) = p_q;
				else {
					(*list_p)->next = p_q;
					(*list_p) = p_q;
				}
        		}
			if ((*list_cnt) > BUFFE_SIZE * j)
				if (!readn(cidsock, (char *)&in, sizeof(cid_getapplist_ot))) {
           		        	printf("Error : CIDLIB GetAppList_Readn_In\n");
	               			close(cidsock);
					return 0;
        			}
		    }
		}
        } else {
                printf("Error : CIDLIB GetAppList_Send_Op\n");
                close(cidsock);
                return 0;
        }
        close(cidsock);
        return 1;
}


void free_app_list(list_pp, end_pp, list_cntp)
appmsg_t **list_pp;
appmsg_t **end_pp;
int *list_cntp;
{
        appmsg_t *p_q1, *p_q2;

        p_q1 = p_q2 = (*end_pp);
        while (p_q1 != NULL) {
                p_q1 = p_q2->next;
                free(p_q2);
                p_q2 = p_q1;
        }
        (*list_pp) = (*end_pp) = NULL;
        (*list_cntp) = 0;
}


/*---------------------------------------------------------------------------
  Prototype   : int getapp(char *host, char *login, char *app_name, 
		int parent_pid, cid_getapp_ot *p_q)
  Parameters  : host - IP address in dot format [decimal]
  Returns     :
  Called by   :
  Calls       :
  Notes       :
  Date        : August' 1994
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

int getapp(host, login, app_name, p_q)
char *host;
char *login;
char *app_name;
cid_getapp_ot *p_q;
{
        cid_getapp_ot in;
	char appname[NAME_LEN];
	int	k;
        int cidsock;
        u_short op ;

        if ((cidsock = connectCid(host,login)) == -1)
        {
                printf(" Cannot reach CID \n");
		close(cidsock);
                return 0;
        }
        op = htons(CID_OP_GET_APP);
	strcpy(appname,app_name);
        if (writen(cidsock, (char *)&op, sizeof(u_short)))
        {
        	if (!writen(cidsock, appname, sizeof(appname))) {
                	printf("Error : CIDLIB GetApp_Writen_Appnm\n");
			close(cidsock);
			return 0;
		}
        	if (!readn(cidsock, (char *)&in, sizeof(cid_getapp_ot))) {
                	printf("Error : CIDLIB GetApp_Readn_In\n");
                	close(cidsock);
			return 0;
        	}
		*p_q = in;
        } else {
                printf("Error : CIDLIB GetApp_Send_Op\n");
                close(cidsock);
                return 0;
        }
        close(cidsock);
        return 1;
}


/*---------------------------------------------------------------------------
  Prototype   : int delapp(char *host, char *login, char *app_name)
  Parameters  : host - IP address in dot format [decimal]
  Returns     :
  Called by   :
  Calls       :
  Notes       :
  Date        : August' 1994
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/

int delapp(host, login, app_name)
char *host;
char *login;
char *app_name;
{
        char appname[NAME_LEN];
        int cidsock;
        u_short op ;

        if ((cidsock = connectCid(host,login)) == -1)
        {
                printf(" Cannot reach CID \n");
		close(cidsock);
                return 0;
        }
        op = htons(CID_OP_DEL_APP);
        if (writen(cidsock, (char *)&op, sizeof(u_short)))
        {
                sprintf(appname, "%s", app_name);
        	if (!writen(cidsock, appname, sizeof(appname))) {
                	printf("Error : CIDLIB DelApp_Writen_Appnm\n");
			close(cidsock);
       	        	return 0;
        	}
        } else {
                printf("Error : CIDLIB DelApp_Send_Op\n");
                close(cidsock);
                return 0;
        }
        close(cidsock);
        return 1;
}


/*---------------------------------------------------------------------------
  Prototype   : int delapplist(char *host,char *login)
  Parameters  : host - IP address in dot format [decimal]
  Returns     :
  Called by   :
  Calls       :
  Notes       :
  Date        : August' 1994
  Coded by    : Feijian Sun
  Modification:
---------------------------------------------------------------------------*/
int delapplist(host, login)
char *host;
char *login;
{
        char appname[NAME_LEN];
	int cidsock;
        u_short op ;

        if ((cidsock = connectCid(host,login)) == -1)
        {
                printf(" Cannot reach CID \n");
		close(cidsock);
                return 0;
        }
        op = htons(CID_OP_DEL_APPLIST);
        if (!writen(cidsock, (char *)&op, sizeof(u_short)))
        {
                printf("Error : CIDLIB DelAppList_Send_Op\n");
                close(cidsock);
                return 0;
        }
        close(cidsock);
        return 1;
}

char *resource_check(host, login)
char *host;
char *login;
{
        char appname[NAME_LEN];
	int cidsock;
        u_short op ;
	static char ch[100];

        if ((cidsock = connectCid(host,login)) == -1)
        {
                printf(" Cannot reach CID \n");
		close(cidsock);
                return 0;
        }
        op = htons(CID_OP_RESOURCE_CK);
        if (!writen(cidsock, (char *)&op, sizeof(u_short)))
        {
                printf("Error : CIDLIB DelAppList_Send_Op\n");
                close(cidsock);
                return 0;
        }
        if (!readn( cidsock, ch, 100 )) 
	{
		perror("Report socket read error");
            	exit(0);
	}
        close(cidsock);
	return ch;
}

/* connectCid2 uses u_long host id directly */
int connectCid2(host,login)
u_long host;
char *login;
{
   int sock ;
   u_short cidport;
   char mapid[MAP_LEN];

                                /* get socket and connect to CID */
   if ((sock = get_socket()) == -1)
    {
       	printf("connectCid::get_socket\n") ;
	return -1;
    }
   sprintf(mapid,"sng$cid$%s",login);
   if (!(cidport = pmd_getmap(mapid, host, (u_short)PMD_PROT_TCP)))
    {
        printf("Processor (%ld) cannot be reached.\n",host);
	return -1;
    }
   if (!do_connect(sock, host, cidport))
    {
       printf("connectCid::do_connect\n") ;
       return -1 ;
    }
   return sock ;
}
