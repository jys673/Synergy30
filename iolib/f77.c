/* This file is required by F77/OSF1 due
	to the F77-cc "_" calling convention.
*/

char *cnf_getarg();


extern int sprintf_(char *str, const char *str2, ...)
{
	return sprintf(str, str2 );
}

void cnf_close_(fd)
int fd;
{
	cnf_close(fd);
}

void cnf_fflush_(fd)
int fd;
{
	cnf_fflush(fd);
}

int cnf_fgetc_(fd, c)
int fd;
char c;
{
	return cnf_fgetc(fd,&c);
}

int cnf_fgets_(fd, buf, size)
int fd;
char *buf;
int size;
{
	return cnf_fgets(fd,buf,size);
}

int cnf_fputc_(fd, c)
int fd;
char c;
{
	return cnf_fputc(fd, c);
}

int cnf_fputs_(fd, buf, size)
int fd;
char *buf;
int size;
{
	return cnf_fputs(fd, buf, size);
}

int cnf_fread_(fd, buf, size)
int fd;
char *buf;
int size;
{
	return(cnf_fread(fd, buf, size));
}

int cnf_fseek_(fd, pos, len)
int fd, pos, len;
{
	return (cnf_fseek(fd,pos,len));
}

int cnf_fwrite_(fd, bptr, size, rep)
int fd;
char *bptr;
int size, rep;
{
	return (cnf_fwrite(fd, bptr, size, rep));
}

int cnf_getp_()
{
	return (cnf_getP());
}

void cnf_getarg_(args, idx)
char *args;
{
	args = cnf_getarg(idx);
}

int cnf_getf_()
{
	return (cnf_getf());
}

int cnf_gett_()
{
	return (cnf_gett());
}

int cnf_init_()
{
	return (cnf_init());
}

int cnf_open_(name, mode)
char *name;
char *mode;
{
	return (cnf_open(name, mode));
}

int cnf_read_(fd, buf, size)
int fd;
char *buf;
int size;
{
	return (cnf_read(fd, buf, size));
}

void cnf_term_()
{
	cnf_term();
}

int cnf_tsget_(fd, tpname, buf, sw)
int fd;
char *tpname;
char *buf;
int sw;
{
	return (cnf_tsget(fd, tpname, buf, sw));
}

int cnf_tsput_(fd, tpname, buf, size)
int fd;
char *tpname, *buf;
int size;
{
	return (cnf_tsput(fd, tpname, buf, size));
}

int cnf_tsread_(fd, tpname, buf, sw)
int fd;
char *tpname, *buf;
int sw;
{
	return (cnf_tsread(fd, tpname, buf, sw));
}

int cnf_write_(fd, buf, size)
int fd;
char *buf;
int size;
{
	return (cnf_write(fd, buf, size));
}

int cputime_(t)
double t;
{
	return (cputime(&t));
}
