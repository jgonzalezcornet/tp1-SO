#ifndef SYNCDSHMADT_SHM
#define SYNCDSHMADT_SHM

#include <stdio.h>

typedef struct syncdShmCDT * syncdShmADT;

/**
 * Creates shared memory
 * @returns syncdShmADT, pointer to the created structure with all data
 * @param name name for shared memory
 * @param size size for shared memory
 */
syncdShmADT createSyncdShm(const char * name , size_t size);

/**
 * Destroys shared memory
 * @returns void
 * @param shmem shared memory to be destroyed
 */
void destroySyncdShm(syncdShmADT shmem);

/**
 * Writes in shared memory from buffer.
 * @returns integer with number of characters written
 * @param shmem shared memory to write into
 * @param buffer buffer to be read from
 * @param size size of buffer
 */
int writeSyncdShm(syncdShmADT shmem , const char * buffer , size_t size);

/**
 * Reads from shared memory into buffer.
 * @returns integer with number of characters read
 * @param shmem shared memory to write into
 * @param buffer buffer to be read from
 */
int readSyncdShm(syncdShmADT shmem , char * buffer );

/**
 * Opens an already created shared memory
 * @returns syncdShmADT, pointer to the opened structure with all data
 * @param name name of the shared memory to be opened
 * @param size size of the shared memory
 */
syncdShmADT openSyncdShm(const char * name , size_t size);

/**
 * Closes an already created shared memory
 * @returns void
 * @param name name of the shared memory to be closed
 */
void closeSyncdShm(syncdShmADT shmem);

#endif