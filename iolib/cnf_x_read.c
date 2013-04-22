/*====================================================================*/
/*  Subroutine   cnf_xdr_read()    				      */ 
/*	cnf_xdr_read() - read the external data representation of     */
/*	a 'record' from file or pipe into buffer (starting at 	      */
/*	address xdr_buff) and Translates it to C language.            */
/*      RETURNS: 0 on EOF, o/w number of bytes read.		      */
/*  Coded by Feijian Sun, 08/94                                       */
/*====================================================================*/
#include "synergy.h"
#include "iolib.h"
#include <rpc/xdr.h>

int cnf_xdr_read(id, buf, bufsize, e_type)
int id;
char *buf;
int bufsize;
int e_type;
{
        XDR xdrs;
        char *xdr_buf;
        int status, i;

        status = cnf_read(id, buf, bufsize);
	if (status == 0) return (0); 	/* async read */
        if ((xdr_buf = (char *)malloc(status)) == NULL)
                return(-1);
	for (i = 0; i < status; i ++)
		xdr_buf[i] = buf[i];
        xdrmem_create(&xdrs, xdr_buf, status + sizeof(char), XDR_DECODE);

        switch (e_type) {
            case 0: /* char */
                for (i = 0; i < status; i = i+sizeof(char)) {
                    if (!xdr_char(&xdrs, (char *)&buf[i])) {
                        printf("cnf_xdr_read failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 1: /* short */
                for (i = 0; i < status; i = i+sizeof(short)) {
                    if (!xdr_short(&xdrs, (short *)&buf[i])) {
                        printf("cnf_xdr_read failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 2: /* int */
                for (i = 0; i < status; i = i+sizeof(int)) {
                    if (!xdr_int(&xdrs, (int *)&buf[i])) {
                        printf("cnf_xdr_read failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 3: /* long */
                for (i = 0; i < status; i = i+sizeof(long)) {
                    if (!xdr_long(&xdrs, (long *)&buf[i])) {
                        printf("cnf_xdr_read failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 4: /* float */
                for (i = 0; i < status; i = i+sizeof(float)) {
                    if (!xdr_float(&xdrs, (float *)&buf[i])) {
                        printf("cnf_xdr_read failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 5: /* double */
                for (i = 0; i < status; i = i+sizeof(double)) {
                    if (!xdr_double(&xdrs, (double *)&buf[i])) {
                        printf("cnf_xdr_read failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 6: /* u_char */
                for (i = 0; i < status; i = i+sizeof(u_char)) {
                    if (!xdr_u_char(&xdrs, (u_char *)&buf[i])) {
                        printf("cnf_xdr_read failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 7: /* u_short */
                for (i = 0; i < status; i = i+sizeof(u_short)) {
                    if (!xdr_u_char(&xdrs, (u_short *)&buf[i])) {
                        printf("cnf_xdr_read failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 8: /* u_int */
                for (i = 0; i < status; i = i+sizeof(u_int)) {
                    if (!xdr_u_int(&xdrs, (u_int *)&buf[i])) {
                        printf("cnf_xdr_read failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 9: /* u_long */
                for (i = 0; i < status; i = i+sizeof(u_long)) {
                    if (!xdr_u_long(&xdrs, (u_long *)&buf[i])) {
                        printf("cnf_xdr_read failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            default: /* error element type */
                printf("element type of tuple error.\n");
        }
        xdr_destroy(&xdrs);
        free(xdr_buf);
        return(status);
}
