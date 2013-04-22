/*.........................................................................*/
/*                  SNGDTEST.C ------> SNGD test program                   */
/*                                                                         */
/*                  By N. Isaac Rajkumar [April '93]                       */
/*                 February '13, updated by Justin Y. Shi                  */
/*.........................................................................*/

#include "cidtest.h"
#include <time.h>
int status;

int main(argc, argv)
int argc; 
char **argv;
{
   static void (*op_func[])() = {
      OpNull, OpExecTsh, OpExecTsh, OpExecComponent, OpKill, OpKillApp,
      OpKillAll, OpGetApptable, OpGetFulltable, OpPing, OpPingApp, 
      OpRemove, OpFilePut, OpFileGet, OpAddUser, OpDeleteUser, OpVerifyUser, 
      OpVerifyProcess, OpVerifyHandler, OpExit, OpExitProcess, OpResourceCk,
      OpSaveAppMsg, OpChangeAppMsg, OpGetAppList, OpGetApp, OpDelAppList,
      OpDelApp
      } ;
   static void (*pmdop_func[])() = {
      pmd_OpNull, OpMap, OpUnmap, OpGetMap, OpGetTable, OpPurgeTable, OpExit
      } ;
   int pmd_loop; 

   static void (*tshop_func[])() = {
      OpPut, OpGet, OpGet
      } ;
   int tsh_loop;

   if (argc < 3)
    {
       printf("Usage : %s <cid-host> <login>\n", argv[0]) ;
       exit(1) ;
    }
   strcpy(CID_HOST, argv[1]) ;	/* argv[1] is CID host */
   strcpy(CID_LOGIN, argv[2]);  /* argv[2] is CID login */
   
   while (TRUE)
    {
       this_op = drawMenu();
       if (this_op == 29) /* Display PMD menu */
       {
            pmd_loop = 1;
	    while (pmd_loop)
	    {
                this_op = drawPMDMenu();
                this_op = this_op + PMD_OP_MIN - 1 ;
                if (this_op >= PMD_OP_MIN && this_op <= PMD_OP_MAX)
                {
                   this_op = htons(this_op) ;
                   pmdsock = connectCid() ;
                   if (!writen(pmdsock, (char *)&this_op, sizeof(this_op)))
                   {
                      printf("main::writen\n") ;
                      exit(1) ;
                   }
                   (*pmdop_func[ntohs(this_op) - PMD_OP_MIN])() ;
                   close(pmdsock) ;
                }                       /* validate operation & process */
                else
                   pmd_loop = 0 ;
            } 
       } else if (this_op == 30) /* Display TSH menu */
       {
            tsh_loop = 1;
            while (tsh_loop)
            {
                this_op = drawTSHMenu();
                this_op = this_op + TSH_OP_MIN - 1 ;
                if (this_op >= TSH_OP_MIN && this_op <= TSH_OP_MAX)
                {
                   this_op = htons(this_op) ;
                   tshsock = connectCid() ;
                   if (!writen(tshsock, (char *)&this_op, sizeof(this_op)))
                   {
                      printf("main::writen\n") ;
                      exit(1) ;
                   }
                   (*tshop_func[ntohs(this_op) - TSH_OP_MIN])() ;
                   close(tshsock) ;
                }                       /* validate operation & process */
                else
                   tsh_loop = 0 ;
            } 
       } else
       {
         this_op = this_op + CID_OP_MIN - 1 ;
         if (this_op >= CID_OP_MIN && this_op <= CID_OP_MAX)
          {
	   this_op = htons(this_op); 
	   cidsock = connectCid() ;
	   if (!writen(cidsock, (char *)&this_op, sizeof(this_op)))
	    {
	       printf("main::writen\n") ;
	       exit(1) ;
	    }
	   (*op_func[ntohs(this_op) - CID_OP_MIN])() ;
	   close(cidsock) ;
	  }			/* validate operation & process */
         else 
	  return 0 ;
      }
    }
}


u_short drawMenu()
{
   int choice ;

   status=system("clear") ;
   printf("\n\t\t\tS-N-G-D MENU") ;
   printf("\n\t\t\t----------") ;
   printf("\n\n\n\t 1. Null") ;
   printf("\t\t 2. Exec TSH") ;
   printf("\n\t 3. Exec FAH") ;
   printf("\t\t 4. Exec Component") ;
   printf("\n\t 5. Kill") ;
   printf("\t\t 6. Kill App") ;
   printf("\n\t 7. Kill All") ;
   printf("\t\t 8. Get App Table") ;
   printf("\n\t 9. Get Full Table") ;
   printf("\t10. Ping") ;
   printf("\n\t11. Ping App") ;
   printf("\t\t12. Remove") ;
   printf("\n\t13. Put File") ;
   printf("\t\t14. Get File") ;
   printf("\n\t15. Add user") ;   
   printf("\t\t16. Delete user") ;   
   printf("\n\t17. Verify user") ;   
   printf("\t\t18. Verify Process") ;   
   printf("\n\t19. Verify handler") ;
   printf("\t20. Terminate SNGD") ;
   printf("\n\t21. Exit Process"); 
   printf("\n\t22. Resource check (cpu|mem|disk|network)") ;
   printf("\n\t23. Save Application Id");
   printf("\n\t24. Change Application Status");
   printf("\n\t25. Get Application List");
   printf("\n\t26. Get Application");
   printf("\n\t27. Delete Application List");
   printf("\n\t28. Delete Application");
   printf("\n\t29. Port Mapper Functions");
   printf("\n\t30. Local TSH Functions");
   printf("\n\t31. Quit from this program");

   printf("\n\n\nEnter Choice : ") ;
   status=scanf("%d", &choice) ;

   return choice ;
}


