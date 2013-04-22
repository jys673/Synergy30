/*.........................................................................*/
/*                  FAHTEST.C ------> FAH test program                     */
/*                                                                         */
/*                  By N. Isaac Rajkumar [April '93]                       */
/*.........................................................................*/

#include "fahtest.h"
int status;

int main(argc, argv)
int argc; 
char **argv;
{
   static void (*op_func[])() = {
      OpFopen, OpFclose, OpFgetc, OpFputc, OpFgets, OpFputs, OpFread, OpFwrite,
      OpFseek, OpFflush, OpExit
      } ;
   u_short this_op ;

   if (argc < 3)
    {
       printf("Usage : %s <fah-host> <appid$component_name>\n", argv[0]) ;
       exit(1) ;
    }
   strcpy(FAH_HOST, argv[1]) ;	/* argv[1] is FAH host */
   strcpy(mapid, argv[2]) ;

   while (TRUE)
    {
       this_op = drawMenu() + FAH_OP_MIN - 1 ;
       if (this_op >= FAH_OP_MIN && this_op <= FAH_OP_MAX)
	{
	   this_op = htons(this_op) ;
	   fahsock = connectFah() ;
	   if (!writen(fahsock, (char *)&this_op, sizeof(this_op)))
	    {
	       printf("main::writen\n") ;
	       exit(1) ;
	    }
	   (*op_func[ntohs(this_op) - FAH_OP_MIN])() ;
	   close(fahsock) ;
	}			/* validate operation & process */
       else
	  return 0 ;
    }
}


