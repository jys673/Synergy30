#ifndef		SNG_H
#define		SNG_H		1

/* ******************************************************
 *	Synergy API header file                             *
 *	Created By: Sudhindra Herle                          *
 *	Date: 04/04/95                          *
 *                          *
 *	Annotation:                          *
 *		Synergy supports three kinds of passive objects:
 * 		Pipes, tuples and files. 
 *		Manipulating any of these objects requires that
 *		a handle for the object be established first. This is
 *		done by calling cnf_open(). All manipulations on the
 *		passive objects are similar to unix device operations.
 *		*ALL* synergy API calls also support a machine independent
 *		data representation.  All synergy functions containing 
 *		``_x_'' in their declaration support machine independent data
 *		representation.
 *
 *	Description:
 *		int cnf_open(char *objectName, char *mode) 
 *			This establishes a handle to the named passove object.
 *			If the mode is non null, the object is assumed to be a 
 *			file object and is opened in the mode specified. The
 *			function returns a handle which must be used for 
 *			subsequent references to this object.
 *			See Also: fopen(3)
 *
 *		int cnf_close(int objId)
 *			This removes all references to the object referred to
 *			by the handle. Future references to this object is an
 *			error.
 *		
 *		TO USE ANY OF THE FUNCTIONS DESCRIBED BELOW, A CALL TO
 *		cnf_open() MUST BE MADE. THIS WILL ESTABLISH THE CALLER'S
 *		IPC PATTERN FOR SUBSEQUENT OPERATIONS.
 *
 *		int cnf_getP(void)
 *			returns the number of processors available in
 *			master-slave configuration.
 *		int cnf_getF(void)
 *		int cnf_gett(void)
 *		int cnf_getarg(int objId)
 *
 *		int cnf_tsput(int objId, char *tupleName, 
 *						char *dataBuf, int nBytes)
 *			If the ``objId'' refers to a tuple-space object, then
 *			the function writes ``nBytes'' starting at location
 *			``dataBuf'' into the tuple identified by ``tupleName''.
 *
 *		int cnf_tsget(int objId, char *tupleName,
 *						char *recvBuf, int blockingStatus)
 *			If the ``objId'' referes to a tuple-space object,
 *			then the function retreives the data identified by
 *			``tupleName'' and stores at the location staring at
 *			``recvBuf'', and, *deletes* the tuple referred by
 *			``tupleName'' from the tuple-space. If ``blockingStatus'' 
 *			is zero the function has blocking semantics. If any tuple
 *			in the tuple-space is acceptable, ``tupleName'' must be
 *			the string "*". After returning, ``tupleName'' will contain
 *			the actual name of the tuple returned.
 *
 *		int cnf_tsread(int objId, char *tupleName,
 *						char *recvBuf, int blockingStatus)
 *			If the ``objId'' referes to a tuple-space object,
 *			then the function retreives the data identified by
 *			``tupleName'' and stores at the location staring at
 *			``recvBuf''. If ``blockingStatus'' is zero the function 
 *			has blocking semantics.  If any tuple in the tuple-space 
 *			is acceptable, ``tupleName'' must be the string "*". 
 *			After returning, ``tupleName'' will contain the actual 
 *			name of the tuple returned. The important difference 
 *			between	cnf_tsget() and cnf_tsread() is that the latter 
 *			*does NOT*	delete the tuple from the tuple-space.
 *
 *		int cnf_fread(int objId, char *recvBuf, int itemSize,
 *					int nItems)
 *			Read ``nItems'' of ``itemSize'' each into ``recvBuf''.
 *
 *		int cnf_fwrite(int objId, char *recvBuf, int itemSize,
 *					int nItems)
 *			Write ``nItems'' of ``itemSize'' each from ``sendBuf''
 *			and write on to the file identified by ``objId''.
 *
 *		The remaining file operations are similar to their UNIX
 *		counterparts.
 * ******************************************************/

#ifndef	PARAMS

#if		defined(__STDC__) || defined (__cplusplus)
#define		PARAMS(x)		x
#else
#define		PARAMS(x)		()	
#endif 	/* __STDC__ || __cplusplus */

#endif  /* not PARAMS */

