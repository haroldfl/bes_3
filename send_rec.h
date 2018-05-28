#ifndef SEND_REC_H
#define SEND_REC_H

//Für Semaphore
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
//Für Shared Memory
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <memory.h>
#include <errno.h>

#define SENDERID 1
#define EMPFAENGER 2
#define CAL_SHM_ID(id,count) (1000*id+count)
#define SHM_NAME(string) strcat("/shm_",string)
// Functions

int fct_check_parameter(int argc, char *argv[]);
sem_t *fct_sem_open_create(const char *sem_name, int sem_size,int sem_type);
int fct_edit_sem(char option, sem_t *sem_pointer);
int fct_close_unlink_sem(char *name, sem_t *sem_pointer);
int fct_create_shared_mem(char* name,int memsize);
int *fct_map_shm(int shm_fd, int memsize);
int fct_unmap_shm(int *p_shm, int memsize);
int fct_close_unlink_shm (char *name, int shm_fd);
void fct_create_name(char *name, int identity, int id_sem);
#endif //BSP_3_SEND_REC_H
