#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>

#define START_SLEEP_TIME    2
#define SLAVES              5
#define MAX_PATH_LENGTH     1024

void getSlaveAnswer(int pipesS2M[][2]);
int dispatchPath(int pipesM2S[][2]);
void processPath(char * path, int pipesM2S[][2], int pipesS2M[][2], pid_t * pids);
void processDir(char * path, int pipesM2S[][2], int pipesS2M[][2], pid_t * pids);
pid_t * createSlaves(int quant, int pipesMasterToSlave[][2], int pipesSlaveToMaster[][2], pid_t * pids);

int main(int argc, char * argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error. Must send at least one file path.\n");
        exit(EXIT_FAILURE);
    }

    int pipesMasterToSlave[SLAVES][2];          // [slave 0] -> [read][write]
                                                // [slave 1] -> [read][write]
                                                // [slave 2] -> [read][write]
                                                // [slave 3] -> [read][write]
                                                // [slave 4] -> [read][write]

    int pipesSlaveToMaster[SLAVES][2];          // [slave 0] -> [read][write]
                                                // [slave 1] -> [read][write]
                                                // [slave 2] -> [read][write]
                                                // [slave 3] -> [read][write]
                                                // [slave 4] -> [read][write]

    // create pipes
    for (int slaveID=0; slaveID < SLAVES; slaveID++) {
        if (pipe(pipesMasterToSlave[slaveID]) == -1 || pipe(pipesSlaveToMaster[slaveID]) == -1) {
            perror("Pipe error. Aborting.\n");
            exit(EXIT_FAILURE);
        }
        printf("created pipes for slave %d\n", slaveID);
    }

    // create slaves
    pid_t pids[SLAVES];
    for (int slaveID=0; slaveID < SLAVES; slaveID++) {
        pid_t currPid = fork();
        if (currPid == -1) {
            perror("Forking error. Aborting.\n");
            exit(EXIT_FAILURE);
        }
        if (currPid == 0) {
            // slave (child) process
            close(pipesMasterToSlave[slaveID][1]);
            dup2(pipesMasterToSlave[slaveID][0], STDIN_FILENO);            // TODO: falta verificar que el dup no haya dado error (if == -1)
            close(pipesMasterToSlave[slaveID][0]);

            close(pipesSlaveToMaster[slaveID][0]);
            dup2(pipesSlaveToMaster[slaveID][1], STDOUT_FILENO);            // TODO: falta verificar que el dup no haya dado error (if == -1)
            close(pipesSlaveToMaster[slaveID][1]);

            char * argv[] = {"./slave", NULL};
            char * envp[] = {NULL};
            execve("./slave", argv, envp);
            perror("Execve error. Aborting.\n");
            exit(EXIT_FAILURE);
        }
        else {
            close(pipesMasterToSlave[slaveID][0]);
            close(pipesSlaveToMaster[slaveID][1]);
            pids[slaveID] = currPid;
            printf("created slave %d with pid %d\n", slaveID, currPid);        // TODO: borrar
        }
    }

    // process paths (command line arguments)
    for (int i = 1; i < argc; i++) {
        printf("path: %s\n", argv[i]);
        processPath(argv[i], pipesMasterToSlave, pipesSlaveToMaster, pids);
    }
}

void processPath(char * path, int pipesM2S[][2], int pipesS2M[][2], pid_t * pids) {
    struct stat fileStat;
    stat(path, &fileStat);
    if (S_ISDIR(fileStat.st_mode)) {
        processDir(path, pipesM2S, pipesS2M, pids);
    }
    else {
        printf("path 2 %s\n", path);
        int slaveID = dispatchPath(pipesM2S);
        printf("%d\n", slaveID);
        if (write(pipesM2S[slaveID][1], path, strlen(path) + 1) == -1) {
            perror("Writing error.\n");
            exit(EXIT_FAILURE);
        }
        printf("ya escribio\n");
        getSlaveAnswer(pipesS2M);
        return;
    }
}

