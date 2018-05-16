//
// Created by ernstl on 16.05.18.
//
#include "send_rec.h"

// C File for all functions we are using in sender.c and receiver.c
int ringbuffer = 0;

//Parameter werden überprüft und Ringpuffer Größe zurückgegeben.

int fct_check_parameter(int argc, char *argv[]){
    int opt = 0;
    while((opt = getopt(argc, argv, "m:"))!=-1) {
        switch (opt) {
            case 'm':
                errno = 0;
                ringbuffer = strtol(optarg, (char **) NULL, 10);
                if (errno) {
                    fprintf(stderr,"\n%s: %s", argv[0], strerror(errno));
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr,"\n%s: wrong parameter used", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if(ringbuffer<1){
        fprintf(stderr,"\n%s: ringbuffer size to short [>0]", argv[0]);
        exit(EXIT_FAILURE);
    }
    return ringbuffer;
}

//Funktion checkt ob Semaphore bereits angelegt sind oder ob diese erst angelegt werden müssen.

sem_t *fct_sem_open_create(char *sem_name, int sem_size){
    sem_t *sem_pointer = NULL;

    sem_pointer = sem_open(sem_name,0);
    if(sem_pointer == SEM_FAILED){
        //Erzeugen der Semaphore falls noch nicht vorhanden
        //name, flags, mode, init value
        sem_pointer = sem_open(sem_name,O_CREAT|O_EXCL,0700,sem_size);
        if(sem_pointer == SEM_FAILED){
            fprintf(stderr,"\n%s: semaphore was not able to open"); //argv mitübergeben für Fehlermeldung?
            exit(EXIT_FAILURE);
        }
        else{
            printf("\nErzeugung Semaphore erfolgreich");
        }
    }
    else{
        printf("\nSemaphore waren bereits erzeugt");
    }
}

