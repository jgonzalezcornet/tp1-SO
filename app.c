// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include "slaveADT.h"
#include "syncdShmADT.h"

#define NUM_MAX                 20
#define SHMEM_NAME_FORMAT       "/md5shmem-%d"
#define SHMEM_NAME_SIZE         11
#define MAX_PID_LEN             11 
#define SLAVES                  10
#define FILESPERSLAVE           4
#define BUFF_MAX                4096
#define SHMEM_SIZE              4096
#define SHMEM_SSIZE             "4096"

#define MIN(a,b) (a) < (b) ? a : b;

char * slavePath = "./slave";
int filesToProcess;
int filesWritten = 0;

void writeFilesToSlave(slaveADT slave , char * slaveFileCount , char * files[]){
    while(*slaveFileCount < FILESPERSLAVE && filesWritten < filesToProcess) {
        if(writeToSlave(slave , files[filesWritten + 1] , strlen(files[filesWritten + 1]) + 1) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        filesWritten++;  
        (*slaveFileCount)++;
    }
}

size_t countChar(char * s, size_t len, char c) {
    size_t count = 0;
    for(size_t i = 0; i < len; i++) {
        if(s[i] == c) count++; 
    }
    return count;
    
}

void checkParams(int argc) {
    if(argc < 2) {
        fprintf(stderr, "Error. Must send at least one file path.\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char * argv[]) {
    setvbuf(stdout,NULL,_IONBF,0);
    filesToProcess = argc - 1;
    int slaveCount = MIN(SLAVES , filesToProcess)
    
    checkParams(argc);

    char shname[SHMEM_NAME_SIZE + MAX_PID_LEN];
    sprintf(shname , SHMEM_NAME_FORMAT , getpid());
    syncdShmADT shmem = createSyncdShm(shname , SHMEM_SIZE);
    if(shmem == NULL) {
        exit(EXIT_FAILURE);
    }
    puts(shname);
    puts(SHMEM_SSIZE);
    sleep(1);

    char numFiles[NUM_MAX];
    sprintf(numFiles , "%d", filesToProcess);
    if(writeSyncdShm(shmem,numFiles, strlen(numFiles)) == -1 ) {
        perror("writeSyncdShm");
        exit(EXIT_FAILURE);
    }
    slaveADT slaves[SLAVES];
    char * slaveArgv[] = {"./slave", NULL};
    char full[slaveCount];

    for(size_t i = 0; i < slaveCount; i++) {
        full[i] = 0;
        slaves[i] = createSlave(slavePath , slaveArgv , NULL);
        writeFilesToSlave(slaves[i], full + i, argv);
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
                if(len < 0) {
                    perror("read");
                    exit(EXIT_FAILURE);
                }
                if(len > 0) {
                    buffer[len-1] = 0;
                    if(writeSyncdShm(shmem,buffer,len - 1) == -1) {
                        perror("writeSyncdShm");
                        exit(EXIT_FAILURE);
                    }
                    buffer[len-1] = '\n';
                    if(write(resultFd,buffer,len) == -1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                    }
                    int resultCount = countChar(buffer, len, '\n');
                    filesRead += resultCount;
                    full[i] -= resultCount;
                }
                writeFilesToSlave(slaves[i] , full + i , argv);
            } 
        }
    }
    
    close(resultFd);
    closeSlaves(slaves , slaveCount);
    writeSyncdShm(shmem,"",0);
    closeSyncdShm(shmem);
    
    return 0;
}