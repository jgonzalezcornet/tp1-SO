#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

/*  TODO: todo lo que sigue quiero pasarlo a un file lib.h y 
    al final de este archivo esta todo lo que iria en su 
    lib.c correspondiente (los desarrollos de las funciones).
*/

/*  --------- checkParameters(int argc, char * argv[]) ---------
    This function is used to check if master program 
    (app.c's binary file) has been called with proper 
    command line arguments.
*/
void checkParameters(int argc, char * argv[]);

/* --------- processPath(char * path) ---------
    This function is used to explore command line arguments
    and open directories, so as to get a clear array of files
    that must be processed. That clear array is located in it's
    third parameter.
*/
void processPath(char * path);

void processDir(char * path);

void dispatchPath(char * path);

#define START_SLEEP_TIME    2
#define SLAVES          5       // define max so as to use min between #filesToProcess and max
#define BLOCK               5
#define MAX_PATH_LENGTH     260
#define MAX_ANSWER          

int quantSlaves = 0;        // each slave get assigned a number used for accessing it's pipe and pid

void createSlaves() {
    // TODO: crear slaves
}


int main(int argc, char * argv[]){
    pid_t pids[SLAVES]; 
    int pipesM2S[SLAVES][2];        // pipes used for sending "path" to slaves
    int pipesS2M[SLAVES][2];        // pipes used for receiving answer from slaves

    checkParameters(argc, argv);

    int file = open("result.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (file == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    for (int i=1; i < argc; i++){
        processPath(argv[i]);
    }

    //sleep(START_SLEEP_TIME);
    // TODO: si aparece el proceso vista compartirle el buffer de memoria compartida

    return 0;
}

/*  TODO: todo lo que sigue va en lib.c
*/

void checkParameters(int argc, char * argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error. Must send at least one file path.\n");
        exit(EXIT_FAILURE);
    }
}
// crear pipe para enviarle el path como stdin
// reemplazar el stdin del slave y cerrar su stdout
// crear otro pipe para recibir el resultado del slave 
// reemplazar el stdout del slave y cerrar su stdin
// mandarle el path a un slave

void dispatchPath(char * path) {
    if (quantSlaves == 0){
        createSlaves();
    } 

    quantSlaves++;
    printf("%s\n", path);   // TODO: borrar esto es solo para testear
}


// https://c-for-dummies.com/blog/?p=3246 
void processDir(char * path) {
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
        processPath(auxPath);
    }
}

void processPath(char * path) {
    struct stat fileStat;
    stat(path, &fileStat);
    if (S_ISDIR(fileStat.st_mode)) {
        processDir(path);
    }
    else {
        dispatchPath(path);
    }
}

/*
if (pipe(pipesM2S[quantSlaves][]) == -1) {          // create pipeM2S to send path as stdin for slave
        perror("Pipe error. Aborting.\n");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipesS2M[quantSlaves][]) == -1) {          // create pipeS2M to receive answer from slave
        perror("Pipe error. Aborting.\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {                                      // case: fork error
        perror("Fork error. Aborting.\n");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {                                         // case: child process
        close(pipesM2S[quantSlaves][1]);                    // close writing end of the pipeM2S -> slave will only receive through this one
        close(pipeS2M[quantSlaves][0]);                     // close receiving end of the pipeS2M -> slave will only write through this one
        if (dup2(pipeM2S[quantSlaves][0], STDIN_FILENO)) {  // redirect STDIN
            perror("Dup2 error. Aborting.\n");
            exit(EXIT_FAILURE);
        }
        if (dup2(pipeS2M[quantSlaves][1], STDOUT_FILENO)) { // redirect STDOUT
            perror("Dup2 error. Aborting.\n");
            exit(EXIT_FAILURE);
        }
        close(pipeM2S[quantSlaves][0]);                     // close original receiving end of pipeM2S
        close(pipeS2M[quantSlaves][1]);                     // close original writing end of pipeS2M
        char * argv[] = {"./slave", NULL};
        execve("./slave", argv, NULL);
        perror("Execve error. Aborting.\n");
        exit(EXIT_FAILURE);
    }
    else {                                                  // case: parent process
        close(pipesM2S[quantSlaves][0]);                    // close receiving end of the pipeM2S -> master will only write through this one
        write(pipeS2M[quantSlaves][1], "result.txt", strlen("result.txt")); // write stdout for child
        close(pipeS2M[quantSlaves][1]);                     // close writing end of the pipeS2M -> master will only receive through this one
        write(pipeM2S[quantSlaves][1], path, strlen(path));
        close(pipeM2S[quantSlaves][1]);                     // close pipe after writing path
        pids[quantSlaves] = pid;
        waitpid(pid, NULL, 0);
        close(pipeS2M[quantSlaves][0]);                     // close receiving nd
    }

*/