/* reportd.c */

#include "synergy.h" 
void return_value();
void sig_routine();
void fork1();


FILE          	*ptr, *uu;
char    	buf[255], packet[255];
int		sock , length , pid ,process_id , process_id2;
int		msgsock, REUSE ;
unsigned int	prognum = 4747;
unsigned int	version = 2;
char		ch, mapid[MAP_LEN];
int		rpt_port, pt; 
u_long  	hostid; 
struct hostent 	*rpt_host;

main(argc, argv)
int argc;
char **argv; 
{
	char hostname[128]; 
#ifdef TEST
  return_value(packet);
#else

	if (argc > 1) REUSE = 1;
	else REUSE = 0;

	/* executes this routine when the server is killed forcefully */
	signal(SIGTERM , sig_routine);


	/* kills the server using its process id when cntl-c is hit */
	process_id = getpid();

	if ((sock = get_socket()) == -1)
    	{
       		fprintf(stderr, "initSocket::get_socket\n") ;
       		exit(1) ;
    	}
	if (!(rpt_port = bind_socket(sock, 0)))
    	{
       		fprintf(stderr, "initSocket::bind_socket\n") ;
       		close(sock);
       		exit(1) ;
    	}
	if (gethostname(hostname,sizeof(hostname)) == -1)
    	{
		perror(" Cannot get hostname"); 
        	exit(1);
    	}
    	if ((rpt_host = gethostbyname(hostname)) == NULL)
    	{
		perror(" Cannot gethostbyname"); 
        	exit(1) ;
    	}
	hostid = *((long *)rpt_host->h_addr_list[0]); 
    	strcpy(mapid,"sng$reportd");
    	if ((pt = pmd_getmap(mapid, hostid, (u_short)PMD_PROT_TCP))>0)
    	{
        	if (!REUSE)
        	{
                	exit(0);
        	}
    	}
    	if (!(pmd_map(mapid, rpt_port, (u_short)PMD_PROT_TCP)))
    	{
        	printf("PMD down (%s)\n", hostname);
        	exit(0);
    	}

	/* Start accepting connections*/
	listen(sock, 5);
	do {
		msgsock = accept(sock , 0 , 0);
		if (msgsock == -1) {
			perror("accept");
		}
                /*****
		if ( (pid = fork()) < 0) {
			perror("forking error");
			exit(1);
		}
                *****/
                pid = 0;
		if ( pid == 0) {
			fork1();
		}
		close(msgsock);
	} while (1);
	close(msgsock);
#endif
}


/* Returns the value as specified by command */

void return_value( packet )
    char *packet;
{
  ParseCPU(packet);

  ParseMEM(packet);

  ParseDisk(packet);

  ParseNet(packet);
}

ParseNet(packet)
char *packet;
{
int connections=0;

/* parse received */
if ( (uu=popen("netstat | wc","r")) == NULL )
  perror("popen netstat error");
fgets(buf, 255, uu);
sscanf(buf,"%d", &connections);

sprintf(buf, "NET: connections %d ", connections);
strcat(packet, buf);
#ifdef TEST
puts(packet);
#endif
}

ParseMEM(packet)
char *packet;
{
int active=0, free=0;
int position;

if ( (uu=popen("vmstat","r")) == NULL )
  perror("error popen vmstat");

while (fgets(buf, 255, uu))
  {
    position=member("avm", buf);
    if (position <= 0)
      position=member("act", buf);
    if (position >0)
      {
	fgets(buf, 255, uu);
	sscanf(buf+position-1, "%d%d", &active, &free);
	if (active!=0)
	  break;
      }
  }/* end of while */

sprintf(buf, "MEM: %d/%d  ",active, free+active);
strcat(packet, buf);
#ifdef TEST
puts(packet);
#endif
}

ParseDisk(packet)
char *packet;
{
char tmp[255];
int volume=0, used=0, avail=0, capacity=0;
int sum=0, total_used=0;

if ( (uu=popen("df","r")) == NULL )
  perror("popen df error");

fgets(buf, 255, uu);

while (fgets(buf, 255, uu))
  {
    sscanf(buf, "%s%d%d%d%d", tmp, &volume, &used, &avail, &capacity);
    if (used !=0 )
      {
	sum=sum+volume;
	total_used=total_used+used;
      }
  } /* end of while */
pclose(uu);

/* doesn't work on zoro */

sprintf(tmp,"DISK: %d/%d  ", total_used, sum);
strcat(packet,tmp);
#ifdef TEST
puts(packet);
#endif
}

ParseCPU(packet)
char *packet;
{
int position, us=0, sy=0, id=0;

if ( (uu=popen("vmstat", "r")) == NULL )
  perror("error popen vmstat");

while (fgets(buf, 255, uu))
  {
    /* this takes some brain power. 'coz cpu field is not at constant place*/
    position=member("us",buf);
    if (position >0 )
      {
	fgets(buf, 255, uu);
	sscanf(buf+position, "%d%d%d", &us, &sy, &id);
	if (us != 0)
	  break;
      }
  }/* end of while */
sprintf(buf,"CPU: %d/%d  ", us+sy, us+sy+id);
strcat(packet, buf);
#ifdef TEST
puts(packet);
#endif
}

member(seed, string)
char *seed, *string;
{
int i, j;

for (i=0; i<strlen(string); i++)
  if ( (seed[0]==string[i]) && (seed[1]==string[i+1]) )
    return(i);
return(-1);
}
	
/* This routine is executed every time the server is killed forcefully */

void sig_routine()
{
	perror("killing the server forcefully");
	pmd_unmap(mapid, pid, PMD_PROT_TCP);
	exit(1);
}


/* this routine gets executed as a result of the fork1 call */
void fork1()
{
                
    char packet[1024], cmd[128];
    int status, i;

    if  ((status=read(msgsock, cmd, 10))<10){
/*        perror("Slow Connection"); */
    }
    if (cmd[0] == 0) 
    {
	pmd_unmap(mapid, pid, PMD_PROT_TCP);
	exit(1);
    } 
    for (i=0; i<1024; i++) packet[i] = 0;
    return_value(packet);

    if (write(msgsock, packet, 1024 ) < 0) {
      perror("writing on stream socket\n");
      exit(1);
    }

    if  ((status=read(msgsock, cmd, 10))<10){
/*      perror("Slow Connection"); */
    }
    close(msgsock);
}


