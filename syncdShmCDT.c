#include "syncdShmADT.h"
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>

#define TRUE 1
#define FALSE 0
#define PSHARED 1
struct shm{
    sem_t * semp;
    int * avail;
    void * buff;
    size_t buffSize;
};

struct syncdShmCDT{
    char * shmName;
    struct shm * shmem;
    int readPos;
    int writePos;
}; 
syncdShmADT initADT(const char * name , size_t size , int shFlags , int mode ,int startSem ){
    int fd = shm_open(name, shFlags, mode);
    if (fd == -1){
        perror("shm_open");
        return NULL;
    }
    if (O_CREAT & shFlags )
    {
        if ( ftruncate(fd, size + sizeof(sem_t) + sizeof(int)) == -1){
            perror("ftruncate");
            return NULL;
        }
    }
    void * shmp = mmap(NULL, size + sizeof(sem_t) + sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED){
        perror("mmap");
        return NULL;
    }
    //close(fd); @TODO: va esto?
    syncdShmADT rta = malloc(sizeof(struct syncdShmCDT));
    rta->shmName = name;
    rta->shmem = mallloc(sizeof(struct shm));
    rta->shmem->semp = shmp;
    if (startSem)
    {
        if( sem_init(rta->shmem->semp,PSHARED , 0) == -1 ){
            destroySyncdShm(rta); //@TODO: ver si deberiamos liberar todo ante un error (para todos los casos) 
            perror("sem_init");
            return NULL;
        }
    }
    rta->shmem->avail = shmp + sizeof(sem_t);
    rta->shmem->buff = shmp + sizeof(sem_t) + sizeof(int);
    rta->shmem->buffSize = size;
    rta->readPos = 0;
    rta->writePos = 0;
    return shmp;
}
syncdShmADT createSyncdShm(const char * name , size_t size){
    return initShm(name , size , O_CREAT | O_TRUNC | O_RDWR , 0600 , TRUE); 
}

// @TODO: revisar qu onda esto de destroy y close
void destroySyncdShm(syncdShmADT shmem){
    munmap(shmem->shmem->semp, sizeof(sem_t) + shmem->shmem->buffSize );
    shm_unlink(shmem->shmName);
    free(shmem->shmem);
    free(shmem);
}

int writeSyncdShm(syncdShmADT shmem , const void * buffer , size_t size ){
    memcpy(shmem->shmem->buff + shmem->writePos , buffer , size );
    shmem->writePos += size;
    post(shmem->shmem->semp);
    return *(shmem->shmem->avail);
}

int readSyncdShm(syncdShmADT shmem , void * buffer){
    wait(shmem->shmem->semp);
    memcpy(buffer , shmem->shmem->buff + shmem->readPos, *(shmem->shmem->avail) );
    shmem->readPos += *(shmem->shmem->avail);
    return *(shmem->shmem->avail);
}

syncdShmADT openSyncdShm(const char * name , size_t size){
    return initADT(name , size , O_RDWR , 0 , FALSE);
}

void closeSyncdShm(syncdShmADT shmem){

}