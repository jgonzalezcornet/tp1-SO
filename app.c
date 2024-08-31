#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>

#define SLAVES              5
#define FILESPERSLAVE       2
#define BUFF_MAX            1024

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

int sendFile(char * file, int fileCount, int fd){
    if(write(fd, file, strlen(file) + 1) == -1 || write(fd, "\n", 1) == -1) {
        return -1;
    }
    return 0;
}

int readFile(int fd){
    char buffer[BUFF_MAX];
    int len = read(fd, buffer, sizeof(buffer));
    if (len < 0) {
        return -1;
    }
    buffer[len-1] = 0;
    printf("%s\n", buffer);
    return 0;
}

int main(int argc, char * argv[]) {
    // Chequeo de parametros
    checkParams(argc);

    // Calculamos la cantidad de esclavos y la carga
    getSlaveLoad();

    // Creamos los pipes de comunicacion entre master y slaves.
    int masterToSlave[SLAVES][2];          // pipes used to communicate from master to slaves
    int slaveToMaster[SLAVES][2];          // pipes used to communicate from slaves to master
    pid_t slavePids[SLAVES];               // TODO: nunca usamos este arreglo me parece que no hace falta guardarlos
    int maxFD = -1;

    //Abrimos el archivo de resultado
    int resultFile = open("result.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (resultFile < 0) {
        perror("Open error.");
        exit(EXIT_FAILURE);
    }

    if (dup2(resultFile, STDOUT_FILENO) == -1) {
        perror("Dup error.");
        exit(EXIT_FAILURE);
    }

    // Creamos los procesos esclavo junto con sus pipes
    char * slaveArgv[] = {"./slave", NULL};
    char * envp[] = {NULL};

    int filesWritten = 0;

    for (int i = 0; i < SLAVES; i++) {
        if ((pipe(masterToSlave[i]) == -1) || (pipe(slaveToMaster[i]) == -1)) {
            perror("Pipes error.");
            exit(EXIT_FAILURE);
        }

        pid_t cpid = fork();
        if (cpid < 0) {
            perror("Fork error.");
            exit(EXIT_FAILURE);
        }

        if (cpid == 0) {                               
            //Child process
            close(slaveToMaster[i][0]);

            if (dup2(masterToSlave[i][0], STDIN_FILENO) == -1) {
                perror("Dup error.");
                exit(EXIT_FAILURE);
            }
            close(masterToSlave[i][0]);

            if (dup2(slaveToMaster[i][1], STDOUT_FILENO) == -1) {
                perror("Dup error.");
                exit(EXIT_FAILURE);
            }
            close(slaveToMaster[i][1]);

            execve("./slave", slaveArgv, envp);
            perror("Execve error.");
            exit(EXIT_FAILURE);
        }
        // Parent process
        slavePids[i] = cpid;
        close(masterToSlave[i][0]);
        close(slaveToMaster[i][1]);
        maxFD = slaveToMaster[i][0] > maxFD ? slaveToMaster[i][0] : maxFD;

        // Enviamos los primeros archivos a los esclavos
        for(int k = 0; k < FILESPERSLAVE && filesWritten < argc - 1; k++) {
            if(sendFile(argv[filesWritten + 1], 1, masterToSlave[i][1]) == -1) {
                perror("Write error.");
                exit(EXIT_FAILURE);
            }
            filesWritten++;
        }
    }

    int filesRead = 0;
    char full[SLAVES] = {0};
    fd_set readFDS;
    FD_ZERO(&readFDS);
    do {
        for (int i = 0; i < SLAVES ; i++) {
            if (FD_ISSET(slaveToMaster[i][0], &readFDS)) { 
                if(readFile(slaveToMaster[i][0]) == -1) {
                    perror("Read error.");
                    exit(EXIT_FAILURE);
                }
                filesRead++;
                full[i] = 0;
            }
            if(!full[i] && filesWritten < argc - 1){
                if(sendFile(argv[filesWritten + 1], 1, masterToSlave[i][1]) == -1) {
                    perror("Write error.");
                    exit(1);
                }
                filesWritten++;
                full[i] = 1;               
            }
        }

        FD_ZERO(&readFDS);
        for (int i = 0; i < SLAVES; i++) {
            FD_SET(slaveToMaster[i][0], &readFDS);  // TODO: ver si es redundante el 0 o no
        }

    } while(filesRead < (argc - 1) && select(maxFD + 1, &readFDS, NULL, NULL, NULL));
    
    // Cerramos los pipes para matar los procesos esclavos
    for (int i = 0; i < SLAVES; i++) {
        close(masterToSlave[i][1]);
        close(slaveToMaster[i][0]);
    }

    close(resultFile);

    return 0;
}