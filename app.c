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
#define SHMEM_NAME_FORMAT "/md5shmem-%d"
#define SHMEM_NAME_SIZE 11
#define MAX_PID_LEN 11 
#define SLAVES              5
#define FILESPERSLAVE       2
#define BUFF_MAX            4096
#define MIN(a,b) (a) < (b) ? a : b;
char * slavePath = "./slave";

size_t countChar(char * s, size_t len, char c){
    size_t count = 0;
    for(size_t i = 0; i < len; i++) {
        if(s[i] == c) count++; 
    }
    return count;
    
}
void checkParams(int argc){
    if(argc < 2) {
        fprintf(stderr, "Error. Must send at least one file path.\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char * argv[]) {
    setvbuf(stdout,NULL,_IONBF,0);
    int filesToProcess = argc - 1;
    int slaveCount = MIN(SLAVES , filesToProcess)
    
    checkParams(argc);

    char shname[SHMEM_NAME_SIZE + MAX_PID_LEN];
    sprintf(shname , SHMEM_NAME_FORMAT , getpid());
    syncdShmADT shmem = createSyncdShm(shname , BUFF_MAX);
    if (shmem == NULL) {
        exit(EXIT_FAILURE);
    }
    puts(shname);

    sleep(10);

    char numFiles[NUM_MAX];
    sprintf(numFiles , "%d", filesToProcess);
    if( writeSyncdShm(shmem,numFiles, strlen(numFiles)) == -1 ) {
        perror("writeSyncdShm");
        exit(EXIT_FAILURE);
    }
    slaveADT slaves[SLAVES];
    char * slaveArgv[] = {"./slave", NULL};
    char full[slaveCount];

    int filesWritten = 0;
    for(size_t i = 0; i < slaveCount; i++) {
        slaves[i] = createSlave(slavePath , slaveArgv , NULL);
        for(int k = 0; k < FILESPERSLAVE && filesWritten < filesToProcess; k++) {
            if(writeToSlave(slaves[i] , argv[filesWritten + 1] , strlen(argv[filesWritten + 1]) + 1)  == -1){
                perror("write");
                exit(EXIT_FAILURE);
            }
            filesWritten++;  
        }
        full[i] = 1;
    }

    int filesRead = 0;
    
    int resultFd = open("result.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if(resultFd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    while(filesRead < filesToProcess) {
        anyReadable(slaves,slaveCount);
        for(size_t i = 0; i < slaveCount; i++) {
            if(isReadable(slaves[i])) {
                char buffer[BUFF_MAX];
                int len = readFromSlave(slaves[i] , buffer , BUFF_MAX);
                if (len < 0) {
                    perror("read");
                    exit(EXIT_FAILURE);
                }
                if(len > 0) {
                    buffer[len-1] = 0;
                    if(writeSyncdShm(shmem,buffer,len) == -1) {
                        perror("writeSyncdShm");
                        exit(EXIT_FAILURE);
                    }
                    buffer[len-1] = '\n';
                    if(write(resultFd,buffer,len) == -1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                    }
                    filesRead += countChar(buffer, len, '\n');
                    full[i] = 0;
                }
            } 
            if(!full[i] && filesWritten < filesToProcess) {
                filesWritten++;
                if(writeToSlave(slaves[i] , argv[filesWritten], strlen(argv[filesWritten])) == -1) {
                    perror("writeToSlave");
                    exit(EXIT_FAILURE);
                }
                full[i] = 1;
            }
        }
    }

    close(resultFd);
    closeSlaves(slaves , slaveCount);
    writeSyncdShm(shmem,"",0);
    closeSyncdShm(shmem);
    
    return 0;
}