#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include "slaveADT.h"

#define SLAVES              6
#define FILESPERSLAVE       2
#define BUFF_MAX            1024

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

    // Chequeo de parametros
    checkParams(argc);

    // Calculamos la cantidad de esclavos y la carga
    getSlaveLoad();


    slaveADT slaves[SLAVES];
    char * slaveArgv[] = {"./slave", NULL};

    int filesWritten = 0;
    for (size_t i = 0; i < SLAVES; i++)
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
    char full[SLAVES];
    for (size_t i = 0; i < SLAVES; i++)
    {
        full[i] = 1;
    }
    
    for (size_t i = 0; i < SLAVES; i++) full[i] = 1;
    while (filesRead < argc - 1)
    {
        anyReadable(slaves,SLAVES);
        for (size_t i = 0; i < SLAVES; i++)
        {
            if (isReadable(slaves[i]))
            {
                char buffer[BUFF_MAX];
                int len = readFromSlave(slaves[i] , buffer , BUFF_MAX);
                if (len < 0) {
                    perror("read");
                    exit(EXIT_FAILURE);
                }
                buffer[len-1] = 0;
                filesRead++;
                full[i] = 0;
                printf("%s\n", buffer);
            } 
            if (!full[i] && filesWritten < argc - 1)
            {
                writeToSlave(slaves[i] , argv[filesWritten + 1] , strlen(argv[filesWritten + 1]) );
                filesWritten++;
            }
        }
    }
    closeSlaves(slaves , SLAVES);
    return 0;
}