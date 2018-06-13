#include "send_rec.h"

int main(int argc, char *argv[]) {
    int buffer_size = 0;
    int i = 0;
    int end = 0;

    buffer_size = fct_check_parameter(argc, argv);

    fct_create_name(g_sem_name_send, 1, SENDERID);

    g_p_sem_send = fct_sem_open_create(g_sem_name_send, buffer_size, SENDERID);

    //get sem pointer of empfänger
    fct_create_name(g_sem_name_empf, 1, EMPFAENGERID); //create receiver name
    g_p_sem_empf = fct_sem_open_create(g_sem_name_empf, buffer_size, EMPFAENGERID);//to get pointer of receiver sem

    //create name for the shared memory
    fct_create_name(g_shm_name, 2, SENDERID);

    g_fd_shm = fct_create_shared_mem(g_shm_name, buffer_size, argv);

    //Nur wenn Shared Memory noch nicht gemapped wurde mappen
    g_p_shm = fct_map_shm(g_fd_shm, buffer_size, EMPFAENGERID);

    do {
        sem_wait(g_p_sem_empf);    // counts the reading places one down
        if (g_p_shm[i] == EOF) {
            end = 1;
        } else {
            fputc(g_p_shm[i], stdout);
        }
        sem_post(g_p_sem_send);   // counts the writing places one up
        i = (i + 1) % buffer_size;
    } while (end != 1);

    fct_unmap_shm(g_p_shm, buffer_size);

    fct_close();

    return 0;
}













