testdate()
{	
long t;
long expire=789000000;

t=time(0);
if (t>expire)
	{
	printf("Current date is %s\n", ctime(&t));
	printf("Public domain license expired on %s\n", ctime(&expire));
	puts("Please contact Dr. Yuan Shi <shi@cis.temple.edu> for license agreement.");
	puts("Thank you for using synergy.");
	exit(1);
	} 
}
