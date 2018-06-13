///
/// @file sender.c
///
/// Betriebssysteme Sender File
/// Beispiel 3
///
/// @author Ibrahim Milli <ic17b063@technikum-wien.at>
/// @author Dominic Ernstbrunner <ic17b015@technikum-wien.at>
/// @author Florian Harold <ic17b093@technikum-wien.at>
/// @date 2018/06/13
///
/// @version 100
///
/// -------------------------------------------------------------- includes --

#include "send_rec.h"

/// --------------------------------------------------------------- globals --

extern char g_sem_name_empf[40];
extern char g_sem_name_send[40];
extern char g_shm_name[40];
extern sem_t *g_p_sem_send;
extern sem_t *g_p_sem_empf;
extern int g_fd_shm;
extern int *g_p_shm;

/// ------------------------------------------------------------- functions --

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
    g_p_shm = fct_map_shm(g_fd_shm, buffer_size, SENDERID);

    do {
        sem_wait(g_p_sem_send);     // counts the writing places on down
        g_p_shm[i] = fgetc(stdin);
        if (g_p_shm[i] == EOF) {
            end = 1;
        }
        sem_post(g_p_sem_empf);      // counts the reading places on up
        i = (i + 1) % buffer_size;
    } while (end != 1);

    fct_unmap_shm(g_p_shm, buffer_size);
    return 0;
}










