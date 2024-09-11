#ifndef SYNCDSHMADT_SHM
#define SYNCDSHMADT_SHM
#include <stdio.h>

typedef struct syncdShmCDT * syncdShmADT;

syncdShmADT createSyncdShm(const char * name , size_t size);

void destroySyncdShm(syncdShmADT shmem);

int writeSyncdShm(syncdShmADT shmem , const void * buffer , size_t size);

int readSyncdShm(syncdShmADT shmem , void * buffer );

syncdShmADT openSyncdShm(const char * name , size_t size);

void closeSyncdShm(syncdShmADT shmem);

#endif