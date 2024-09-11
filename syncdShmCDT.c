// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "syncdShmADT.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>
#define TRUE 1
#define FALSE 0
#define PSHARED 1
#define MODE 0600

struct shm{
    sem_t * semp;
    char * buff;
    size_t buffSize;
};

struct syncdShmCDT{
    const char * shmName;
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
    if (O_CREAT & shFlags)
    {
        if ( ftruncate(fd, size + sizeof(sem_t)) == -1){
            perror("ftruncate");
            return NULL;
        }
    }
    void * shmp = mmap(NULL, size + sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED){
        perror("mmap");
        return NULL;
    }
    close(fd);
    syncdShmADT rta = malloc(sizeof(struct syncdShmCDT)); //@TODO: controlar malloc
    if (rta == NULL)
    {
        perror("malloc");
        munmap(shmp, size + sizeof(sem_t));
        return NULL;
    }
    rta->shmName = name;
    rta->shmem = malloc(sizeof(struct shm));
    if (rta->shmem == NULL)
    {
        free(rta);
        munmap(shmp, size + sizeof(sem_t) + sizeof(int));
        perror("malloc");
        return NULL;
    }
    
    rta->shmem->semp = shmp;
    if (startSem) //@TODO: ver si ponemos lo mismo que arriba
    {
        if( sem_init(rta->shmem->semp,PSHARED , 0) == -1 ){
            free(rta->shmem); 
            free(rta);  
            munmap(shmp, size + sizeof(sem_t));
            perror("sem_init");
            return NULL;
        }
    }
    rta->shmem->buff = shmp + sizeof(sem_t);
    rta->shmem->buffSize = size;
    rta->readPos = 0;
    rta->writePos = 0;
    return rta;
}
syncdShmADT createSyncdShm(const char * name , size_t size){
    return initADT(name , size , O_CREAT | O_TRUNC | O_RDWR , MODE , TRUE); 
}

// @TODO: revisar qu onda esto de destroy y close
void destroySyncdShm(syncdShmADT shmem){
    sem_destroy(shmem->shmem->semp);
    munmap(shmem->shmem->semp, sizeof(sem_t) + shmem->shmem->buffSize );
    shm_unlink(shmem->shmName);
    free(shmem->shmem);
    free(shmem);
}

int writeSyncdShm(syncdShmADT shmem , const char * buffer , size_t size ){ //@TODO: validar buffsize
    char * aux = shmem->shmem->buff + shmem->writePos;
    memcpy(aux , buffer , size + 1);
    shmem->writePos += size + 1;
    sem_post(shmem->shmem->semp);
    return size;
}

int readSyncdShm(syncdShmADT shmem , char * buffer){
    sem_wait(shmem->shmem->semp);
    char * aux = stpcpy(buffer , shmem->shmem->buff + shmem->readPos);
    shmem->readPos += aux - buffer + 1;
    return aux - buffer;
}

syncdShmADT openSyncdShm(const char * name , size_t size){
    return initADT(name , size , O_RDWR , 0 , FALSE);
}

void closeSyncdShm(syncdShmADT shmem){
    sem_close(shmem->shmem->semp); //@TODO: ver si es necesario
    munmap(shmem->shmem->semp, sizeof(sem_t) + shmem->shmem->buffSize );
    free(shmem->shmem);
    free(shmem);
}