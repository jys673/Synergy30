/*====================================================================*/
/*  Subroutine   cnf_xdr_tsget()                                      */
/*    Destructive reads the external data representation of a tuple   */
/*    from a named tuple space and Translates it to C language        */
/*  Coded by Feijian Sun, 08/94                                       */
/*====================================================================*/
#include "synergy.h"
#include "iolib.h"
#include <rpc/xdr.h>

int cnf_xdr_tsget(tsh, tp_name, tuple, tp_len, e_type)
int tsh;
char *tp_name;
char *tuple;
int tp_len;
int e_type;
{
        XDR xdrs;
        char *xdr_itp;
        int status, i;

        status = cnf_tsget(tsh, tp_name, tuple, tp_len);
	if (status == 0) return (0); 	/* async read */
        if ((xdr_itp = (char *)malloc(status)) == NULL)
                return(-1);
	for (i = 0; i < status; i ++)
		xdr_itp[i] = tuple[i];
        xdrmem_create(&xdrs, xdr_itp, status + sizeof(char), XDR_DECODE);

        switch (e_type) {
            case 0: /* char */
                for (i = 0; i < status; i = i+sizeof(char)) {
                    if (!xdr_char(&xdrs, (char *)&tuple[i])) {
                        printf("cnf_xdr_tsget failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 1: /* short */
                for (i = 0; i < status; i = i+sizeof(short)) {
                    if (!xdr_short(&xdrs, (short *)&tuple[i])) {
                        printf("cnf_xdr_tsget failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 2: /* int */
                for (i = 0; i < status; i = i+sizeof(int)) {
                    if (!xdr_int(&xdrs, (int *)&tuple[i])) {
                        printf("cnf_xdr_tsget failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 3: /* long */
                for (i = 0; i < status; i = i+sizeof(long)) {
                    if (!xdr_long(&xdrs, (long *)&tuple[i])) {
                        printf("cnf_xdr_tsget failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 4: /* float */
                for (i = 0; i < status; i = i+sizeof(float)) {
                    if (!xdr_float(&xdrs, (float *)&tuple[i])) {
                        printf("cnf_xdr_tsget failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 5: /* double */
                for (i = 0; i < status; i = i+sizeof(double)) {
                    if (!xdr_double(&xdrs, (double *)&tuple[i])) {
                        printf("cnf_xdr_tsget failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 6: /* u_char */
                for (i = 0; i < status; i = i+sizeof(u_char)) {
                    if (!xdr_u_char(&xdrs, (u_char *)&tuple[i])) {
                        printf("cnf_xdr_tsget failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 7: /* u_short */
                for (i = 0; i < status; i = i+sizeof(u_short)) {
                    if (!xdr_u_char(&xdrs, (u_short *)&tuple[i])) {
                        printf("cnf_xdr_tsget failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 8: /* u_int */
                for (i = 0; i < status; i = i+sizeof(u_int)) {
                    if (!xdr_u_int(&xdrs, (u_int *)&tuple[i])) {
                        printf("cnf_xdr_tsget failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            case 9: /* u_long */
                for (i = 0; i < status; i = i+sizeof(u_long)) {
                    if (!xdr_u_long(&xdrs, (u_long *)&tuple[i])) {
                        printf("cnf_xdr_tsget failed!\n");
                        xdr_destroy(&xdrs);
                        return(-1);
                    }
                }
                break;
            default: /* error element type */
                printf("element type of tuple error.\n");
        }
        xdr_destroy(&xdrs);
        free(xdr_itp);
        return(status);
}
