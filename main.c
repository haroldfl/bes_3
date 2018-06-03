#include "send_rec.h"



int main(int argc, char *argv[]) {
    int test_buffer_size = 0;

    test_buffer_size = fct_check_parameter(argc, argv);
    printf("Buffer Size: %i", test_buffer_size);

    fct_create_name(g_sem_name_send,1,SENDERID);
    printf("\nSEM NAME: %s",g_sem_name_send);

    g_p_sem_send = fct_sem_open_create(g_sem_name_send,test_buffer_size,SENDERID);

    //get sem pointer of empfänger
    fct_create_name(g_sem_name_empf,1,EMPFAENGERID); //create receiver name
    g_p_sem_empf=fct_sem_open_create(g_sem_name_empf,test_buffer_size,EMPFAENGERID);//to get pointer of receiver sem

    //create name for the shared memory
    fct_create_name(g_shm_name,2,SENDERID);
    printf("\nSHM NAME: %s",g_shm_name);

    g_fd_shm = fct_create_shared_mem(g_shm_name,test_buffer_size);
    fct_close();
    //Nur wenn Shared Memory noch nicht gemapped wurde mappen
    /*if(g_p_shm == NULL){
        g_p_shm = fct_map_shm(g_fd_shm,test_buffer_size);
    }

    while(g_fd_shm!=EOF){     //read as long as the char is not EOF
        sem_wait(g_p_sem_send);

        g_fd_shm=fgetc(stdin);
        sem_post(g_p_sem_empf);
        g_fd_shm++;

    }*/
    // synchronisation sem


    fct_unmap_shm(g_p_shm,test_buffer_size);

    /*fct_close_unlink_sem(g_sem_name_send,g_p_sem_send);
    fct_close_unlink_sem(g_sem_name_empf,g_p_sem_empf);
    fct_close_unlink_shm(g_shm_name,g_fd_shm);*/
    fct_close();

    return 0;
}