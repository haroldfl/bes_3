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

sem_t *fct_sem_open_create(const char *sem_name, int sem_size){

    sem_t *sem_pointer;
    long i=strlen(sem_name);

    char test[20];
    strcpy(test,sem_name);

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
int create_shared_mem( char* name,int memsize){

    int shm_fd=0;
    int name_length=0;
//öffnen des Shared Memory Bereichs falls vorhanden
    name_length=strlen(name);
    name[name_length]='\0';
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





}

char* create_sem_name(int identity){

    char shm_name[20];

    char shm_basic[40] = "/shm_";
    char sem_basic[40] = "/sem_";



    snprintf(shm_name,20,"%d",CAL_SHM_ID(getuid(),0));

    if(identity==1) {
        strcat(shm_name,"\0");
        return strcat(sem_basic, shm_name);



    }else if(identity==2){
        return strcat(shm_basic, shm_name);
    }

}
