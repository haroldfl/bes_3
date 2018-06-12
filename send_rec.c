//
// Created by ernstl on 16.05.18.
//
#include "send_rec.h"

char g_sem_name_empf[40];
char g_sem_name_send[40];
char g_shm_name[40];
sem_t *g_p_sem_send = NULL;
sem_t *g_p_sem_empf = NULL;
int g_fd_shm = -2;
int *g_p_shm = NULL;

// C File for all functions we are using in sender.c and receiver.c
void fct_close(void){
    fct_close_unlink_sem(g_sem_name_empf,g_p_sem_empf);
    fct_close_unlink_sem(g_sem_name_send,g_p_sem_send);
    fct_close_unlink_shm(g_shm_name,g_fd_shm);
}

//Parameter werden überprüft und Ringpuffer Größe zurückgegeben.

int fct_check_parameter(int argc, char *argv[]){
    int g_ringbuffer = -1;
    int opt = 0;
char *error;


if(argc<2){

fprintf(stderr,"Usage: %s ", argv[0]);
                exit(EXIT_FAILURE);
}    

while((opt = getopt(argc, argv, "m:"))!=-1) {
    
	
	switch (opt) {
            case 'm':
                errno = 0;
                
		g_ringbuffer = strtol(optarg, &error, 10);
                if (errno || *error!='\0' || error==optarg) {
                fprintf(stderr,"Usage: %s", argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr,"Usage: %s ", argv[0]);
                exit(EXIT_FAILURE);
        }
 
}
if(optind<argc){
fprintf(stderr,"Usage: %s ", argv[0]);
                exit(EXIT_FAILURE);

}    
if(g_ringbuffer<1){
        fprintf(stderr,"Usage: %s ", argv[0]);
       exit(EXIT_FAILURE);
    }
    return g_ringbuffer;
}

void fct_create_name(char *name, int identity, int id_sem){

    char shm_name[20];

    char shm_basic[40] = "/shm_";
    char sem_basic[40] = "/sem_";

    snprintf(shm_name,20,"%d",CAL_SHM_ID(getuid(),id_sem));

    if(identity==1) {
        strcat(sem_basic, shm_name);
        strcpy(name,sem_basic);
    }else if(identity==2){
        strcat(shm_basic, shm_name);
        strcpy(name,shm_basic);
    }
}

//Funktion checkt ob Semaphore bereits angelegt sind oder ob diese erst angelegt werden müssen.
//Eigentlich ist der einzige Unterschied zwischen Typ 1 und 2 die Größe, bräuchten das if nicht doppelt moppeln
sem_t *fct_sem_open_create(const char *sem_name, int sem_size,int sem_type){

    sem_t *sem_pointer = NULL;
    if(sem_type==1) {
        sem_pointer = sem_open(sem_name, 0);
        if (sem_pointer == SEM_FAILED) {
            //Erzeugen der Semaphore falls noch nicht vorhanden
            //name, flags, mode, init value
            sem_pointer = sem_open(sem_name, O_CREAT | O_EXCL, 0700, sem_size);
            if (sem_pointer == SEM_FAILED) {
                fprintf(stderr, "\nsemaphore was not able to open"); //argv mitübergeben für Fehlermeldung?
                exit(EXIT_FAILURE);
            } else {
//                printf("\nErzeugung Semaphore erfolgreich");
            }
        } else {
  //          printf("\nSemaphore waren bereits erzeugt");
        }
    }else if(sem_type==2) {
        sem_pointer = sem_open(sem_name, 0);
        if (sem_pointer == SEM_FAILED) {
            //Erzeugen der Semaphore falls noch nicht vorhanden
            //name, flags, mode, init value
            sem_pointer = sem_open(sem_name, O_CREAT | O_EXCL, 0700, 0);
            if (sem_pointer == SEM_FAILED) {
                fprintf(stderr, "\nsemaphore was not able to open"); //argv mitübergeben für Fehlermeldung?
                exit(EXIT_FAILURE);
            } else {
//                printf("\nErzeugung Semaphore erfolgreich");
            }
        } else {
  //          printf("\nSemaphore waren bereits erzeugt");
        }
    }
    return sem_pointer;
}

int fct_edit_sem(char option, sem_t *sem_pointer){
    int sem_value = 0;
    switch (option) {
        case 'd': //dekrementieren
            //sem_wait return 0 --> Sucess
            if(!sem_wait(sem_pointer)){
    //            printf("\nErfolgreich dekrementiert");
            } else{
      //          printf("\nFehler dekrementieren");
                return 1;
            }
            break;
        case 'i': //inkrementieren sem
            //sem_post return 0 --> Sucess
            if(!sem_post(sem_pointer)){
        //        printf("\nErfolgreich inkrementiert");
            } else{
          //      printf("\nFehler inkrementieren");
                return 1;
            }
            break;
        case 's': //show
            if(sem_getvalue(sem_pointer,&sem_value)==-1){
            //    printf("\nFehler Semaphore Wert holen");
                return 1;
            }
            else{
            //    printf("\nAktueller Semaphor Wert = %i",sem_value);
            }
            break;
        default:
           // printf("\nUngueltige Option wurde uebergeben!");
            return 1;
            break;
    }
    return 0;
}

int fct_close_unlink_sem(char *name, sem_t *sem_pointer){

    //schließen Semaphore
    if(sem_close(sem_pointer)==-1){
        //printf("\nFehler schließen");
        return 1;
    } else{
       // printf("\nErfolgreich geschlossen");
    }

    //freigeben Semaphore
    if(sem_unlink(name)==-1){
        //printf("\nFehler freigeben");
        return 1;
    } else{
       // printf("\nErfolgreich freigegeben");
    }
    return 0;
}


int fct_create_shared_mem(char* name,int memsize,char* argv[]){
    int shm_fd=-2;
//öffnen des Shared Memory Bereichs falls vorhanden

    //shm_fd = shm_open(name,O_RDONLY|O_WRONLY,S_IRWXU);
    shm_fd = shm_open(name,O_RDWR,0);
    if(shm_fd == -1){
        //Erzeugen des Shared Memory falls noch nicht vorhanden
        //name, flags, mode, init value
        shm_fd = shm_open(name,O_CREAT|O_EXCL|O_RDWR,S_IRWXU);
        if(shm_fd == -1){
            fprintf(stderr,"Usage: %s", argv[0]);
            exit(EXIT_FAILURE);
        }
        else{
         //   printf("\nErzeugung Shared Memory erfolgreich");
            //Größe bestimmen, hierfür muss das SHM schreibend geöffnet werden.
            if(ftruncate(shm_fd, sizeof(int)*memsize)==-1){
                fprintf(stderr,"Usage: %s", argv[0]);
            exit(EXIT_FAILURE);
            }
            else{
           //     printf("\nGroesse wurde erfolgreich festgelegt");
            }
        }
    }
    else{
        if(ftruncate(shm_fd, sizeof(int)*memsize)==-1){
                fprintf(stderr,"Usage: %s", argv[0]);
            exit(EXIT_FAILURE);
            }

    }
    return shm_fd;
}

int *fct_map_shm(int shm_fd, int memsize, int type){
    int *shm_pointer = NULL;
    errno = 0;
    if(type == SENDERID){
        shm_pointer = mmap(NULL, (sizeof(int)*memsize), PROT_WRITE, MAP_SHARED, shm_fd, 0);
    }
    else if(type == EMPFAENGERID){
        shm_pointer = mmap(NULL, (sizeof(int)*memsize), PROT_READ, MAP_SHARED, shm_fd, 0);
    }
    if(shm_pointer == (int *)MAP_FAILED){
       // printf("\nEinblenden des SHM fehlerhaft1");
       // printf("\nERRRNO: %s", strerror(errno)); //strerror(errno));
        exit(EXIT_FAILURE);
        //return NULL;
    }
    else{
       // printf("\nEinblenden des SHM erfolgreich");
    }
    return shm_pointer;
}

int fct_unmap_shm(int *p_shm, int memsize){
    //Ausblenden des Bereiches
    if(munmap(p_shm, (sizeof(int)*memsize)) == -1){
       // printf("\nAusblenden des SHM fehlerhaft");
       // printf("\nERRRNO: %s", strerror(errno));
        return 1;
    }
    else {
       // printf("\nAusblenden des SHM erfolgreich");
    }
    return 0;
}

int fct_close_unlink_shm (char *name, int shm_fd){
    //Schließen Filedeskriptor
    if(close(shm_fd)==-1){
       // printf("\nSchließen des SHM Filedeskriptors fehlerhaft");
        return 1;
    }
    else{
       // printf("\nAusblenden des SHM Filedeskriptors erfolgreich");
    }

    //Freigeben des SHM
    if(shm_unlink(name) == -1){
       // printf("\nFreigeben des SHM fehlerhaft");
        return 1;
    }
    else{
       // printf("\nFreigeben des SHM erfolgreich");
    }
    return 0;
}



