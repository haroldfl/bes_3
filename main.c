#include "send_rec.h"



int main(int argc, char *argv[]) {
    int test_buffer_size = 0;
    char sem_name[40];
    char shm_name[40];
    sem_t *p_sem = NULL;
    int fd_shm = -2;
    int *pointer_shm = NULL;

    test_buffer_size = fct_check_parameter(argc, argv);
    printf("Buffer Size: %i", test_buffer_size);

    fct_create_name(sem_name,1);
    printf("\nSEM NAME: %s",sem_name);

    p_sem = fct_sem_open_create(sem_name,test_buffer_size);
    fct_edit_sem('i', p_sem);
    fct_edit_sem('s', p_sem);
    fct_edit_sem('d', p_sem);
    fct_edit_sem('s', p_sem);


    fct_create_name(shm_name,2);
    printf("\nSHM NAME: %s",shm_name);

    fd_shm = fct_create_shared_mem(shm_name,test_buffer_size);

    pointer_shm = fct_map_shm(fd_shm,test_buffer_size);
    fct_unmap_shm(pointer_shm,test_buffer_size);

    fct_close_unlink_sem(sem_name,p_sem);
    fct_close_unlink_shm(shm_name,fd_shm);

    return 0;
}