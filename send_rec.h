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




#define CAL_SHM_ID(id,count) (1000*id+count)
#define SHM_NAME(string) strcat("/shm_",string)
// Functions

int fct_check_parameter(int argc, char *argv[]);
sem_t *fct_sem_open_create(const char *sem_name, int sem_size);
int create_shared_mem(char* name,int memsize);
char* create_sem_name(int identity);
#endif //BSP_3_SEND_REC_H
