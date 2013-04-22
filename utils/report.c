/* report.c */

#include "synergy.h" 

/* This Routine Gets Called If There Is A Segmentation Violation */
void sig_routine()
{
	perror("there has been a segmentation violation");
	exit(1);
}

main(argc, argv)
int  argc;
char *argv[];
{

    struct sockaddr_in   server;
    struct hostent       *hp, *gethostbyname();
    char                 ch[1024], mapid[MAP_LEN] ;
    int	                 index, status, sock;
    u_long		 rpt_host; 

    if (argc != 2)
      {
	printf("Usage: %s <hostname>\n", argv[0]);
	exit(0);
      }

/* signal routine for handling segmentation violations */
    signal(SIGSEGV , sig_routine);
    signal(SIGBUS , sig_routine);


/* create a socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("opening stream socket\n");
        exit(1);
    }


/* connect socket using name */
    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    if (hp == 0) {
        fprintf(stderr , "%s:unknown hostO\n", argv[1] );
        exit(2);
    } 
    bcopy(hp->h_addr , &server.sin_addr , hp->h_length);
    strcpy(mapid,"sng$reportd");
    rpt_host = *((long *)hp->h_addr_list[0]);
    if (!(server.sin_port = pmd_getmap(mapid, rpt_host, (u_short)PMD_PROT_TCP)))
    {
	printf(" Please start PMD first !!\n");
	exit(1);
    } 
    if (connect(sock, &server, sizeof(server)) < 0) {
        close(sock);
        perror("connecting stream socket\n");
        exit(1);
    }

/*--------------------------------------------------------------------------*/

        ch[0] = 1;   /* Command: Bring everything you got */
        if  ((status=write(sock, ch, 10))<10){
            perror("Writing Socket malfunctioning");
            exit(0);
        }
        if ((status=sockread( sock, ch, 1024 )) < 0 ) 
            return(0);

        index = 0;
        ParseDF( ch );

    fflush(stdout);

    close(sock);
    exit(0);

}


ParseDF( buf )
char *buf;
{
     puts(buf);
}

sockread(sock, buff, length)
int sock;
char *buff;
int length;
{
    int icount, index, remain;
    
    index = 0;
    remain = length;
    do {
        if  ( (icount=read(sock, &buff[index], remain)) < 0 ) {
             perror("Malfunctioning socket during read");
             return(-1);
        }
        index = index + icount;
        remain = remain - icount;
    } while (index<length);
    return(length);
}


