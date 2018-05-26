
   
#define KEY (1000*getuid()+2);

 
    int shm_id;
    static int *shm_pt;
    



int createSharedMemory(void){
    
    shm_id = shmget ( KEY, sizeof(int) * buffsize, 0660 |IPC_CREAT);
    
    if (shm_id == -1)
    {
        perror("shmget");
		freeRessources();
        return -1;
    }
    
    return 0;
}

int attachSharedMemory(int flag)
{
    
    if((shm_pt = shmat(shm_id, NULL, flag)) == (int*) -1){
       
        perror("shmat");
		return -1;
    }
       
    return 0;
    
       
}

int detachSharedMemory(void)
{
    if (shm_pt != NULL) {
        if (shmdt(shm_pt) == -1) {
           perror("shmdt");
		   shm_pt = NULL;
            return -1;
        }
        shm_pt = NULL;
    }
    return 0;
}

int deleteSharedMemory(void)
{
    if (shm_id != -1)
    {
        if (shmctl(shm_id, IPC_RMID, NULL) == -1)
        {
            perror("shmctl");
			shm_id = -1;
            return -1;
        }
        shm_id = -1;
    }
    return 0;
}