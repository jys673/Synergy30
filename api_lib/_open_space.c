/*
 This is a wrapper of function 'cnf_open', which is used in automatic 
 parallelizer-generated parallel programs.

 Feijian Sun
 06/12/2003
 */
int _open_space(local_name, mode, workerId)
    char *local_name;
    char *mode;
    char *workerId;
{
    int id;

    id = cnf_open(local_name, mode);
    _activate_space(id, workerId);

    return id;
}
