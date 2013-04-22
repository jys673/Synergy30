/* cds.c -- Check remote REPORTDs according to input file. */
#include "synergy.h"

main (argc, argv)
int argc;
char **argv;
{
	FILE *infile;
	char *buf;
	char ipaddr[32], login[32], host[128], home[128], filenm[128];
	char local_host[128], hostname[128];
	int  status, sd;
	u_short op; 

	if (argc > 2)
	{
		strcpy(filenm,argv[2]);
	} else {
		gethostname(hostname,sizeof(hostname));
		sprintf(home,"%s",getenv("HOME"));
		sprintf(filenm,"%s/.sng_hosts",home);
	} 
	if ((infile = fopen(filenm, "r")) !=NULL)
 	{
		status = fscanf(infile, "%s %s %s %s %s",
			ipaddr, host, home, home, login);
  		while (status > 0)
		{
			check_reportd(host);
			status = fscanf(infile, "%s %s %s %s %s",
				ipaddr, host, home, home, login);
		}
	}
}


check_reportd(ipaddr)
char *ipaddr;
{
    struct sockaddr_in	server;
    struct hostent       *host;
    char                 ch[1024] ;
    int                  index, status, sock, rptport;
    char 		 mapid[MAP_LEN]; 
    u_long		 rpt_host; 

    if ((host = gethostbyname(ipaddr)) == NULL)
    {
           perror("connect reportd::gethostbyname\n") ;
           exit(1) ;
    }
    rpt_host = *((long *)host->h_addr_list[0]) ;
    bcopy(host->h_addr, &server.sin_addr, host->h_length);
                                /* get socket and connect to CID */
    if ((sock = get_socket()) == -1)
    {
       perror("connect reportd::get_socket\n") ;
       exit(1) ;
    }
    strcpy(mapid,"sng$reportd");
    if (!(server.sin_port = pmd_getmap(mapid, rpt_host, (u_short)PMD_PROT_TCP)))
    {
        printf("REPORTD down (%s) PMD down\n",ipaddr);
	return;
    }
    server.sin_family = AF_INET; 
    if (connect(sock, &server, sizeof(server)) < 0) {
        close(sock);
	printf("REPORTD down (%s)\n",ipaddr); 
	return;
    }
    ch[0] = 1;   /* Command: Get everything */
    if  ((status=write(sock, ch, 10))<10){
            perror("Writing Socket malfunctioning");
            exit(0);
    }
    if ((status=sockread( sock, ch, 1024 )) < 0 ) 
            return(0);
    printf("(%s)\n [%s]\n",ipaddr,ch);
    fflush(stdout);
    close(sock);
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


