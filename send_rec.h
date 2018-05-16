//
// Created by ernstl on 16.05.18.
//

#ifndef BSP_3_SEND_REC_H
#define BSP_3_SEND_REC_H

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

// Functions

int fct_check_parameter(int argc, char *argv[]);
sem_t *fct_sem_open_create(char *sem_name, int sem_size);

#endif //BSP_3_SEND_REC_H