// https://c-for-dummies.com/blog/?p=3246 
void processDir(char * path, int pipesM2S[][2], int pipesS2M[][2], pid_t * pids) {
    DIR * dir = opendir(path);   // open said directory
        if (dir == NULL) {
            perror("No se pudo abrir el directorio");
            exit(EXIT_FAILURE);
        }
    struct dirent * dirEntry;
    while ((dirEntry = readdir(dir)) != NULL) {
        if (strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0) {
            continue;      // ignore this file (. and ..) and go the the next iteration
        }
        char auxPath[MAX_PATH_LENGTH];
        snprintf(auxPath, sizeof(auxPath), "%s/%s", path, dirEntry->d_name);
        processPath(auxPath, pipesM2S, pipesS2M, pids);
    }
    closedir(dir);
}

int dispatchPath(int pipesM2S[][2]) { 
    // fd_set readFDS;
    // FD_ZERO(&readFDS);
    fd_set writeFDS;
    FD_ZERO(&writeFDS);
    
    int maxFD = 0;
    for (int i=0; i < SLAVES; i++) {
        // FD_SET(pipesS2M[i][0], &readFDS);
        FD_SET(pipesM2S[i][1], &writeFDS);
        if (/*pipesS2M[i][0] > maxFD ||*/ pipesM2S[i][1] > maxFD) {
            maxFD = pipesM2S[i][1];
        }
    }
    int status = select(maxFD + 1, /*&readFDS*/ NULL, &writeFDS, NULL, NULL);
    printf("%d\n", status);
    //if (status < 0) {
    //    perror("Select error.\n");        // TODO: descomentar
    //    exit(EXIT_FAILURE);
    //} 
    //else {
        printf("status > 0\n");
        for (int slaveID=0; slaveID < SLAVES; slaveID++) {
            if (FD_ISSET(pipesM2S[slaveID][1], &writeFDS)) {
                return slaveID;
                // if (FD_ISSET(pipesS2M[slaveID][0], &readFDS)) {      // TODO: borrar
                //     char slaveAns[1024];
                //     int len = read(pipesS2M[slaveID][0], slaveAns, sizeof(slaveAns));
                //     printf("Readed %d bytes", len);
                //     printf("Slave ans is %s", slaveAns);
                //     printf("Slave's pid is %d", pids[slaveID]);
                //     return slaveAns;
                // }
            }
            // break;
        }
    }
//}

void getSlaveAnswer(int pipesS2M[][2]) {
    fd_set readFDS;
    FD_ZERO(&readFDS);
    int maxFD = 0;
    for (int i=0; i < SLAVES; i++) {
        FD_SET(pipesS2M[i][0], &readFDS);
        if (pipesS2M[i][0] > maxFD /*|| pipesM2S[i][1] > maxFD*/) {
            maxFD = pipesS2M[i][0];
        }
    }
    int status = select(maxFD + 1, &readFDS, /*&writeFDS*/ NULL, NULL, NULL);

    if (status == -1) {
        perror("Select error. Aborting.\n");
        exit(EXIT_FAILURE);
    }
    else if (status > 0) {
        printf("Status is greater than zero\n");
        for (int slaveID=0; slaveID < SLAVES; slaveID++) {
            if (FD_ISSET(pipesS2M[slaveID][0], &readFDS)) {
                char buffer[1024];
                int len = read(pipesS2M[slaveID][0], buffer, sizeof(buffer));
                if (len == -1) {
                    perror("Reading error.\n");
                    exit(EXIT_FAILURE);
                }
                buffer[len] = '\0';
                printf("%s\n", buffer);
                // if (FD_ISSET(pipesS2M[slaveID][0], &readFDS)) {
                //     char slaveAns[1024];
                //     int len = read(pipesS2M[slaveID][0], slaveAns, sizeof(slaveAns));
                //     printf("Readed %d bytes", len);
                //     printf("Slave ans is %s", slaveAns);
                //     printf("Slave's pid is %d", pids[slaveID]);
                //     return slaveAns;
                // }
            }
            // break;
        }
    }
}