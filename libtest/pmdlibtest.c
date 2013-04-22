/*.........................................................................*/
/*                  PMDLIBTEST.C ------> PMDLIB test program               */
/*                                                                         */
/*                  By N. Isaac Rajkumar [April '93]                       */
/*.........................................................................*/

#include "synergy.h"

void OpMap(/*void*/) ;
void OpUnmap(/*void*/) ;
void OpGetMap(/*void*/) ;
void OpNull(/*void*/) ;
u_short drawMenu(/*void*/) ;

int main()
{
   int this_op ;

   while (TRUE)
    {
       if ((this_op = drawMenu()) == 1)
	  OpMap() ;
       else if (this_op == 2)
	  OpUnmap() ;
       else if (this_op == 3)
	  OpGetMap() ;
       else if (this_op == 4) 
	  OpNull() ;
       else
	  exit(0) ;
    }
}


void OpMap()
{
   char mapid[15] ;
   int tmp ;
   u_short port ;
   
   system("clear") ;
   printf("PMD_OP_MAP") ;
   printf("\n----------\n") ;

   printf("\nEnter mapid : ") ;
   scanf("%s", mapid) ;
   getchar() ;
   printf("\nEnter port : ") ;
   scanf("%d", &tmp) ;
   port = htons((u_short) tmp) ;
   if (!pmd_map(mapid, port, PMD_PROT_TCP))
      printf("\nPMD Error\n") ;
   else
      printf("\nSuccess\n") ;
   getchar() ;  getchar() ;
}


void OpUnmap()
{
   char mapid[15] ;

   system("clear") ;
   printf("PMD_OP_UNMAP") ;
   printf("\n------------\n") ;

   printf("\nEnter mapid : ") ;
   scanf("%s", mapid) ;
   if (!pmd_unmap(mapid, 0, PMD_PROT_TCP))
      printf("\nError\n") ;
   else
      printf("\nSuccess\n") ;
   getchar() ;  getchar() ;
}


void OpGetMap()
{
   char mapid[15] ;
   char hostname[100] ;
   struct hostent *host ;
   u_long pmd_host ;
   short port ;

   system("clear") ;
   printf("PMD_OP_GETMAP") ;
   printf("\n-------------\n") ;

   printf("\nEnter mapid : ") ;
   scanf("%s", mapid) ;
   getchar() ;
				/* host in which PMD is running */
   printf("\nEnter host-name : ") ;
   scanf("%s", hostname) ;
   if (isdigit(hostname[0]))
      pmd_host = inet_addr(hostname) ;
   else
    {
       if ((host = gethostbyname(hostname)) == NULL)
	{
	   printf("\nOpGetMap::gethostbyname\n") ;
	  getchar() ;  getchar() ;
	   return ;
	}
       pmd_host = *((long *)host->h_addr_list[0]) ;
    }
   if ((port = pmd_getmap(mapid, pmd_host, PMD_PROT_TCP)) == -1)
      printf("\nError\n") ;
   else
      printf("\nPort : %u\n", ntohs(port)) ;
   getchar() ;  getchar() ;
}


void OpNull()
{
   char hostname[100] ;
   struct hostent *host ;
   u_long pmd_host ;
   u_short port ;
   
   system("clear") ;
   printf("PMD_OP_NULL") ;
   printf("\n-----------\n") ;
   
   printf("\nEnter host-name : ") ;
   scanf("%s", hostname) ;
   if (isdigit(hostname[0]))
      pmd_host = inet_addr(hostname) ;
   else
    {
       if ((host = gethostbyname(hostname)) == NULL)
	{		
	   printf("\nOpGetMap::gethostbyname\n") ;
	   getchar() ;  getchar() ;
	   return ;
	}
       pmd_host = *((long *)host->h_addr_list[0]) ;
    }

   if (!ping_pmd(pmd_host))
      printf("\nPMD is down.") ;
   else
      printf("\nPMD is up.") ;
   getchar() ;  getchar() ;
}


u_short drawMenu()
{
   int choice ;
				/* draw menu of user options */
   system("clear") ;
   printf("\n\n\n\t\t\t---------") ;
   printf("\n\t\t\tMAIN MENU") ;
   printf("\n\t\t\t---------") ;
   printf("\n\t\t\t1. Map") ;
   printf("\n\t\t\t2. Un-map") ;
   printf("\n\t\t\t3. Get-map") ;
   printf("\n\t\t\t4. Ping") ;
   printf("\n\t\t\t5. Quit from this program") ;
       
   printf("\n\n\n\t\t\tEnter Choice : ") ;

   scanf("%d", &choice) ;	/* return user choice */
   return choice ;
}



