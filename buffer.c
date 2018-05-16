
char buf[buffsize];
char *pSI, *pLI, *pEnd;
char full;


void buf_init()
{
    pSI = pLI = buf;       
    pEnd = &buf[buffsize];   
    full = 0;               
}


int buf_put(char c)
{
    if (pSI == pLI  &&  full)
        return 0;          

    *pSI = c;             
     pSI++;
     if (pSI >= pEnd)        
        pSI = buf;          

    if (pSI == pLI)        
        full = 1;           
    return 1;              
}


int buf_get(char *pc)
{
    if (pIn == pOut  &&  !full)
        return 0;           

    *pc = *pLI;             
     pLI++;
    if (pLI >= pEnd)       
        pLI = buf;        

    full = 0;               
    return 1;               
}