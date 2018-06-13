//
// Created by ernstl on 16.05.18.
//
///
/// @file send_rec.c
///
/// Betriebssysteme Sender Receiver Function File
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
char g_sem_name_empf[40];
char g_sem_name_send[40];
char g_shm_name[40];
sem_t *g_p_sem_send = NULL;
sem_t *g_p_sem_empf = NULL;
int g_fd_shm = -2;
int *g_p_shm = NULL;
/// ------------------------------------------------------------- functions --

/// fct_close
///
/// This function close every semaphore and shared memory

void fct_close(void) {
    fct_close_unlink_sem(g_sem_name_empf, g_p_sem_empf);
    fct_close_unlink_sem(g_sem_name_send, g_p_sem_send);
    fct_close_unlink_shm(g_shm_name, g_fd_shm);
}

/// fct_check_parameter
///
/// This function checks every argument
/// If an argument is wrong the program interrupt with the usage handling
/// If the arguments are correct the funktion returns the size of the buffer
///
/// \param argc number of parameter
/// \param argv char array of the parameter
/// \return g_ringbuffer the size of the ringbuffer

int fct_check_parameter(int argc, char *argv[]) {
    int g_ringbuffer = -1;
    int opt = 0;
    char *error;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s ", argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((opt = getopt(argc, argv, "m:")) != -1) {
        switch (opt) {
            case 'm':
                errno = 0;
                g_ringbuffer = strtol(optarg, &error, 10);
                if (errno || *error != '\0' || error == optarg) {
                    fprintf(stderr, "Usage: %s", argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "Usage: %s ", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind < argc) {
        fprintf(stderr, "Usage: %s ", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (g_ringbuffer < 1) {
        fprintf(stderr, "Usage: %s ", argv[0]);
        exit(EXIT_FAILURE);
    }
    return g_ringbuffer;
}

/// fct_create_name
///
/// This function checks every argument
/// If an argument is wrong the program interrupt with the usage handling
/// If the arguments are correct the funktion returns the size of the buffer
///
/// \param *name pointer where the name should be written
/// \param identity 1..semaphore | 2..shared memory
/// \param id_sem id for the semaphore. To distinguish between sender and receiver semaphore

void fct_create_name(char *name, int identity, int id_sem) {

    char shm_name[20];
    char shm_basic[40] = "/shm_";
    char sem_basic[40] = "/sem_";

    snprintf(shm_name, 20, "%d", CAL_SHM_ID(getuid(), id_sem));

    if (identity == 1) {
        strcat(sem_basic, shm_name);
        strcpy(name, sem_basic);
    } else if (identity == 2) {
        strcat(shm_basic, shm_name);
        strcpy(name, shm_basic);
    }
}


/// fct_sem_open_create
///
/// Checks if the semaphore was created. If not this function creates creates the semaphore.
///
/// \param *sem_name name of the semaphore
/// \param sem_size buffersize of the ringbuffer to init the receiver semaphore
/// \param sem_type type of the semaphore 1...sender | 2...receiver
/// \return sem_pointer the pointer to the semaphore

sem_t *fct_sem_open_create(const char *sem_name, int sem_size, int sem_type) {
    sem_t *sem_pointer = NULL;

    sem_pointer = sem_open(sem_name, 0);
    if (sem_pointer == SEM_FAILED) {
        //Erzeugen der Semaphore falls noch nicht vorhanden
        //name, flags, mode, init value
        if (sem_type == SENDERID) {
            sem_pointer = sem_open(sem_name, O_CREAT | O_EXCL, 0700, sem_size);
        } else if (sem_type == EMPFAENGERID) {
            sem_pointer = sem_open(sem_name, O_CREAT | O_EXCL, 0700, 0);
        }
        if (sem_pointer == SEM_FAILED) {
            fprintf(stderr, "\nsemaphore was not able to open"); //argv mitübergeben für Fehlermeldung?
            exit(EXIT_FAILURE);
        }
    }
    return sem_pointer;
}

/// fct_close_unlink_sem
///
/// Unlink the semaphore from the program
///
/// \param *name name of the semaphore
/// \param *sem_pointer pointer of the semaphore to unlink
/// \return 0: success 1:error

int fct_close_unlink_sem(char *name, sem_t *sem_pointer) {
    //schließen Semaphore
    if (sem_close(sem_pointer) == -1) {
        return 1;
    }
    //freigeben Semaphore
    if (sem_unlink(name) == -1) {
        return 1;
    }
    return 0;
}

/// fct_create_shared_mem
///
/// This function checks if the shared memory exists.
/// If not, the function creates the shared memory with the size of the memsize
///
/// \param *name pointer where the name should be written
/// \param memsize buffersize of the shared memory
/// \param argv parameter for the usage handling

int fct_create_shared_mem(char *name, int memsize, char *argv[]) {
    int shm_fd = -2;
    //öffnen des Shared Memory Bereichs falls vorhanden
    //shm_fd = shm_open(name,O_RDONLY|O_WRONLY,S_IRWXU);
    shm_fd = shm_open(name, O_RDWR, 0);
    if (shm_fd == -1) {
        //Erzeugen des Shared Memory falls noch nicht vorhanden
        //name, flags, mode, init value
        shm_fd = shm_open(name, O_CREAT | O_EXCL | O_RDWR, S_IRWXU);
        if (shm_fd == -1) {
            fprintf(stderr, "Usage: %s", argv[0]);
            exit(EXIT_FAILURE);
        } else {
            //Größe bestimmen, hierfür muss das SHM schreibend geöffnet werden.
            if (ftruncate(shm_fd, sizeof(int) * memsize) == -1) {
                fprintf(stderr, "Usage: %s", argv[0]);
                exit(EXIT_FAILURE);
            }
        }
    } else {
        if (ftruncate(shm_fd, sizeof(int) * memsize) == -1) {
            fprintf(stderr, "Usage: %s", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    return shm_fd;
}

/// fct_map_shm
///
/// This function maps the shared memory to the program.
///
/// \param shm_fd filedescriptor of the shared memory
/// \param memsize buffersize of the shared memory
/// \param type of the shared memory 1...sender | 2...receiver
/// \return shm_pointer pointer of the shared memory

int *fct_map_shm(int shm_fd, int memsize, int type) {
    int *shm_pointer = NULL;

    if (type == SENDERID) {
        shm_pointer = mmap(NULL, (sizeof(int) * memsize), PROT_WRITE, MAP_SHARED, shm_fd, 0);
    } else if (type == EMPFAENGERID) {
        shm_pointer = mmap(NULL, (sizeof(int) * memsize), PROT_READ, MAP_SHARED, shm_fd, 0);
    }
    if (shm_pointer == (int *) MAP_FAILED) {
        exit(EXIT_FAILURE);
    }
    return shm_pointer;
}

/// fct_unmap_shm
///
/// This function unmaps the shared memory from the program.
///
/// \param *p_shm pointer of the shared memory
/// \param memsize buffersize of the shared memory
/// \return 0: success 1: error

int fct_unmap_shm(int *p_shm, int memsize) {
    //Ausblenden des Bereiches
    if (munmap(p_shm, (sizeof(int) * memsize)) == -1) {
        return 1;
    }
    return 0;
}

/// fct_close_unlink_shm
///
/// This function close and unlink the shared memory from the program.
///
/// \param *name name of the shared memory
/// \param shm_fd filedescriptor of the shared memory
/// \return 0: success 1: error

int fct_close_unlink_shm(char *name, int shm_fd) {
    //Schließen Filedeskriptor
    if (close(shm_fd) == -1) {
        return 1;
    }

    //Freigeben des SHM
    if (shm_unlink(name) == -1) {
        return 1;
    }
    return 0;
}



