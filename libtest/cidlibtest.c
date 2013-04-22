/*.........................................................................*/
/*                  CIDLIBTEST.C ------> CID library testing program       */
/*                                                                         */
/*                  By N. Isaac Rajkumar [August '93]                      */
/*.........................................................................*/

#include "synergy.h"

void OpNull(/*void*/) ;
void OpFileGet(/*void*/) ;
void OpFilePut(/*void*/) ;
void OpRemove(/*void*/) ;
void OpAddUser(/*void*/) ;
void OpDeleteUser(/*void*/) ;
void OpVerifyUser(/*void*/) ;

int drawMenu(/*void*/) ;
void getCidHost(/*char **/) ;

char cid_host[20] ;

int main(argc, argv)
int argc; 
char **argv;
{
   int choice ;

   if (argc < 2)
    {
       printf("Usage : %s <cid-host>\n", argv[0]) ;
       exit(1) ;
    }
   getCidHost(argv[1]) ;
   
   while (TRUE)
    {
       choice = drawMenu() ;

       if (choice == 1)
	  OpNull() ;
       else if (choice == 2)
	  OpFileGet() ;
       else if (choice == 3)
	  OpFilePut() ;
       else if (choice == 4)
	  OpRemove() ;
       else if (choice == 5)
	  OpAddUser() ;
       else if (choice == 6)
	  OpDeleteUser() ;
       else if (choice == 7)
	  OpVerifyUser() ;
       else
	  break ;
    }
}


void OpNull()
{
   printf("CID is %s .....", ping_cid(cid_host) ? "up" : "down") ;
   getchar() ; getchar() ;
}


void OpFilePut()
{
   char login[LOGIN_LEN] ;
   char lpath[PATH_LEN] ;
   char rpath[PATH_LEN] ;

   printf("\nEnter login : ") ;
   scanf("%s", login) ;
   getchar() ;
   printf("Enter remote path : ") ;
   scanf("%s", rpath) ;
   getchar() ;
   printf("Enter local path : ") ;
   scanf("%s", lpath) ;
   getchar() ;
   printf("\n\nFile %s transferred......", file_put(cid_host, login, lpath, 
						    rpath) ? "" : "not") ;
   getchar() ;
}

void OpFileGet()
{
   char login[LOGIN_LEN] ;
   char lpath[PATH_LEN] ;
   char rpath[PATH_LEN] ;

   printf("\nEnter login : ") ;
   scanf("%s", login) ;
   getchar() ;
   printf("Enter remote path : ") ;
   scanf("%s", rpath) ;
   getchar() ;
   printf("Enter local path : ") ;
   scanf("%s", lpath) ;
   getchar() ;
   printf("\n\nFile %s transferred......", file_get(cid_host, login, lpath, 
						    rpath) ? "" : "not") ;
   getchar() ;
}


void OpRemove()
{
   char login[LOGIN_LEN] ;
   char path[PATH_LEN] ;

   printf("\nEnter login : ") ;
   scanf("%s", login) ;
   getchar() ;
   printf("Enter path : ") ;
   scanf("%s", path) ;
   getchar() ;
   printf("\n\nFile %s removed......", file_remove(cid_host, login, path)
	  ? "" : "not") ;
   getchar() ;
}


void OpAddUser()
{
   char login[LOGIN_LEN] ;

   printf("\nEnter login : ") ;
   scanf("%s", login) ;
   printf("\n\nUser %sadded.....", add_user(cid_host, login) ? "" : "not ") ;
   getchar() ;   getchar() ;
}


void OpDeleteUser()
{
   char login[LOGIN_LEN] ;

   printf("\nEnter login : ") ;
   scanf("%s", login) ;
   printf("\n\nUser %sdeleted.....", delete_user(cid_host, login) ? "":"not ");
   getchar() ;   getchar() ;
}

void OpVerifyUser()
{
   char login[LOGIN_LEN] ;

   printf("\nEnter login : ") ;
   scanf("%s", login) ;
   printf("\n\nUser %svalid.....", verify_user(cid_host, login) ? "" : "in") ;
   getchar() ;   getchar() ;
}


void getCidHost(host_name)
char *host_name;
{
   struct hostent *host ;
   struct in_addr host_addr ;
				/* determine address of CID */
   if (isdigit(host_name[0]))
      strcpy(cid_host, host_name) ;
   else
    {
       if ((host = gethostbyname(host_name)) == NULL)
	{
	   printf("getCidHost::gethostbyname\n") ;
	   exit(1) ;
	}
       host_addr.s_addr = *((long *)host->h_addr_list[0]) ;
       strcpy(cid_host, inet_ntoa(host_addr)) ;
    }
}


int drawMenu()
{
   int choice ;

   system("clear") ;
   printf("\t\t\t\tMAIN MENU") ;
   printf("\n\n\t\t\t 1. Ping Cid") ;
   printf("\n\t\t\t 2. Get File") ;
   printf("\n\t\t\t 3. Put File") ;
   printf("\n\t\t\t 4. Remove file") ;   
   printf("\n\t\t\t 5. Add user") ;   
   printf("\n\t\t\t 6. Delete user") ;   
   printf("\n\t\t\t 7. Verify user") ;   
   printf("\n\t\t\t 8. Quit from this program") ;

   printf("\n\n\t\t\tEnter Choice : ") ;
   scanf("%d", &choice) ;
   return choice ;
}
