#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>

#define SLAVES              5
#define FILESPERSLAVE       2
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))

int copyString(const char * str, char * buffer) {
    int len;
    for (len = 0; *(str + len) != 0; len++) {
        *(buffer + len) = *(str + len);
    }
    *(buffer + len) = '\n';
    *(buffer + len + 1) = 0;
    return len + 1;
}

int main(int argc, char * argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error. Must send at least one file path.\n");
        exit(EXIT_FAILURE);
    }

    int masterToSlave[SLAVES];          // pipes used to communicate from master to slaves
    int slaveToMaster[SLAVES];          // pipes used to communicate from slaves to master
    pid_t pids[SLAVES];

    char * slaveArgv[] = {"./slave", NULL};
    char * envp[] = {NULL};

    int file = open("result.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (file < 0) {
        perror("Open error.");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < SLAVES; i++) {       // create slaves and their corresponding pipes
        int m2s[2], s2m[2];
        if ((pipe(m2s) == -1) | (pipe(s2m) == -1)) {
            perror("Pipes error.");
            exit(EXIT_FAILURE);
        }

        pid_t cpid = fork();
        if (cpid < 0) {
            perror("Fork error.");
            exit(EXIT_FAILURE);
        }
        if (cpid == 0) {                                // child (slave) process
            
            close(m2s[1]);
            if (dup2(m2s[0], STDIN_FILENO) == -1) {
                perror("Dup error.");
                exit(EXIT_FAILURE);
            }
            close(m2s[0]);

            close(s2m[0]);
            if (dup2(s2m[1], STDOUT_FILENO) == -1) {
                perror("Dup error.");
                exit(EXIT_FAILURE);
            }
            close(s2m[1]);

            execve("./slave", slaveArgv, envp);
            perror("Execve error.");
            exit(EXIT_FAILURE);
        }
        pids[i] = cpid;                                // parent (master) process
        close(m2s[0]);
        close(s2m[1]);
        
        masterToSlave[i] = m2s[1];
        slaveToMaster[i] = s2m[0];
    }


    for (int pathNum = 1; pathNum < argc; pathNum = pathNum + FILESPERSLAVE) {
        fd_set readFDS;
        fd_set writeFDS;
        FD_ZERO(&readFDS);
        FD_ZERO(&writeFDS);

        int maxFD = 0;
        for (int i = 0; i < SLAVES; i++) {
            if (masterToSlave[i] > maxFD) {      // TODO: ver si es redundante el 1 o no
                maxFD = masterToSlave[i];
            }
            if (slaveToMaster[i] > maxFD) {      // TODO: ver si es redundante el 0 o no
                maxFD = slaveToMaster[i];
            }
        }

        for (int i = 0; i < SLAVES; i++) {
            FD_SET(slaveToMaster[i], &readFDS);  // TODO: ver si es redundante el 0 o no
            FD_SET(masterToSlave[i], &writeFDS); // TODO: ver si es redundante el 1 o no
        }

        int available = select(maxFD + 1, &readFDS, &writeFDS, NULL, NULL);
        if (available < 0){
            perror("Select error");
            exit(EXIT_FAILURE);
        }
        
        // available > 0
        for (int i = 0; i < SLAVES; i++) {
            if (FD_ISSET(masterToSlave[i], &writeFDS)) { 
                for (int j = 0; (j < FILESPERSLAVE - 1) && (pathNum + j < argc); j++) {
                    char buffer[1024];
                    int len = copyString(argv[pathNum + j], buffer);
                    write(masterToSlave[i], buffer, len);
                }
            }
            if (FD_ISSET(slaveToMaster[i], &readFDS)) {
                char buffer[1024];
                int len = read(slaveToMaster[i], buffer, sizeof(buffer));
                if (len < 0) {
                    perror("Read error.");
                    exit(EXIT_FAILURE);
                }
                buffer[len-1] = 0;
                printf("%s processed by %d\n", buffer, pids[i]);
            }
        }
    }

    // TODO: VER SI HACE FALTA
    for (int i = 0; i < SLAVES; i++) {
        close(masterToSlave[i]);
        close(slaveToMaster[i]);
        if (kill(pids[i], SIGTERM) == -1) {
            perror("Kill error.");
        }
    }
}