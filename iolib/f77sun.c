/* This file is required by F77/OSF1 due
	to the F77-cc "_" calling convention.
*/

char *cnf_getarg();


extern int _sprintf_(char *str, const char *str2, ...)
{
	return sprintf(str, str2 );
}

void _cnf_close_(fd)
int fd;
{
	cnf_close(fd);
}

void _cnf_fflush_(fd)
int fd;
{
	cnf_fflush(fd);
}

int _cnf_fgetc_(fd, c)
int fd;
char c;
{
	return cnf_fgetc(fd,&c);
}

int _cnf_fgets_(fd, buf, size)
int fd;
char *buf;
int size;
{
	return cnf_fgets(fd,buf,size);
}

int _cnf_fputc_(fd, c)
int fd;
char c;
{
	return cnf_fputc(fd, c);
}

int _cnf_fputs_(fd, buf, size)
int fd;
char *buf;
int size;
{
	return cnf_fputs(fd, buf, size);
}

int _cnf_fread_(fd, buf, size)
int fd;
char *buf;
int size;
{
	return(cnf_fread(fd, buf, size));
}

int _cnf_fseek_(fd, pos, len)
int fd, pos, len;
{
	return (cnf_fseek(fd,pos,len));
}

int _cnf_fwrite_(fd, bptr, size, rep)
int fd;
char *bptr;
int size, rep;
{
	return (cnf_fwrite(fd, bptr, size, rep));
}

int _cnf_getp_()
{
	return (cnf_getP());
}

void _cnf_getarg_(args, idx)
char *args;
{
	args = cnf_getarg(idx);
}

int _cnf_getf_()
{
	return (cnf_getf());
}

int _cnf_gett_()
{
	return (cnf_gett());
}

int _cnf_init_()
{
	return (cnf_init());
}

int _cnf_open_(name, mode)
char *name;
char *mode;
{
	return (cnf_open(name, mode));
}

int _cnf_read_(fd, buf, size)
int fd;
char *buf;
int size;
{
	return (cnf_read(fd, buf, size));
}

void _cnf_term_()
{
	cnf_term();
}

int _cnf_tsget_(fd, tpname, buf, sw)
int fd;
char *tpname;
char *buf;
int sw;
{
	return (cnf_tsget(fd, tpname, buf, sw));
}

int _cnf_tsput_(fd, tpname, buf, size)
int fd;
char *tpname, *buf;
int size;
{
	return (cnf_tsput(fd, tpname, buf, size));
}

int _cnf_tsread_(fd, tpname, buf, sw)
int fd;
char *tpname, *buf;
int sw;
{
	return (cnf_tsread(fd, tpname, buf, sw));
}

int _cnf_write_(fd, buf, size)
int fd;
char *buf;
int size;
{
	return (cnf_write(fd, buf, size));
}

int _cputime_(t)
double t;
{
	return (cputime(&t));
}
