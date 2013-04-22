/*.........................................................................*/
/*                  PMDTEST.C ------> PMD test program                     */
/*                                                                         */
/*                  By N. Isaac Rajkumar [April '93]                       */
/*                  February '13, updated by Justin Y. Shi                 */
/*.........................................................................*/

#include "pmdtest.h"
#include <time.h>
int status;
   
int main(argc, argv)
int argc; 
char **argv;
{
   static void (*op_func[])() = {
      OpNull, OpMap, OpUnmap, OpGetMap, OpGetTable, OpPurgeTable, OpExit
      } ;
   u_short this_op ;

   if (argc < 2)
    {
       printf("Usage : %s <pmd-host>\n", argv[0]) ;
       exit(1) ;
    }
   strcpy(PMD_HOST, argv[1]) ;	/* argv[1] is PMD host */
   
   while (TRUE)
    {
       this_op = drawMenu() + PMD_OP_MIN - 1 ;
       if (this_op >= PMD_OP_MIN && this_op <= PMD_OP_MAX)
	{
	   this_op = htons(this_op) ;
	   pmdsock = connectPmd() ;
	   if (!writen(pmdsock, (char *)&this_op, sizeof(this_op)))
	    {
	       printf("main::writen\n") ;
	       exit(1) ;
	    }
	   (*op_func[ntohs(this_op) - PMD_OP_MIN])() ;
	   close(pmdsock) ;
	}			/* validate operation & process */
       else
	  return 0 ;
    }
}


void OpNull()
{
   pmd_null_ot in ;
   
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
   {	time_t t;

	t = (time_t)in.start_time;
   	printf("\nstart_time : %s\n", ctime(&t)) ;
   }
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
printf("No privilege for attempted operation.\n");
exit(1);
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


void OpExit()
{
   pmd_exit_ot in ;
   
   status=system("clear") ;
printf("No privilege for attempted operation. \n");
exit(1);
   printf("PMD_OP_EXIT") ;
   printf("\n-----------\n") ;
				/* read PMD response */
   if (!readn(pmdsock, (char *)&in, sizeof(in)))
    {
       printf("\nOpExit::readn\n") ;
       getchar() ;  getchar() ;
       return ;
    }
				/* print PMD response */
   printf("\n\nFrom PMD :\n") ;
   printf("\nstatus : %d", ntohs(in.status)) ;
   printf("\nerror : %d\n", ntohs(in.error)) ;
   getchar() ;  getchar() ;
}


int connectPmd()
{
   int sock ;
   u_long pmd_host ;
   struct hostent *host ;
				/* determine address of PMD */
   if (isdigit(PMD_HOST[0]))
      pmd_host = inet_addr(PMD_HOST) ;
   else
    {
       if ((host = gethostbyname(PMD_HOST)) == NULL)
	{
	   printf("connectPmd::gethostbyname\n") ;
	   exit(1) ;
	}
       pmd_host = *((long *)host->h_addr_list[0]) ;
    }
				/* get socket and connect to PMD */
   if ((sock = get_socket()) == -1)
    {
       printf("connectPmd::get_socket\n") ;
       exit(1) ;
    }
   if (!do_connect(sock, pmd_host, htons(PMD_PORT)))
    {
       printf("connectPmd::do_connect\n") ;
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
   printf("\n\n\t\t\t1. Null") ;
   printf("\n\t\t\t2. Map") ;
   printf("\n\t\t\t3. Un-map") ;
   printf("\n\t\t\t4. Get map") ;
   printf("\n\t\t\t5. Get table") ;
   printf("\n\t\t\t8. Quit from this program") ;
       
   printf("\n\n\n\t\t\tEnter Choice : ") ;

   status=scanf("%d", &choice) ;	/* return user choice */
   return choice ;
}
