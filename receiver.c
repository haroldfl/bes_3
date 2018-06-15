///
/// @file receiver.c
///
/// Betriebssysteme Receiver File
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

    //check the parameters von STDIN and return der shared memory size
    buffer_size = fct_check_parameter(argc, argv);

    //create a name for the senders semaphore
    fct_create_name(g_sem_name_send, 1, SENDERID);

    //Try to create a new semaphore for the sender and return the pointer to the semaphore
    g_p_sem_send = fct_sem_open_create(g_sem_name_send, buffer_size, SENDERID);

    //create a name for the senders semaphore
    fct_create_name(g_sem_name_empf, 1, EMPFAENGERID); //create receiver name
    
    //Try to create a new semaphore for the sender and return the pointer to the semaphore
    g_p_sem_empf = fct_sem_open_create(g_sem_name_empf, buffer_size, EMPFAENGERID);

    //create name for the shared memory
    fct_create_name(g_shm_name, 2, SENDERID);

    //Try to create a new shared memory and return the pointer to the shared memory
    g_fd_shm = fct_create_shared_mem(g_shm_name, buffer_size, argv);

    //check if shared memory is already mapped, only if it's not map the shared memory
    g_p_shm = fct_map_shm(g_fd_shm, buffer_size, EMPFAENGERID);

    //loop for receving messages from the sender
    do {
        //waits until g_p_sem_empf value is more than zero which indicates a new character in the shared memory
        sem_wait(g_p_sem_empf);
	    //if the character indicates EOF(End of File) stop receiving
        if (g_p_shm[i] == EOF) {
            end = 1;
        } else {
            //Write the received character on STDOUT
            fputc(g_p_shm[i], stdout);
	        //Raise the value from the senders semaphore to wake the semaphore up to send a character
            sem_post(g_p_sem_send);
        }
           
        i = (i + 1) % buffer_size;
    } while (end != 1);
    
    //unmap shared memory
    fct_unmap_shm(g_p_shm, buffer_size);

    fct_close();

    return 0;
}