void OpNull()
{
   cid_null_ot in ;
   long tmp;

   status=system("clear") ;
   printf("CID_OP_NULL") ;
   printf("\n-----------\n") ;
				/* read data from CID */
printf("CIDTEST. cid_null_ot size=(%ld)\n",sizeof(in));
   if (!readn(cidsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpNull::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print data from CID */
   printf("\n\nFrom CID :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
printf("CIDTEST. in.start_time1  ----(%d)\n", in.start_time);
   printf("\nerror : %d", ntohs(in.error)) ;
   in.start_time = ntohl(in.start_time) ;
   tmp = in.start_time;
printf("CIDTEST. in.start_time2  ---- (%d)\n", in.start_time);
   printf("\nstart_time : %s\n", ctime(&tmp)) ;
   getchar() ;  getchar() ;
}


void OpExecTsh()
{
   cid_exectsh_it out1 ;
   cid_exectsh_ot in1 ;
   tsh_start_it out2 ;
   tsh_start_ot in2 ;
   
   status=system("clear") ;
   printf("CID_OP_EXEC_TSH") ;
   printf("\n---------------\n") ;
				/* obtain login, rlogin, appid, name */
   printf("\nEnter login : ") ;
   status=scanf("%s", out1.login) ;
   getchar() ;
   printf("Enter rlogin : ") ;
   status=scanf("%s", out1.rlogin) ;
   getchar() ;
   printf("Enter application id : ") ;
   status=scanf("%s", out1.appid) ;
   getchar() ;
   printf("Enter component name : ") ;
   status=scanf("%s", out1.name) ;

   printf("\n\nTo CID :\n") ;
   printf("\nlogin : %s", out1.login) ;
   printf("\nrlogin : %s", out1.rlogin) ;
   printf("\nappid : %s", out1.appid) ;
   printf("\nname : %s", out1.name) ;
				/* send data to CID */
   if (!writen(cidsock, (char *)&out1, sizeof(out1)))
    {
       printf("\nOpExecTsh::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* read from CID whwther execed */
   if (!readn(cidsock, (char *)&in1, sizeof(in1)))
    {
       printf("\nOpExecTsh::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result form CID */
   printf("\n\nFrom CID :\n") ;
   printf("\nstatus : %d", ntohs(in1.status)) ;
   printf("\nerror : %d\n", ntohs(in1.error)) ;

   if (ntohs(in1.status) == FAILURE)
    {
       getchar() ;  getchar() ;
       return ;
    }
				/* send TSH/FAH init data */
   strcpy(out2.name, out1.name) ;
   strcpy(out2.appid, out1.appid) ;
   if (!writen(cidsock, (char *)&out2, sizeof(out2)))
    {
       printf("\nOpExecTsh::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* read response from TSH/FAH */
   if (!readn(cidsock, (char *)&in2, sizeof(in2)))
    {
       printf("\nOpExecTsh::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print data from TSH/FAH */
   printf("\n\nFrom TSH/FAH :\n") ;
   printf("\nstatus : %d", ntohs(in2.status)) ;
   printf("\nerror : %d", ntohs(in2.error)) ;
   printf("\nport : %d\n", ntohs(in2.port)) ;
   
   getchar() ; getchar() ;
}


void OpExecComponent()
{
   cid_execcomponent_it out ;
   cid_execcomponent_ot in1 ;
   dac_exitprocess_it in2 ;
   int sd ;
   int newsd ;

   status=system("clear") ;
   printf("CID_OP_EXEC_COMPONENT") ;
   printf("\n---------------------\n") ;
				/* obtain login, rlogin, appid, name, path */
   printf("\nEnter login : ") ;
   status=scanf("%s", out.login) ;
   getchar() ;
   printf("Enter rlogin : ") ;
   status=scanf("%s", out.rlogin) ;
   getchar() ;
   printf("Enter application id : ") ;
   status=scanf("%s", out.appid) ;
   getchar() ;
   printf("Enter component name : ") ;
   status=scanf("%s", out.name) ;
   getchar() ;
   printf("Enter component path : ") ;
   status=scanf("%s", out.path) ;
   strcpy(out.args[0], "N Isaac Rajkumar") ;
   out.arg_cnt = htons(1) ;
				/* obtain return port */
   if ((sd = get_socket()) == -1)
    {
       printf("\nOpExecComponent::get_socket\n") ;
       getchar() ; getchar() ;
       return ;
    }
   if (!(out.port = bind_socket(sd, 0)))
    {
       printf("\nOpExecComponent::bind_socket\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print data sent to CID */
   printf("\n\nTo CID :\n") ;
   printf("\nlogin : %s", out.login) ;
   printf("\nrlogin : %s", out.rlogin) ;
   printf("\nappid : %s", out.appid) ;
   printf("\nname : %s", out.name) ;
   printf("\npath : %s", out.path) ;
   printf("\narg_cnt : %d", ntohs(out.arg_cnt)) ;
   printf("\nargs[0] : %s", out.args[0]) ;
   printf("\nport : %d\n", ntohs(out.port)) ;
				/* send data to CID */
   if (!writen(cidsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpExecComponent::writen\n") ;
       getchar() ; getchar() ;
       close(sd) ;
       return ;
    }
				/* obtain exec result from CID */
   if (!readn(cidsock, (char *)&in1, sizeof(in1)))
    {
       printf("\nOpExecComponent::readn\n") ;
       getchar() ; getchar() ;
       close(sd) ;
       return ;
    }
				/* print exec result from CID */
   printf("\n\nFrom CID :\n") ;
   printf("\nstatus : %d", ntohs(in1.status)) ;
   printf("\nerror : %d\n", ntohs(in1.error)) ;

   if (ntohs(in1.status) == FAILURE)
    {
       getchar() ;  getchar() ;
       close(sd) ;
       return ;
    }
				/* wait for connection from CID */
   newsd = get_connection(sd, NULL) ;
   if (!readn(newsd, (char *)&in2, sizeof(in2)))
      printf("\nOpExecComponent::readn\n") ;
   else
    {
       printf("\nname : %s", in2.name) ;
       printf("\nretstatus : %d\n", in2.retstatus) ;
    }
   close(sd) ;
   close(newsd) ;
   getchar() ; getchar() ;
}


void OpKill()
{
   cid_kill_it out ;
   cid_kill_ot in ;
   
   status=system("clear") ;
   printf("CID_OP_KILL") ;
   printf("\n-----------\n") ;
				/* obtain appid, name */
   printf("Enter application id : ") ;
   status=scanf("%s", out.appid) ;
   getchar() ;
   printf("Enter component name : ") ;
   status=scanf("%s", out.name) ;
				/* print data sent to CID */
   printf("\n\nTo CID :\n") ;
   printf("\nappid : %s", out.appid) ;
   printf("\nname : %s\n", out.name) ;
				/* send data to CID */
   if (!writen(cidsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpKill::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* read result from CID */
   if (!readn(cidsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpKill::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from CID */
   printf("\n\nFrom CID :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d\n", ntohs(in.error)) ;
   getchar() ;  getchar() ;
}
	  
	 
void OpKillApp()
{
   cid_killapp_it out ;
   cid_killapp_ot in ;

   status=system("clear") ;
   printf("CID_OP_KILL_APP") ;
   printf("\n---------------\n") ;

   printf("Enter application id : ") ;
   status=scanf("%s", out.appid) ;
				/* obtain appid */
   printf("\n\nTo CID :\n") ;
   printf("\nappid : %s", out.appid) ;
				/* send appid to CID */
   if (!writen(cidsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpKillApp::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* read result from CID */
   if (!readn(cidsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpKillApp::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from CID */
   printf("\n\nFrom CID :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   printf("\ncount : %d\n", ntohs(in.count)) ;
   getchar() ;  getchar() ;
}


void OpKillAll()
{
   cid_killall_ot in ;
   
   status=system("clear") ;
   printf("CID_OP_KILL_ALL") ;
   printf("\n---------------\n") ;
				/* read result from CID */
   if (!readn(cidsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpKillAll::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from CID */
   printf("\n\nFrom CID :\n") ;
   printf("\n\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   printf("\ncount : %d\n", ntohs(in.count)) ;
   getchar() ;  getchar() ;
}


void OpGetApptable()
{
   cid_getapptable_ot1 in1 ;
   cid_getapptable_ot2 in2 ;
   cid_getapptable_it out ;
   
   status=system("clear") ;
   printf("CID_OP_GET_APPTABLE") ;
   printf("\n-------------------\n") ;
				/* obtain appid */
   printf("Enter application id : ") ;
   status=scanf("%s", out.appid) ;
				/* print data sent to CID */
   printf("\n\nTo CID :\n") ;
   printf("\nappid : %s", out.appid) ;
				/* send appid to CID */
   if (!writen(cidsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpGetApptable::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* read result from CID */
  if (!readn(cidsock, (char *)&in1, sizeof(in1)))
   {
      printf("\nOpGetApptable::readn\n") ;
      getchar() ; getchar() ;
      return ;
   }
				/* print result form CID */
   printf("\n\nFrom CID :\n") ;
   printf("\n\nstatus : %d", ntohs(in1.status)) ;
   printf("\nerror : %d", ntohs(in1.error)) ;
   printf("\ncount : %d\n", ntohs(in1.count)) ;
				/* read & print entries from CID */
   in1.count = ntohs(in1.count) ;
   while (in1.count--)
    {
       if (!readn(cidsock, (char *)&in2, sizeof(in2)))
	{
	   printf("\nOpGetApptable::readn\n") ;
	   getchar() ; getchar() ;
	   return ;
	}
       printf("\nname : %s", in2.name) ;
       printf("\ntype : %d\n", ntohs(in2.type)) ;
    }
  getchar() ; getchar() ;
}


void OpGetFulltable()
{
   cid_getfulltable_ot1 in1 ;
   cid_getfulltable_ot2 in2 ;
   
   status=system("clear") ;
   printf("CID_OP_GET_FULLTABLE") ;
   printf("\n--------------------\n") ;
				/* read result from CID */
  if (!readn(cidsock, (char *)&in1, sizeof(in1)))
   {
      printf("\nOpGetFulltable::readn\n") ;
      getchar() ; getchar() ;
      return ;
   }
				/* print result from CID */
   printf("\n\nFrom CID :\n") ;
   printf("\n\nstatus : %d", ntohs(in1.status)) ;
   printf("\nerror : %d", ntohs(in1.error)) ;
   printf("\ncount : %d\n", ntohs(in1.count)) ;
				/* print entries in table from CID */
   in1.count = ntohs(in1.count) ;
   while (in1.count--)
    {
       if (!readn(cidsock, (char *)&in2, sizeof(in2)))
	{
	   printf("\nOpGetFulltable::readn\n") ;
	   getchar() ; getchar() ;
	   return ;
	}
       printf("\nappid : %s", in2.appid) ;
       printf("\nname : %s", in2.name) ;
       printf("\ntype : %d\n", ntohs(in2.type)) ;
    }
  getchar() ; getchar() ;
}

void OpPing()
{
   cid_ping_it out ;
   cid_ping_ot in ;
   
   status=system("clear") ;
   printf("CID_OP_PING") ;
   printf("\n-----------\n") ;
				/* obtain appication id, component name */
   printf("Enter application id : ") ;
   status=scanf("%s", out.appid) ;
   getchar() ;
   printf("Enter component name : ") ;
   status=scanf("%s", out.name) ;
				/* print data sent to CID */
   printf("\n\nTo CID :\n") ;
   printf("\nappid : %s", out.appid) ;
   printf("\nname : %s\n", out.name) ;
				/* send data to CID */
   if (!writen(cidsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpPing::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* read result from CID */
   if (!readn(cidsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpPing::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from CID */
   printf("\n\nFrom CID :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d\n", ntohs(in.error)) ;
   getchar() ;  getchar() ;
}
	  
	 
void OpPingApp()
{
   cid_pingapp_it out ;
   cid_pingapp_ot in ;

   status=system("clear") ;
   printf("CID_OP_PING_APP") ;
   printf("\n---------------\n") ;
				/* obtain appid */
   printf("Enter application id : ") ;
   status=scanf("%s", out.appid) ;
				/* print data sent to CID */
   printf("\n\nTo CID :\n") ;
   printf("\nappid : %s", out.appid) ;
				/* send data to CID */
   if (!writen(cidsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpPingApp::writen\n") ;
       getchar() ; getchar() ;
      return ;
    }
				/* read result from CID */
   if (!readn(cidsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpPingApp::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from CID */
   printf("\n\nFrom CID :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   printf("\ncount : %d\n", ntohs(in.count)) ;
   getchar() ;  getchar() ;
}


void OpRemove()
{
   cid_remove_ot in ;
   cid_remove_it out ;
   
   status=system("clear") ;
   printf("CID_OP_REMOVE") ;
   printf("\n-------------\n") ;
				/* obtain login, rlogin, path */
   printf("\nEnter login : ") ;
   status=scanf("%s", out.login) ;
   getchar() ;
   printf("Enter rlogin : ") ;
   status=scanf("%s", out.rlogin) ;
   getchar() ;
   printf("Enter path : ") ;
   status=scanf("%s", out.path) ;
				/* print data sent to CID */
   printf("\n\nTo CID :\n") ;
   printf("\nlogin : %s", out.login) ;
   printf("\nrlogin : %s", out.rlogin) ;
   printf("\npath : %s\n", out.path) ;
				/* send data to CID */
   if (!writen(cidsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpRemove::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* read result from CID */
   if (!readn(cidsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpRemove::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from CID */
   printf("\n\nFrom CID :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d\n", ntohs(in.error)) ;
   getchar() ;  getchar() ;
}


void OpFilePut()
{
   cid_fileput_it out ;
   cid_fileput_ot in ;
   char path[PATH_LEN] ;
   struct stat buff ;
   FILE *fp ;
   char *data ;
   int toread ;

   status=system("clear") ;
   printf("CID_OP_PUT_FILE") ;
   printf("\n---------------\n") ;
				/* obtain login, rlogin, path */
   printf("\nEnter login : ") ;
   status=scanf("%s", out.login) ;
   getchar() ;
   printf("Enter rlogin : ") ;
   status=scanf("%s", out.rlogin) ;
   getchar() ;
   printf("Enter remote path : ") ;
   status=scanf("%s", out.path) ;
   getchar() ;
   printf("Enter local path : ") ;
   status=scanf("%s", path) ;
				/* obtain file size */
   if (stat(path, &buff) == -1)
    {
       printf("\nOpPutFile::stat\n") ;
       getchar() ; getchar() ;
       return ;
    }
   out.len = htonl(buff.st_size) ;
				/* open source file */
   if ((fp = fopen(path, "r")) == NULL)
    {
       printf("\nOpPutFile::fopen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print data sent to CID */
   printf("\n\nTo CID :\n") ;
   printf("\nlogin : %s", out.login) ;
   printf("\nrlogin : %s", out.rlogin) ;
   printf("\npath : %s", out.path) ;
   printf("\nlen : %ud\n", ntohl(out.len)) ;
				/* send data to CID */
   if (!writen(cidsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpPutFile::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }

				/* send data from file in SEGMENTS */
   data = (char *) malloc(SEGMENT) ;
   while (buff.st_size > 0)
    {
       toread = (buff.st_size >= SEGMENT) ? SEGMENT : buff.st_size ;
       if (fread(data, 1, toread, fp) == toread)
	  if (writen(cidsock, (char *)data, toread))
	   {
	      buff.st_size -= toread ;
	      continue ;
	   }
       free(data) ;
       printf("OpPutFile::fread/writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
   free(data) ;
				/* read result from CID */
   if (!readn(cidsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpPutFile::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from CID */
   printf("\n\nFrom CID :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d\n", ntohs(in.error)) ;
   getchar() ;  getchar() ;
}


void OpFileGet()
{
   cid_fileget_it out ;
   cid_fileget_ot in ;
   char path[PATH_LEN] ;
   FILE *fp ;
   char *data ;
   int towrite ;

   status=system("clear") ;
   printf("CID_OP_GET_FILE") ;
   printf("\n---------------\n") ;
				/* obtain login, rlogin, path */
   printf("\nEnter login : ") ;
   status=scanf("%s", out.login) ;
   getchar() ;
   printf("Enter rlogin : ") ;
   status=scanf("%s", out.rlogin) ;
   getchar() ;
   printf("Enter remote path : ") ;
   status=scanf("%s", out.path) ;
   getchar() ;
   printf("Enter local path : ") ;
   status=scanf("%s", path) ;
				/* open dest file */
   if ((fp = fopen(path, "w")) == NULL)
    {
       printf("\nOpGetFile::fopen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print data sent to CID */
   printf("\n\nTo CID :\n") ;
   printf("\nlogin : %s", out.login) ;
   printf("\nrlogin : %s", out.rlogin) ;
   printf("\npath : %s", out.path) ;
				/* send data to CID */
   if (!writen(cidsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpGetFile::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }

   if (!readn(cidsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpGetFile::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from CID */
   in.len = ntohl(in.len) ;
   printf("\n\nFrom CID :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   printf("\nlen : %ud\n", in.len) ;
   

				/* read data from file in SEGMENTS */
   data = (char *) malloc(SEGMENT) ;
   while (in.len > 0)
    {
       towrite = (in.len >= SEGMENT) ? SEGMENT : in.len ;
       if (readn(cidsock, (char *)data, towrite))
	  if (fwrite(data, 1, towrite, fp) == towrite)
	   {
	      in.len -= towrite ;
	      continue ;
	   }
       free(data) ;
       printf("OpGetFile::fwrite/readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
   free(data) ;
   getchar() ;  getchar() ;
}


void OpAddUser()
{
   cid_adduser_it out ;
   cid_adduser_ot in ;

   status=system("clear") ;
   printf("CID_OP_ADD_USER") ;
   printf("\n---------------\n") ;
				/* obtain login*/
   printf("\nEnter login : ") ;
   status=scanf("%s", out.login) ;
   printf("\n\nTo CID :\n") ;
   printf("\nlogin : %s", out.login) ;

   if (!writen(cidsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpAddUser::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }

   if (!readn(cidsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpAddUser::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from CID */
   printf("\n\nFrom CID :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   getchar() ; getchar() ;
}

void OpDeleteUser()
{
   cid_deleteuser_it out ;
   cid_deleteuser_ot in ;

   status=system("clear") ;
   printf("CID_OP_DELETE_USER") ;
   printf("\n------------------\n") ;
				/* obtain login */
   printf("\nEnter login : ") ;
   status=scanf("%s", out.login) ;
   printf("\n\nTo CID :\n") ;
   printf("\nlogin : %s", out.login) ;

   if (!writen(cidsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpDeleteUser::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }

   if (!readn(cidsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpDeleteUser::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from CID */
   printf("\n\nFrom CID :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   getchar() ; getchar() ;
}


void OpVerifyUser()
{
   cid_verifyuser_it out ;
   cid_verifyuser_ot in ;

   status=system("clear") ;
   printf("CID_OP_VERIFY_USER") ;
   printf("\n------------------\n") ;
				/* obtain login */
   printf("\nEnter login : ") ;
   status=scanf("%s", out.login) ;
   printf("\n\nTo CID :\n") ;
   printf("\nlogin : %s", out.login) ;

   if (!writen(cidsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpVerifyUser::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }

   if (!readn(cidsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpVerifyUser::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from CID */
   printf("\n\nFrom CID :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   getchar() ; getchar() ;
}


void OpVerifyProcess()
{
   cid_verifyprocess_it out ;
   cid_verifyprocess_ot in ;
   
   status=system("clear") ;
   printf("CID_OP_VERIFY_PROCESS") ;
   printf("\n---------------------\n") ;
				/* obtain login, rlogin, type */
   printf("\nEnter login : ") ;
   status=scanf("%s", out.login) ;
   getchar() ;
   printf("Enter rlogin : ") ;
   status=scanf("%s", out.rlogin) ;
   getchar() ;
   printf("Enter path : ") ;
   status=scanf("%s", out.path) ;
				/* print data sent to CID */
   printf("\n\nTo CID :\n") ;
   printf("\nlogin : %s", out.login) ;
   printf("\nrlogin : %s", out.rlogin) ;
   printf("\npath : %s", out.path) ;
				/* send data to CID */
   if (!writen(cidsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpVerifyProcess::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }

   if (!readn(cidsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpVerifyProcess::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from CID */
   printf("\n\nFrom CID :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   getchar() ; getchar() ;
}


void OpVerifyHandler()
{
   cid_verifyhandler_it out ;
   cid_verifyhandler_ot in ;
   
   status=system("clear") ;
   printf("CID_OP_VERIFY_HANDLER") ;
   printf("\n---------------------\n") ;
				/* obtain login, rlogin, type */
   printf("\nEnter login : ") ;
   status=scanf("%s", out.login) ;
   getchar() ;
   printf("Enter rlogin : ") ;
   status=scanf("%s", out.rlogin) ;
   getchar() ;
   printf("Enter handler [1-TSH, 2-FAH] : ") ;
   status=scanf("%hd", &out.type) ;
   out.type += 100 ;
				/* print data sent to CID */
   printf("\n\nTo CID :\n") ;
   printf("\nlogin : %s", out.login) ;
   printf("\nrlogin : %s", out.rlogin) ;
   printf("\ntype : %d", out.type) ;
   out.type = htons(out.type) ;
				/* send data to CID */
   if (!writen(cidsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpVerifyHandler::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }

   if (!readn(cidsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpVerifyHandler::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from CID */
   printf("\n\nFrom CID :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   getchar() ;   getchar() ; 
}


void OpExit()
{
   cid_exit_ot in ;
   
   status=system("clear") ;
   printf("CID_OP_EXIT") ;
   printf("\n-----------\n") ;
				/* read result from CID */
   if (!readn(cidsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpExit::readn\n") ;
       getchar() ;  getchar() ;
       return ;
    }
				/* print result form CID */
   printf("\n\nFrom CID :\n") ;
   printf("\n\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d\n", ntohs(in.error)) ;
   getchar() ;  getchar() ;
}

void OpExitProcess()
{
	printf("Null operation\n");
	return;
}

void OpResourceCk()
{
	char ch[100];

	printf("---- CID Host[%s] Resource Status ----\n",CID_HOST);
        if (!readn( cidsock, ch, 100 )) 
	{
		perror("Report socket read error");
            	exit(0);
	}
	puts(ch);
    	fflush(stdout);
   	getchar() ;  getchar() ;
}

int connectCid()
{
   int sock ;
   u_long cid_host ;
   struct hostent *host ;
   sng_int16 cidport;
   char mapid[MAP_LEN];

				/* determine address of CID */
   if (isdigit(CID_HOST[0]))
      cid_host = inet_addr(CID_HOST) ;
   else
    {
       if ((host = gethostbyname(CID_HOST)) == NULL)
	{
	   printf("connectCid::gethostbyname\n") ;
	   exit(1) ;
	}
       cid_host = *((long *)host->h_addr_list[0]) ;
    }
				/* get socket and connect to CID */
   if ((sock = get_socket()) == -1)
    {
       printf("connectCid::get_socket\n") ;
       exit(1) ;
    }
   sprintf(mapid,"sng$cid$%s",CID_LOGIN);
   if (!(cidport = pmd_getmap(mapid, cid_host, (u_short)PMD_PROT_TCP)))
    {
	printf("Processor (%s) cannot be reached.\n",CID_HOST);
	exit(1); 		
    }
   if (!do_connect(sock, cid_host, cidport))
    {
       printf("connectCid::do_connect\n") ;
       exit(1) ;
    }      
   return sock ;
}


u_short drawPMDMenu()
{
   int choice ;
                                /* draw menu of user options */
   status=system("clear") ;
   printf("\n\n\n\t\t\t---------") ;
   printf("\n\t\t\tMAIN MENU") ;
   printf("\n\t\t\t---------") ;
   printf("\n\n\t\t\t1. Null") ;
   printf("\n\t\t\t2. Map") ;
   printf("\n\t\t\t3. Un-map") ;
   printf("\n\t\t\t4. Get map") ;
   printf("\n\t\t\t5. Get table") ;
   printf("\n\t\t\t6. Purge table") ;
   printf("\n\t\t\t8. Back to main menu") ;

   printf("\n\n\n\t\t\tEnter Choice : ") ;

   status=scanf("%d", &choice) ;       /* return user choice */
   return choice ;
}


void pmd_OpNull()
{
   pmd_null_ot in ;
   long tmp;  /* Alpha change */
   
   status=system("clear") ;
   printf("PMD_OP_NULL") ;
   printf("\n-----------\n") ;
   if (!readn(pmdsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpNull::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print results from PMD */
   printf("\n\nFrom PMD :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   printf("\ncount : %d", ntohs(in.count)) ;
   in.start_time = ntohl(in.start_time) ;
/* Alpha change  */
   tmp = in.start_time;
   printf("\nstart_time : %s\n", ctime(&tmp)) ;
   getchar() ;  getchar() ;
}


void OpMap()
{
   pmd_map_ot in ;
   pmd_map_it out ;
   int tmp ;

   status=system("clear") ;
   printf("PMD_OP_MAP") ;
   printf("\n----------\n") ;

   printf("\nEnter mapid : ") ;
   status=scanf("%s", out.mapid) ;
				/* fill data for PMD */
   out.pid = htonl(getpid()) ;
   printf("\nEnter port : ") ;
   status=scanf("%d", &tmp) ;
   out.port = htons((u_short) tmp) ;

   out.protocol = htons(PMD_PROT_TCP) ;
				/* print data sent to PMD */
   printf("\n\nTo PMD :\n") ;
   printf("\nmapid : %s", out.mapid) ;
   printf("\nport : %d", ntohs(out.port)) ;
   printf("\nprotocol : %d", ntohs(out.protocol)) ;
   printf("\npid : %d", ntohl(out.pid)) ;
				/* send data to PMD */
   if (!writen(pmdsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpMap::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* read result */
   if (!readn(pmdsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpMap::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from PMD */
   printf("\n\nFrom PMD :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d\n", ntohs(in.error)) ;
   getchar() ;  getchar() ;
}


void OpUnmap()
{
   pmd_unmap_ot in ;
   pmd_unmap_it out ;
   
   status=system("clear") ;
   printf("PMD_OP_UNMAP") ;
   printf("\n------------\n") ;
				/* umnmap mapid unmapped from PMD */
   printf("\nEnter mapid : ") ;
   status=scanf("%s", out.mapid) ;
   
   out.pid = htonl(0) ;		/* so that only this entry is unmapped */
   out.protocol = htons(PMD_PROT_TCP) ;
				/* print data sent to PMD  */
   printf("\n\nTo PMD :\n") ;
   printf("\nmapid : %s", out.mapid) ;
   printf("\nprotocol : %d", ntohs(out.protocol)) ;
   printf("\npid : %d", 0) ;
				/* send data to PMD */
   if (!writen(pmdsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpUnmap::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* read PMD response */
   if (!readn(pmdsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpUnmap::read\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print PMD response */
   printf("\n\nFrom PMD :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   printf("\ncount : %d\n", ntohs(in.count)) ;
   getchar() ;  getchar() ;
}


void OpGetMap()
{
   pmd_getmap_ot in ;
   pmd_getmap_it out ;
   
   status=system("clear") ;
   printf("PMD_OP_GET_MAP") ;
   printf("\n--------------\n") ;
				/* get mapid for which mapping has to be got */
   printf("\nEnter mapid : ") ;
   status=scanf("%s", out.mapid) ;
   out.protocol = htons(PMD_PROT_TCP) ;
				/* print data sent to PMD */
   printf("\n\nTo PMD :\n") ;
   printf("\nmapid : %s", out.mapid) ;
   printf("\nprotocol : %d", ntohs(PMD_PROT_TCP)) ;
				/* send request to PMD */
   if (!writen(pmdsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpGetMap::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* obtain response from PMD */
   if (!readn(pmdsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpGetMap::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print response from PMD */
   printf("\n\nFrom PMD :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   printf("\nport : %d\n", ntohs(in.port)) ;
   getchar() ;  getchar() ;
}


void OpPurgeTable()
{
   pmd_purgetable_ot in ;
   
   status=system("clear") ;
   printf("PMD_OP_PURGE_TABLE") ;
   printf("\n------------------\n") ;
				/* obtain response from PMD */
   if (!readn(pmdsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpPurgeTable::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print response from PMD */
   printf("\n\nFrom PMD :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   printf("\ncount : %d\n", ntohs(in.count)) ;
   getchar() ;  getchar() ;
}


void OpGetTable()
{
   pmd_gettable_ot1 in1 ;
   pmd_gettable_ot2 in2 ;
   
   status=system("clear") ;
   printf("PMD_OP_GET_TABLE") ;
   printf("\n----------------\n") ;
				/* obtain table length from PMD */
   if (!readn(pmdsock, (char *)&in1, sizeof(in1)))
    {
       printf("\nOpGetTable::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print initial data from PMD */
   printf("\n\nFrom PMD :\n") ;
   printf("\nstatus : %d", ntohs(in1.status)) ;
   printf("\nerror : %d", ntohs(in1.error)) ;
   printf("\ncount : %d\n", ntohs(in1.count)) ;
   
   in1.count = ntohs(in1.count) ;
   while (in1.count--)
    {				/* read table entries from PMD */
       if (!readn(pmdsock, (char *)&in2, sizeof(in2)))
	{
	   printf("\nOpGetTable::readn\n") ;
	   getchar() ; getchar() ;
	   return ;
	}	
				/* print table entries */
       printf("\nmapid : %s", in2.mapid) ;
       printf("\nprotocol : %d", ntohs(in2.protocol)) ;
       printf("\nport : %d\n", ntohs(in2.port)) ;
    }
   getchar() ;  getchar() ;
}


void OpSaveAppMsg()
{
	cid_saveappmsg_it in;

	status=system("clear") ;
	printf("Save Application Id") ;
	printf("\n----------------\n") ;

	printf("\nApplication Id: ");
	status=scanf("%s", in.app_id);
	getchar();
	printf("\nDAC port: ");
	status=scanf("%ud", &in.dacport);
	getchar();
	printf("\nApplication Status: ");
	status=scanf("%c", &in.app_status);
	getchar();

	if (!writen(cidsock, (char *)&in, sizeof(in)))
		printf("Error : CIDTEST saveappmsg\n");
	getchar(); getchar();
}


void OpChangeAppMsg()
{
	
	cid_changeappmsg_it in;

        status=system("clear") ;
        printf("Change Application Status") ;
        printf("\n----------------\n") ;

        printf("\nApplication Id: ");
        status=scanf("%s", in.app_id);
	getchar();
        printf("\nApplication Status: ");
        status=scanf("%c", &in.app_status);
	getchar();
        if (!writen(cidsock, (char *)&in, sizeof(in)))
                printf("Error : CID changeappmsg\n");
        getchar(); getchar();
}


void OpGetAppList()
{
	appmsg_t *list_p, *end_p;
	int list_cnt;
	cid_getapplist_ot in;
	appmsg_t *p_q;
	int i, j, k;

        status=system("clear") ;
        printf("Get Application List") ;
        printf("\n----------------\n") ;

	i = j = k = 0;
        if (!readn(cidsock, (char *)&in, sizeof(in))) {
                printf("Error : CIDTEST OpGetAppList.\n");
                return;
        }
	list_cnt = ntohs(in.list_cnt);
	if (list_cnt <= 0)
	    printf("The application list in CID is NULL.\n");
	else {
	    while (list_cnt > BUFFE_SIZE * j) {
		j++;
        	for ( ; i < BUFFE_SIZE * j && i < list_cnt; i ++) {
               		if ((p_q = (appmsg_t *) malloc(sizeof(appmsg_t))) 
					== NULL)
                        	exit(E_MALLOC);
			k = i - BUFFE_SIZE * (j - 1);
			strcpy(p_q->app_id, (in.buffer[k]).app_id);
                	p_q->dacport = (in.buffer[k]).dacport;
                	p_q->app_status = (in.buffer[k]).app_status;
                	strcpy(p_q->start_time, (in.buffer[k]).start_time);
                	strcpy(p_q->complete_time, (in.buffer[k]).complete_time);
			p_q->next = NULL;
			if (i == 0) {
				list_p = end_p = p_q;
			}
			else {
				list_p->next = p_q;
				list_p = p_q;
			}
        	}
		if (list_cnt > BUFFE_SIZE * j)
			if (!readn(cidsock, (char *)&in, sizeof(in))) {
           	        	printf("Error : CIDTEST OpGetAppList.\n");
	               		break;
        		}
	    }

	    i = 1;
	    p_q = end_p;
	    while (p_q != NULL) {
		printf("APP.(%d).__app_id (%s)_dacport (%u)_status (%c)",
				i, p_q->app_id, p_q->dacport, p_q->app_status);
 
		k = strlen(p_q->start_time); if ( k ) k--;
		p_q->start_time[k] = 0x0;
		k = strlen(p_q->complete_time); if ( k ) k--;
		p_q->complete_time[k] = 0x0;
		printf("\n\t_start (%s) \n\t_stop (%s)\n", 
				p_q->start_time, p_q->complete_time);
		p_q = p_q->next;
	    }
	}
	getchar(); getchar();
}


void OpGetApp()
{
        cid_getapp_ot in;
	char appname[NAME_LEN];
	int	k;

        status=system("clear") ;
        printf("Get Application") ;
        printf("\n----------------\n") ;

	printf("\nApplication Id: ");
	status=scanf("%s", appname);
	getchar();
        if (!writen(cidsock, appname, sizeof(appname))) {
                printf("Error : CIDTEST OpGetApp_writen\n");
		return;
	}

        if (!readn(cidsock, (char *)&in, sizeof(in))) {
                printf("Error : CIDTEST OpGetApp_readn.\n");
                return;
        }
	printf("APP.__app_id (%s)_dacport (%u)_status (%c)", 
			in.app_id, in.dacport, in.app_status);
	k = strlen(in.start_time); if ( k ) k--;
	in.start_time[k] = 0x0;
	k = strlen(in.complete_time); if ( k ) k--;
	in.complete_time[k] = 0x0;
	printf("\n\t_start (%s) \n\t_stop (%s)\n", in.start_time, in.complete_time);
	getchar(); getchar();
} 


void OpDelApp()
{
        char appname[NAME_LEN];

        status=system("clear") ;
        printf("Delete Application") ;
        printf("\n----------------\n") ;

        printf("\nApplication Id: ");
        status=scanf("%s", appname);
        getchar();
        if (!writen(cidsock, appname, sizeof(appname))) {
                printf("Error : CIDTEST OpDelApp_writen\n");
                return;
        }
        getchar(); getchar();
}


void OpDelAppList()
{
        status=system("clear") ;
        printf("Delete Application List") ;
        printf("\n----------------\n") ;

        getchar(); getchar();
}


u_short drawTSHMenu()
{
   int choice ;
				/* draw menu of user options */
   status=system("clear") ;
   printf("\n\n\n\t\t\t---------") ;
   printf("\n\t\t\tMAIN MENU") ;
   printf("\n\t\t\t---------") ;
   printf("\n\n\t\t\t 1. Put") ;
   printf("\n\t\t\t 2. Get") ;
   printf("\n\t\t\t 3. Read") ;
   printf("\n\t\t\t 8. Back to main menu") ;
       
   printf("\n\n\n\t\t\tEnter Choice : ") ;

   status=scanf("%d", &choice) ;	/* return user choice */
   return choice ;
}


void OpPut()
{
   tsh_put_it out ;
   tsh_put_ot in ;
   int tmp ;
   char *buff, *st ;

   status=system("clear") ;
   printf("TSH_OP_PUT") ;
   printf("\n----------\n") ;

/* obtain application id, tuple name, priority, length, and te tuple */
   printf("\nEnter application id : ") ; 
   status=scanf("%s", out.appid) ;
   printf("\nEnter tuple name : ") ; /* and the tuple */
   status=scanf("%s", out.name) ;
   printf("Enter priority : ") ;
   status=scanf("%d", &tmp) ;
   out.priority = (u_short)tmp ;
   printf("Enter length : ") ;
   status=scanf("%d", &out.length) ;
   getchar() ;
   printf("Enter tuple : ") ;
   buff = (char *)malloc(out.length) ;
   st=fgets(buff, out.length, stdin) ;
                                /* print data sent to CID_TSH */
   printf("\n\nTo CID_TSH :\n") ;
   printf("\nname : %s", out.name) ;
   printf("\npriority : %d", out.priority) ;
   printf("\nlength : %d", out.length) ;
   printf("\ntuple : %s\n", buff) ;

   out.priority = htons(out.priority) ;
   out.length = htonl(out.length) ;
                                /* send data to CID_TSH */
   if (!writen(tshsock, (char *)&out, sizeof(out)))
    {
       perror("\nOpPut::writen\n") ;
       getchar() ;
       free(buff) ;
       return ;
    }
                                /* send tuple to CID_TSH */
   if (!writen(tshsock, buff, ntohl(out.length)))
    {
       perror("\nOpPut::writen\n") ;
       getchar() ;
       free(buff) ;
       return ;
    }
				/* read result */
   if (!readn(tshsock, (char *)&in, sizeof(in)))
    {
       perror("\nOpPut::readn\n") ;
       getchar() ;
       return ;
    }
                                /* print result from CID_TSH */
   printf("\n\nFrom CID_TSH :\n") ;
   printf("\nstatus : %d\n", ntohs(in.status)) ;
   printf("\nerror : %d\n", ntohs(in.error)) ;
   getchar() ; getchar() ;
   getchar() ; getchar() ;
   getchar() ; getchar() ;
}


void OpGet()
{
   tsh_get_it out ;
   tsh_get_ot1 in1, out1 ;
   tsh_get_ot2 in2 ;
   struct in_addr addr ;
   int sd, sock ;
   char *buff ;

   status=system("clear") ;
   printf("TSH_OP_GET") ;
   printf("\n----------\n") ;

   printf("\nEnter application id : ") ;
   status=scanf("%s", out.appid) ;
				/* obtain tuple name/wild card */
   printf("\nEnter tuple name [wild cards ?, * allowed] : ") ;
   status=scanf("%s", out.expr) ;
				/* obtain port for return data if tuple */
   out.host = gethostid() ;	/* is not available */
   if ((sd = get_socket()) == -1)
    {
       perror("\nOpGet::get_socket\n") ;
       getchar() ; getchar() ;
       return ;
    }
   if (!(out.port = bind_socket(sd, 0)))
    {
       perror("\nOpGet::bind_socket\n") ;
       getchar() ; getchar() ;
       return ;
    }
   addr.s_addr = out.host ;
                                /* print data  sent to CID_TSH */
   printf("\n\nTo CID_TSH :\n") ;
   printf("\nexpr : %s", out.appid) ;
   printf("\nexpr : %s", out.expr) ;
   printf("\nhost : %s", inet_ntoa(addr)) ;
   printf("\nport : %d\n", ntohs(out.port)) ;
                                /* send data to CID_TSH */
   if (!writen(tshsock, (char *)&out, sizeof(out)))
    {
       perror("\nOpGet::writen\n") ;
       getchar() ; getchar() ;
       close(sd) ;
       return ;
    }
				/* find out if tuple available */
   if (!readn(tshsock, (char *)&in1, sizeof(in1)))
    {
       perror("\nOpGet::readn\n") ;
       getchar() ; getchar() ;
       close(sd) ;
       return ;
    }
				/* print whether tuple available in TSH */
   printf("\n\nFrom CID_TSH :\n") ;
   printf("\nstatus : %d", ntohs(in1.status)) ;
   printf("\nerror : %d\n", ntohs(in1.error)) ;
				/* if tuple is available read from the same */
   if (ntohs(in1.status) == SUCCESS) /* socket */
      sock = tshsock ;
   else				/* get connection in the return port */
      sock = get_connection(sd, NULL) ;
                                /* read tuple details from CID_TSH */
   if (!readn(sock, (char *)&in2, sizeof(in2)))
    {
       perror("\nOpGet::readn\n") ;
       getchar() ; getchar() ;
       close(sd) ;
       return ;
    }                           /* print tuple details from CID_TSH */
   printf("\nappid : %s", in2.appid) ;
   printf("\nname : %s", in2.name) ;
   printf("\npriority : %d", ntohs(in2.priority)) ;
   printf("\nlength : %d", ntohl(in2.length)) ;
   buff = (char *)malloc(ntohl(in2.length)) ;

   if (!readn(sock, buff, ntohl(in2.length)))
   {
       perror("\nOpGet::readn\n") ;
       getchar() ; getchar() ;
       close(sd) ;
       return ;
    }                           /* print  tuple from CID_TSH */
    printf("\ntuple : %s\n", buff) ;

   out1.status = htons(SUCCESS);
   out1.error = htons(TSH_ER_NOERROR) ;
   if (!writen(tshsock, (char *)&out1, sizeof(tsh_get_ot1)))
      perror("\nOpGet::writen\n") ;
   else {
      printf("\n\nTo CID_TSH :\n") ;
      printf("\nstatus : %d", ntohs(out1.status)) ;
      printf("\nerror : %d\n", ntohs(out1.error)) ;
   }

   close(sd) ;
   close(sock) ;
   free(buff) ;
   getchar() ; getchar() ;
}

