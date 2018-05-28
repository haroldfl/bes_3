#include "send_rec.h"



int main(int argc, char *argv[]) {
    int test_buffer_size = 0;
    char sem_name[40];
    char shm_name[40];
    sem_t *p_sem = NULL;
    sem_t *p_sem_empf=NULL;
    int fd_shm = -2;
    int *pointer_shm = NULL;

    test_buffer_size = fct_check_parameter(argc, argv);
    printf("Buffer Size: %i", test_buffer_size);

    fct_create_name(sem_name,1,SENDERID);
    printf("\nSEM NAME: %s",sem_name);

    p_sem = fct_sem_open_create(sem_name,test_buffer_size,SENDERID);

    //get sem pointer of empfänger
    fct_create_name(sem_name,1,EMPFAENGER); //create receiver name
    p_sem_empf=fct_sem_open_create(sem_name,test_buffer_size,EMPFAENGER);//to get pointer of receiver sem

    fct_edit_sem('i', p_sem);
    fct_edit_sem('s', p_sem);
    fct_edit_sem('d', p_sem);
    fct_edit_sem('s', p_sem);



    fct_create_name(shm_name,2,SENDERID);
    printf("\nSHM NAME: %s",shm_name);

    fd_shm = fct_create_shared_mem(shm_name,test_buffer_size);

    pointer_shm = fct_map_shm(fd_shm,test_buffer_size);


    while(fd_shm!=EOF){     //read as long as the char is not EOF
        sem_wait(p_sem);

        fd_shm=fgetc(STDIN);
        sem_post(p_sem_empf);
        fd_shm++;

    }
    // synchronisation sem


    fct_unmap_shm(pointer_shm,test_buffer_size);

    fct_close_unlink_sem(sem_name,p_sem);
    fct_close_unlink_shm(shm_name,fd_shm);

    return 0;
}