void OpFopen()
{
   fah_fopen_ot in ;
   fah_fopen_it out ;

   status=system("clear") ;
   printf("FAH_OP_FOPEN") ;
   printf("\n------------\n") ;
				/* obtain path & access mode */
   printf("\nEnter file-path : ") ;
   status=scanf("%s", out.path) ;
   printf("\nEnter mode : ") ;
   status=scanf("%s", out.mode) ;
				/* print data sent to FAH */
   printf("\n\nTo FAH :\n") ;
   printf("\npath : %s", out.path) ;
   printf("\nmode : %s\n", out.mode) ;
				/* send data to FAH */
   if (!writen(fahsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpFopen::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* read result */
   if (!readn(fahsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpFopen::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from FAH */
   printf("\n\nFrom FAH :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   printf("\nfid : %d\n", ntohs(in.fid)) ;
   getchar() ;  getchar() ;
}


void OpFclose()
{
   fah_fclose_ot in ;
   fah_fclose_it out ;
   int tmp ;

   status=system("clear") ;
   printf("FAH_OP_FCLOSE") ;
   printf("\n-------------\n") ;
				/* obtain file desc. to be closed */
   printf("\nEnter file-id : ") ;
   status=scanf("%d", &tmp) ;
   out.fid = htons((u_short)tmp) ;
				/* print data sent to FAH */
   printf("\n\nTo FAH :\n") ;
   printf("\nfid : %d\n", ntohs(out.fid)) ;
				/* send data to FAH */
   if (!writen(fahsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpFclose::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* read result */
   if (!readn(fahsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpFclose::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from FAH */
   printf("\n\nFrom FAH :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d\n", ntohs(in.error)) ;
   getchar() ;  getchar() ;
}


void OpFgetc()
{
   fah_fgetc_ot in ;
   fah_fgetc_it out ;
   int tmp ;

   status=system("clear") ;
   printf("FAH_OP_FGETC") ;
   printf("\n------------\n") ;
				/* obtain file desc. */
   printf("\nEnter file-id : ") ;
   status=scanf("%d", &tmp) ;
   out.fid = htons((u_short)tmp) ;
				/* print data sent to FAH */
   printf("\n\nTo FAH :\n") ;
   printf("\nfid : %d\n", ntohs(out.fid)) ;
				/* send data to FAH */
   if (!writen(fahsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpFgetc::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* read result */
   if (!readn(fahsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpFgetc::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from FAH */
   printf("\n\nFrom FAH :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   printf("\nch : %c\n", ntohl(in.ch)) ;
   getchar() ;  getchar() ;
}


void OpFputc()
{
   fah_fputc_ot in ;
   fah_fputc_it out ;
   int tmp ;

   status=system("clear") ;
   printf("FAH_OP_FPUTC") ;
   printf("\n------------\n") ;
				/* obtain file desc & character */
   printf("\nEnter file-id : ") ;
   status=scanf("%d", &tmp) ;
   out.fid = htons((u_short)tmp) ;
   getchar() ;
   printf("\nEnter character : ") ;
   status=scanf("%c", &out.ch) ;
				/* print data sent to FAH */
   printf("\n\nTo FAH :\n") ;
   printf("\nfid : %d", ntohs(out.fid)) ;
   printf("\nch : %c\n", out.ch) ;
				/* send data to FAH */
   if (!writen(fahsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpFputc::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* read result */
   if (!readn(fahsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpFputc::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from FAH */
   printf("\n\nFrom FAH :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d\n", ntohs(in.error)) ;
   getchar() ;  getchar() ;
}


void OpFgets()
{
   fah_fgets_ot in ;
   fah_fgets_it out ;
   char *buff ;
   int tmp ;

   status=system("clear") ;
   printf("FAH_OP_FGETS") ;
   printf("\n------------\n") ;
				/* obtain file desc & length of string */
   printf("\nEnter file-id : ") ;
   status=scanf("%d", &tmp) ;
   out.fid = htons((u_short)tmp) ;
   getchar() ;
   printf("\nEnter length : ") ;
   status=scanf("%d", &out.len) ;
   out.len = htonl(out.len) ;
				/* print data sent to FAH */
   printf("\n\nTo FAH :\n") ;
   printf("\nfid : %d", ntohs(out.fid)) ;
   printf("\nlen : %d\n", ntohl(out.len)) ;
				/* send data to FAH */
   if (!writen(fahsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpFgets::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* read result */
   if (!readn(fahsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpFgets::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from FAH */
   printf("\n\nFrom FAH :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   printf("\nlen : %d\n", ntohl(in.len)) ;
				/* read string from FAH */
   buff = (char *)malloc(ntohl(in.len)) ;
   if (!readn(fahsock, buff, ntohl(in.len)))
    {
       printf("\nOpFgets::readn\n") ;
       getchar() ; getchar() ;
       free(buff) ;
       return ;
    }				/* print string */
   printf("\nstring : %s\n", buff) ;
   getchar() ;  getchar() ;
   free(buff) ;
}


void OpFputs()
{
   fah_fputs_ot in ;
   fah_fputs_it out ;
   char buff[80],*st ;
   int tmp ;
   
   status=system("clear") ;
   printf("FAH_OP_FPUTS") ;
   printf("\n------------\n") ;
				/* obtain file desc & string */
   printf("\nEnter file-id : ") ;
   status=scanf("%d", &tmp) ;
   out.fid = htons((u_short)tmp) ;
   getchar() ;
   printf("\nEnter string : ") ;
   st=fgets(buff, sizeof(buff), stdin) ;
   out.len = htonl(strlen(buff)+1) ;
				/* print data sent to FAH */
   printf("\n\nTo FAH :\n") ;
   printf("\nfid : %d", ntohs(out.fid)) ;
   printf("\nlen : %d", ntohl(out.len)) ;
   printf("\nstring : %s\n", buff) ;
				/* send operation data to FAH */
   if (!writen(fahsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpFputs::writen\n") ;
       getchar() ; 
       return ;
    }
				/* send string to FAH */
   if (!writen(fahsock, buff, strlen(buff)+1))
    {
       printf("\nOpFputs::writen\n") ;
       getchar() ; 
       return ;
    }
				/* read result */
   if (!readn(fahsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpFputs::readn\n") ;
       getchar() ; 
       return ;
    }
				/* print result from FAH */
   printf("\n\nFrom FAH :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d\n", ntohs(in.error)) ;
   getchar() ;
}


void OpFread()
{
   fah_fread_ot in ;
   fah_fread_it out ;
   struct {
      char name[25] ;
      int age ;
   } rec ;
   int tmp ;
   
   status=system("clear") ;
   printf("FAH_OP_FREAD") ;
   printf("\n------------\n") ;
				/* obtain file desc */
   printf("\nEnter file-id : ") ;
   status=scanf("%d", &tmp) ;
   out.fid = htons((u_short)tmp) ;
   getchar() ;
   out.nitems = htonl(1) ;
   out.size = htonl(sizeof(rec)) ;
   
   printf("\n\nTo FAH :\n") ;
   printf("\nfid : %d", ntohs(out.fid)) ;
   printf("\nnitems : %d", ntohl(out.nitems)) ;
   printf("\nsize : %d\n", ntohl(out.size)) ;
				/* send record details to FAH */
   if (!writen(fahsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpFread::writen\n") ;
       getchar() ; 
       return ;
    }
				/* read result */
   if (!readn(fahsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpFread::readn\n") ;
       getchar() ; 
       return ;
    }
				/* print result from FAH */
   printf("\n\nFrom FAH :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   printf("\nnitems : %d", ntohl(in.nitems)) ;

   if (ntohl(in.nitems) > 0)
      if (!readn(fahsock, (char *)&rec, sizeof(rec)))
       {
	  printf("\nOpFread::readn\n") ;
	  getchar() ;
	  return ;
       }
				/* print record contents */
   printf("\nname : %s", rec.name) ;
   printf("age : %d\n", rec.age) ;
   getchar() ; 
}


void OpFwrite()
{
   fah_fwrite_ot in ;
   fah_fwrite_it out ;
   struct {
      char name[25] ;
      int age ;
   } rec ;
   int tmp ;
   char *st;
   
   status=system("clear") ;
   printf("FAH_OP_FWRITE") ;
   printf("\n-------------\n") ;
				/* obtain file desc */
   printf("\nEnter file-id : ") ;
   status=scanf("%d", &tmp) ;
   out.fid = htons((u_short)tmp) ;
   getchar() ;
   out.nitems = htonl(1) ;
   out.size = htonl(sizeof(rec)) ;
				/* obtain record data - name, age */
   printf("\nEnter name : ") ;
   st=fgets(rec.name, sizeof(rec.name), stdin) ;
   printf("Enter age : ") ;
   status=scanf("%d", &rec.age) ;
				/* print data sent to FAH */
   printf("\n\nTo FAH :\n") ;
   printf("\nfid : %d", ntohs(out.fid)) ;
   printf("\nnitems : %d", ntohl(out.nitems)) ;
   printf("\nsize : %d", ntohl(out.size)) ;
   printf("\nname : %s", rec.name) ;
   printf("age : %d\n", rec.age) ;
				/* send record details */
   if (!writen(fahsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpFwrite::writen\n") ;
       getchar() ;  getchar() ;
       return ;
    }
				/* send record to FAH */
   if (!writen(fahsock, (char *)&rec, sizeof(rec)))
    {
       printf("\nOpFwrite::writen\n") ;
       getchar() ;  getchar() ;
       return ;
    }
				/* read result */
   if (!readn(fahsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpFwrite::readn\n") ;
       getchar() ;  getchar() ;
       return ;
    }
				/* print result from FAH */
   printf("\n\nFrom FAH :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d", ntohs(in.error)) ;
   printf("\nnitems : %d\n", ntohl(in.nitems)) ;
   getchar() ;  getchar() ;
}


void OpFseek() 
{
   fah_fseek_ot in ;
   fah_fseek_it out ;
   int tmp ;

   status=system("clear") ;
   printf("FAH_OP_FSEEK") ;
   printf("\n------------\n") ;
				/* obtain fseek parameters */
   printf("\nEnter file-id : ") ;
   status=scanf("%d", &tmp) ;
   out.fid = htons((u_short)tmp) ;
   getchar() ;
   printf("\nEnter from [0/1/2] : ") ;
   status=scanf("%d", &out.from) ;
   out.from = htonl(out.from) ;
   getchar() ;
   printf("\nEnter offset : ") ;
   status=scanf("%d", &out.offset) ;
   out.offset = htonl(out.offset) ;
				/* print data sent to FAH */
   printf("\n\nTo FAH :\n") ;
   printf("\nfid : %d", ntohs(out.fid)) ;
   printf("\nfrom : %d", ntohl(out.from)) ;
   printf("\noffset : %d", ntohl(out.offset)) ;
				/* send data to FAH */
   if (!writen(fahsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpFseek::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* read result */
   if (!readn(fahsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpFseek::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from FAH */
   printf("\n\nFrom FAH :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d\n", ntohs(in.error)) ;
   getchar() ;  getchar() ;
} 


void OpFflush()
{
   fah_fflush_ot in ;
   fah_fflush_it out ;
   int tmp ;

   status=system("clear") ;
   printf("FAH_OP_FFLUSH") ;
   printf("\n-------------\n") ;
				/* obtain file desc to be flushed */
   printf("\nEnter file-id : ") ;
   status=scanf("%d", &tmp) ;
   out.fid = htons((u_short)tmp) ;
				/* print data sent to FAH */
   printf("\n\nTo FAH :\n") ;
   printf("\nfid : %d\n", ntohs(out.fid)) ;
				/* send data to FAH */
   if (!writen(fahsock, (char *)&out, sizeof(out)))
    {
       printf("\nOpFflush::writen\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* read result */
   if (!readn(fahsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpFflush::readn\n") ;
       getchar() ; getchar() ;
       return ;
    }
				/* print result from FAH */
   printf("\n\nFrom FAH :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d\n", ntohs(in.error)) ;
   getchar() ;  getchar() ;
}


void OpExit()
{
   fah_exit_ot in ;
   
   status=system("clear") ;
   printf("FAH_OP_EXIT") ;
   printf("\n-----------\n") ;
				/* read FAH response */
   if (!readn(fahsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpExit::readn\n") ;
       getchar() ;  getchar() ;
       return ;
    }
				/* print FAH response */
   printf("\n\nFrom FAH :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d\n", ntohs(in.error)) ;
   getchar() ;  getchar() ;
}


int connectFah()
{
   struct hostent *host ;
   int fah_port ;
   u_long fah_host ;
   int sock ;
				/* determine address of FAH */
   if (isdigit(FAH_HOST[0]))
      fah_host = inet_addr(FAH_HOST) ;
   else
    {
       if ((host = gethostbyname(FAH_HOST)) == NULL)
	{
	   printf("connectFAH::gethostbyname\n") ;
	   exit(1) ;
	}
       fah_host = *((long *)host->h_addr_list[0]) ;
    }
				/* get mappinf from PMD where FAH is running */
   if ((fah_port = pmd_getmap(mapid, fah_host, PMD_PROT_TCP)) == -1)
    {
       printf("connectFAH::pmd_getmap\n") ;
       exit(1) ;
    }
				/* get socket and connect to FAH */
   if ((sock = get_socket()) == -1)
    {
       printf("connectFAH::get_socket\n") ;
       exit(1) ;
    }
   if (!do_connect(sock, fah_host, fah_port))
    {
       printf("connectFAH::do_connect\n") ;
       exit(1) ;
    }      
   return sock ;
}


u_short drawMenu()
{
   int choice ;
				/* draw menu of user options */
   status=system("clear") ;
   printf("\n\n\n\t\t\t---------") ;
   printf("\n\t\t\tMAIN MENU") ;
   printf("\n\t\t\t---------") ;
   printf("\n\n\t\t\t 1. Fopen") ;
   printf("\n\t\t\t 2. Fclose") ;
   printf("\n\t\t\t 3. Fgetc") ;
   printf("\n\t\t\t 4. Fputc") ;
   printf("\n\t\t\t 5. Fgets") ;
   printf("\n\t\t\t 6. Fputs") ;
   printf("\n\t\t\t 7. Fread") ;
   printf("\n\t\t\t 8. Fwrite") ;
   printf("\n\t\t\t 9. Fseek") ;
   printf("\n\t\t\t10. Fflush") ;
   printf("\n\t\t\t11. Exit (FAH)") ;
   printf("\n\t\t\t12. Quit from this program") ;
       
   printf("\n\n\n\t\t\tEnter Choice : ") ;

   status=scanf("%d", &choice) ;	/* return user choice */
   return choice ;
}
