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

//Testbeispiel zu Beispiel 3 BES

int main() {
    //variables
    sem_t *sem_pointer = NULL;
    char e_char = "";
    int sem_value = 0;
    int shm_fd = 0; //Filedeskritpor Shared Memory
    int shm_groesse = 0;
    int e_int_shm = 0;
    //int * const shm_pointer;

    //Hier wird der Semaphor nur geöffnet falls dieser bereits erzeugt wurde.
    sem_pointer = sem_open("/sem",0);
    if(sem_pointer == SEM_FAILED){
        //Erzeugen der Semaphore falls noch nicht vorhanden
        //name, flags, mode, init value
        sem_pointer = sem_open("/sem",O_CREAT|O_EXCL,0700,20);
        if(sem_pointer == SEM_FAILED){
            printf("\nFehler Semaphore open");
            exit(EXIT_FAILURE);
        }
        else{
            printf("\nErzeugung Semaphore erfolgreich");
        }
    }
    else{
        printf("\nSemaphore waren bereits erzeugt");
    }

    do{
        //Für die Wertigkeit würde bei einem Fehler ebenso das errno gesetzt.
        if(sem_getvalue(sem_pointer,&sem_value)==-1){
            printf("\nFehler Semaphore Wert holen");
        }
        else{
            printf("\nAktueller Semaphor Wert = %i",sem_value);
        }
        printf("\n[d]ekrementieren, [i]nkrementieren, [x]close: ");
        scanf("\n%c",&e_char);
        if(e_char == 'd'){
            //sem_wait return 0 --> Sucess
            if(!sem_wait(sem_pointer)){
                printf("\nErfolgreich dekrementiert: %i", *sem_pointer);
            } else{
                printf("\nFehler dekrementieren");
                exit(EXIT_FAILURE);
            }
        }
        else if(e_char == 'i'){
            //sem_wait return 0 --> Sucess
            if(!sem_post(sem_pointer)){
                printf("\nErfolgreich inkrementiert: %i", *sem_pointer);
            } else{
                printf("\nFehler inkrementieren");
                exit(EXIT_FAILURE);
            }
        }
    }while(e_char != 'x');

    //schließen Semaphore
    if(sem_close(sem_pointer)==-1){
        printf("\nFehler schließen");
    } else{
        printf("\nErfolgreich geschlossen");
    }

    //freigeben Semaphore
    if(sem_unlink("/sem")==-1){
        printf("\nFehler freigeben");
    } else{
        printf("\nErfolgreich freigegeben");
    }

    //----------------------------------------
    //###### Shared Memory ############
    //----------------------------------------

    //öffnen des Shared Memory Bereichs falls vorhanden
    shm_fd = shm_open("/shm",O_RDONLY|O_WRONLY,S_IRWXU);
    if(shm_fd == -1){
        //Erzeugen des Shared Memory falls noch nicht vorhanden
        //name, flags, mode, init value
        shm_fd = shm_open("/shm",O_CREAT|O_EXCL|O_RDWR,S_IRWXU);
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

    printf("\nGroesse des Shared Memories: ");
    scanf("%i",&shm_groesse);

    //Größe bestimmen, hierfür muss das SHM schreibend geöffnet werden.
    if(ftruncate(shm_fd, sizeof(int)*shm_groesse)==-1){
        printf("\nFehler beim Festlegen der Groesse des SHM");
    }
    else{
        printf("\nGroesse wurde erfolgreich festgelegt");
    }

    //Einblenden des Shared Memories
    int *shm_pointer = mmap(NULL, (sizeof(int)*shm_groesse), PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_pointer == (int *)MAP_FAILED){
        printf("\nEinblenden des SHM fehlerhaft");
        printf("\nERRRNO: %s", strerror(errno));
    }
    else{
        printf("\nEinblenden des SHM erfolgreich");
    }

    printf("\nEingabe SHM:");
    scanf("%i",&e_int_shm);

    *shm_pointer = e_int_shm;

    //Ausblenden des Bereiches
    if(munmap(shm_pointer, (sizeof(int)*shm_groesse)) == -1){
        printf("\nAusblenden des SHM fehlerhaft");
        printf("\nERRRNO: %s", strerror(errno));
    }
    else{
        printf("\nAusblenden des SHM erfolgreich");
    }

    //Schließen Filedeskriptor
    if(close(shm_fd)==-1){
        printf("\nSchließen des SHM Filedeskriptors fehlerhaft");
    }
    else{
        printf("\nAusblenden des SHM Filedeskriptors erfolgreich");
    }

    //Freigeben des SHM
    if(shm_unlink("/shm") == -1){
        printf("\nFreigeben des SHM fehlerhaft");
    }
    else{
        printf("\nFreigeben des SHM erfolgreich");
    }

    printf("\nTOLL!\n");
    return 0;
}