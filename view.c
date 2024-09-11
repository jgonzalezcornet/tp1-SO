// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "syncdShmADT.h"
#include <stdlib.h>
#include <unistd.h>
#define STRING_MAX 128
#define BUFF_MAX 2048
#define FORMAT_MAX 20
int main(){
    char string[STRING_MAX]; 
    char format[FORMAT_MAX];
    sprintf(format , "%%%ds",BUFF_MAX - 1);
    scanf(format, string );
    syncdShmADT shmem = openSyncdShm(string,BUFF_MAX);
    if (shmem == NULL)
    {
        exit(EXIT_FAILURE);
    }
    readSyncdShm(shmem,string);
    printf("Processing %s file/s:\n", string);
    while(readSyncdShm(shmem,string)){
        puts(string);
    }
    destroySyncdShm(shmem);
    return 0;
}