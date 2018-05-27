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

void fct_create_name(char *name, int identity){

    char shm_name[20];

    char shm_basic[40] = "/shm_";
    char sem_basic[40] = "/sem_";

    snprintf(shm_name,20,"%d",CAL_SHM_ID(getuid(),0));

    if(identity==1) {
        strcat(sem_basic, shm_name);
        strcpy(name,sem_basic);
    }else if(identity==2){
        strcat(shm_basic, shm_name);
        strcpy(name,shm_basic);
    }
}

//Funktion checkt ob Semaphore bereits angelegt sind oder ob diese erst angelegt werden müssen.

sem_t *fct_sem_open_create(const char *sem_name, int sem_size){

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
    return sem_pointer;
}

int fct_edit_sem(char option, sem_t *sem_pointer){
    int sem_value = 0;
    switch (option) {
        case 'd': //dekrementieren
            //sem_wait return 0 --> Sucess
            if(!sem_wait(sem_pointer)){
                printf("\nErfolgreich dekrementiert: %p", sem_pointer);
            } else{
                printf("\nFehler dekrementieren");
                return 1;
            }
            break;
        case 'i': //inkrementieren sem
            //sem_post return 0 --> Sucess
            if(!sem_post(sem_pointer)){
                printf("\nErfolgreich inkrementiert: %p", sem_pointer);
            } else{
                printf("\nFehler inkrementieren");
                return 1;
            }
            break;
        case 's': //show
            if(sem_getvalue(sem_pointer,&sem_value)==-1){
                printf("\nFehler Semaphore Wert holen");
                return 1;
            }
            else{
                printf("\nAktueller Semaphor Wert = %i",sem_value);
            }
            break;
        default:
            printf("\nUngueltige Option wurde uebergeben!");
            return 1;
            break;
    }
    return 0;
}

int fct_close_unlink_sem(char *name, sem_t *sem_pointer){

    //schließen Semaphore
    if(sem_close(sem_pointer)==-1){
        printf("\nFehler schließen");
        return 1;
    } else{
        printf("\nErfolgreich geschlossen");
    }

    //freigeben Semaphore
    if(sem_unlink(name)==-1){
        printf("\nFehler freigeben");
        return 1;
    } else{
        printf("\nErfolgreich freigegeben");
    }
    return 0;
}


int fct_create_shared_mem(char* name,int memsize){

    int shm_fd=-2;
//öffnen des Shared Memory Bereichs falls vorhanden

    shm_fd = shm_open(name,O_RDONLY|O_WRONLY,S_IRWXU);
    if(shm_fd == -1){
        //Erzeugen des Shared Memory falls noch nicht vorhanden
        //name, flags, mode, init value
        shm_fd = shm_open(name,O_CREAT|O_EXCL|O_RDWR,S_IRWXU);
        if(shm_fd == -1){
            printf("\nFehler Shared Memory open");
            exit(EXIT_FAILURE);
        }
        else{
            printf("\nErzeugung Shared Memory erfolgreich");
        }
    }
    else{
        printf("\nShared Memory war bereits erzeugt");
    }

    //Größe bestimmen, hierfür muss das SHM schreibend geöffnet werden.
    if(ftruncate(shm_fd, sizeof(int)*memsize)==-1){
        printf("\nFehler beim Festlegen der Groesse des SHM");
    }
    else{
        printf("\nGroesse wurde erfolgreich festgelegt");
    }
    return shm_fd;
}

int *fct_map_shm(int shm_fd, int memsize){
    int *shm_pointer = NULL;
    shm_pointer = mmap(NULL, (sizeof(int)*memsize), PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_pointer == (int *)MAP_FAILED){
        printf("\nEinblenden des SHM fehlerhaft");
        printf("\nERRRNO: %s", strerror(errno));
        return NULL;
    }
    else{
        printf("\nEinblenden des SHM erfolgreich");
    }
    return shm_pointer;
}

int fct_unmap_shm(int *p_shm, int memsize){
    //Ausblenden des Bereiches
    if(munmap(p_shm, (sizeof(int)*memsize)) == -1){
        printf("\nAusblenden des SHM fehlerhaft");
        printf("\nERRRNO: %s", strerror(errno));
        return 1;
    }
    else {
        printf("\nAusblenden des SHM erfolgreich");
    }
    return 0;
}

int fct_close_unlink_shm (char *name, int shm_fd){
    //Schließen Filedeskriptor
    if(close(shm_fd)==-1){
        printf("\nSchließen des SHM Filedeskriptors fehlerhaft");
        return 1;
    }
    else{
        printf("\nAusblenden des SHM Filedeskriptors erfolgreich");
    }

    //Freigeben des SHM
    if(shm_unlink(name) == -1){
        printf("\nFreigeben des SHM fehlerhaft");
        return 1;
    }
    else{
        printf("\nFreigeben des SHM erfolgreich");
    }
    return 0;
}



