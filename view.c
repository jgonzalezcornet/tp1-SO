// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "syncdShmADT.h"
#include <stdlib.h>
#include <unistd.h>

#define STRING_MAX 1024
#define BUFF_MAX 2048
#define FORMAT_MAX 20
#define SHNAME_MAX 128

int main() {
    char shName[SHNAME_MAX]; 
    char format[FORMAT_MAX];

    // For PVS warning - stack overflow prevention
    sprintf(format , "%%%ds",BUFF_MAX - 1);
    scanf(format, shName);
    syncdShmADT shmem = openSyncdShm(shName,BUFF_MAX);
    if (shmem == NULL) {
        exit(EXIT_FAILURE);
    }
    char buffer[STRING_MAX];
    readSyncdShm(shmem,buffer);
    printf("Processing %s file/s:\n", buffer);
    while(readSyncdShm(shmem,buffer)) {
        puts(buffer);
    }
    destroySyncdShm(shmem);
    
    return 0;
}