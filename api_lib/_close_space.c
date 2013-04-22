/*
 This is a wrapper of function 'cnf_close', which is used in automatic 
 parallelizer-generated parallel programs.

 Feijian Sun
 06/12/2003
 */
int _close_space(id, workerId, cleanup)
    int id, cleanup;
    char *workerId;
{
    if (cleanup)
       _cleanup_queue(id, workerId);

    return cnf_close(id);
}