#if		defined(__cplusplus)
extern "C"
{
#endif		/* __cplusplus */

		/* general Functions */
extern	double 	wall_clock PARAMS ((void)) ;

		/* Object open/close functions */
extern	int		cnf_open PARAMS((char *objName, char *mode)) ;
extern	int		cnf_close PARAMS((int objId)) ;
extern	void	cnf_term PARAMS((void)) ;

		/* Configuration info. functions */
extern	int		cnf_getP PARAMS((void)) ;
extern	int		cnf_getf PARAMS((void)) ;
extern	int		cnf_gett PARAMS((void)) ;
extern	int		cnf_getarg PARAMS((int arg)) ;


		/* Tuple space manipulation functions */
extern	int 	cnf_tsread PARAMS((int objId, char *tpName, char *buf, int blkSts)) ;
extern	int 	cnf_tsget PARAMS((int objId, char *tpName, char *buf, int blkSts)) ;
extern	int		cnf_tsput PARAMS((int objId, char *tpName, char *buf, int nBytes)) ;

extern  int	cnf_rmall PARAMS((int objID)); 


		/* File object manipulation functions */
extern	int		cnf_fread PARAMS((int objId,  char *recvBuf, int eleSize, int nItems)) ;
extern 	int 	cnf_fwrite PARAMS((int objId,  char *sendBuf, int eleSize, int nItems)) ;
extern	int 	cnf_fflush PARAMS((int objId)) ;
extern	int 	cnf_fgets PARAMS((int objId, char *buf, int size)) ;
extern	int		cnf_fputs PARAMS((int objId, char *buf, int size)) ;
extern	int 	cnf_fseek PARAMS((int objId, int from, int offset)) ;
extern	int 	cnf_fgetc PARAMS((int objId, char));
extern	int 	cnf_fputc PARAMS((int objId, char));


		/* Pipe object manipulation functions */
extern	int 	cnf_read  PARAMS((int objId, char *buf, int nBytes));
extern	int 	cnf_write PARAMS((int objId, char *buf, int nBytes));


		/* Machine Independent data representation macros */
#define		SNG_XDR_CHAR		0
#define		SNG_XDR_SHORT		1
#define		SNG_XDR_INT			2
#define		SNG_XDR_LONG		3
#define		SNG_XDR_FLOAT		4
#define		SNG_XDR_DOUBLE		5
#define		SNG_XDR_UCHAR		6
#define		SNG_XDR_USHORT		7
#define		SNG_XDR_UINT		8
#define		SNG_XDR_ULONG		9





		/* Tuple space manipulation functions */
extern	int 	cnf_x_tsread PARAMS((int objId, char *tpName, char *buf, int blkSts, int eType)) ;
extern	int 	cnf_x_tsget PARAMS((int objId, char *tpName, char *buf, int blkSts, int eType)) ;
extern	int		cnf_x_tsput PARAMS((int objId, char *tpName, char *buf, int nBytes, int eType)) ;



		/* File object manipulation functions */
extern	int		cnf_x_fread PARAMS((int objId,  char *recvBuf, int eleSize, int nItems, int eType)) ;
extern 	int 	cnf_x_fwrite PARAMS((int objId,  char *sendBuf, int eleSize, int nItems, int eType)) ;
extern	int 	cnf_x_fflush PARAMS((int objId, int eType)) ;
extern	int 	cnf_x_fgets PARAMS((int objId, char *buf, int size, int eType)) ;
extern	int		cnf_x_fputs PARAMS((int objId, char *buf, int size, int eType)) ;
extern	int 	cnf_x_fseek PARAMS((int objId, int from, int offset, int eType)) ;
extern	int 	cnf_x_fgetc PARAMS((int objId, char, int eType));
extern	int 	cnf_x_fputc PARAMS((int objId, char, int eType));


		/* Pipe object manipulation functions */
extern	int 	cnf_x_read  PARAMS((int objId, char *buf, int nBytes, int eType));
extern	int 	cnf_x_write PARAMS((int objId, char *buf, int nBytes, int eType));

#if		defined(__cplusplus)
}
#endif		/* __cplusplus */
#endif		/* SNG_H */

/*---------------------------------------------------------------------------*/
/* TSH error codes                                                           */
/*---------------------------------------------------------------------------*/
#define TSH_ER_NOERROR            400
#define TSH_ER_INSTALL            401
#define TSH_ER_NOTUPLE            402
#define TSH_ER_NOMEM              403
#define TSH_ER_OVERRT             404


