/*---------------------------------------------------------------------
    cnf_strupcase - convert asciz string pointed to by sp to uppercase. 
 ---------------------------------------------------------------------*/
	
cnf_strupcase(sp)
    char sp[];			/* pointer to asciz string to convert */
{
    int i, len;
    
    len = strlen(sp);
    for (i=0; i<len; i++) {
	if ( isalpha(sp[i]) && islower(sp[i])) sp[i] = toupper(sp[i]);
	}
}
