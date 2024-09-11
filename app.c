// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include "slaveADT.h"
#include "syncdShmADT.h"

#define NUM_MAX 20
#define SHMEM_NAME "/md5shmem"
#define SLAVES              6
#define FILESPERSLAVE       2
#define BUFF_MAX            4096
#define MIN(a,b) (a) < (b) ? a : b;
char * slavePath = "./slave";

void checkParams(int argc){
    if(argc < 2) {
        fprintf(stderr, "Error. Must send at least one file path.\n");
        exit(EXIT_FAILURE);
    }
}

void getSlaveLoad(){
    // TODO: develop
    return;
}

int main(int argc, char * argv[]) {
    setvbuf(stdout,NULL,_IONBF,0);
    int slaveCount = MIN(SLAVES , argc - 1)
    int fileCount = argc - 1; //@TODO: reemplazar
    // Chequeo de parametros
    checkParams(argc);

    //@TODO: Calculamos la cantidad de esclavos y la carga
    getSlaveLoad();

    syncdShmADT shmem = createSyncdShm(SHMEM_NAME , BUFF_MAX);
    if (shmem == NULL)
    {
        exit(EXIT_FAILURE);
    }
    puts(SHMEM_NAME);
    sleep(5);
    char numFiles[NUM_MAX];
    sprintf(numFiles , "%d", fileCount);
    writeSyncdShm(shmem,numFiles, strlen(numFiles));
    slaveADT slaves[SLAVES];
    char * slaveArgv[] = {"./slave", NULL};

    int filesWritten = 0;
    for (size_t i = 0; i < slaveCount; i++)
    {
        slaves[i] = createSlave(slavePath , slaveArgv , NULL);
         // Enviamos los primeros archivos a los esclavos
        for(int k = 0; k < FILESPERSLAVE && filesWritten < argc - 1; k++) {
            if ( writeToSlave(slaves[i] , argv[filesWritten + 1] , strlen(argv[filesWritten + 1]) + 1 )  == -1){
                perror("write");
                exit(EXIT_FAILURE);
            }
            filesWritten++;
        }
    }
    int filesRead = 0;
    char full[slaveCount];
    for (size_t i = 0; i < slaveCount; i++)
    {
        full[i] = 1;
    }
    
    int resultFd = open("result.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (resultFd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < slaveCount; i++) full[i] = 1;
    while (filesRead < argc - 1)
    {
        anyReadable(slaves,slaveCount);
        for (size_t i = 0; i < slaveCount; i++)
        {
            if (isReadable(slaves[i]))
            {
                char buffer[BUFF_MAX];
                int len = readFromSlave(slaves[i] , buffer , BUFF_MAX);
                if (len < 0) {
                    perror("read");
                    exit(EXIT_FAILURE);
                }
                if( len > 0 ){
                    buffer[len-1] = 0;
                    char * result = strtok(buffer,"\n");
                    if (result != NULL)
                    {    
                        do
                        {
                            int aux = strlen(result);
                            writeSyncdShm(shmem,result,aux);
                            if( write(resultFd,result,aux) == -1 || write(resultFd,"\n",1) == -1 ){
                                perror("write");
                                exit(EXIT_FAILURE);
                            }
                            filesRead++;
                        } while ( (result = strtok(NULL ,"\n")) != NULL);
                    }
                }
                full[i] = 0;
            } 
            if (!full[i] && filesWritten < argc - 1)
            {
                writeToSlave(slaves[i] , argv[filesWritten + 1] , strlen(argv[filesWritten + 1]) );
                filesWritten++;
            }
        }

    }
    close(resultFd);
    closeSlaves(slaves , slaveCount);
    writeSyncdShm(shmem,"",0);
    destroySyncdShm(shmem);
    return 0;
}