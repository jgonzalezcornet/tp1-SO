#include "slaveADT.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define PIPE_WRITE 1
#define PIPE_READ 0
#define BLOCK 5

struct slaveCDT{
    int pid;
    int sstdin;
    int sstdout;
};

fd_set read_fds;
int aux2[2] = {0};
slaveADT createSlave(const char * path , char * argv[] , char * envp[] ){
    slaveADT slave = malloc(sizeof(struct slaveCDT));
    if (slave == NULL) {
        return NULL;
    }

    int m2s[2] ,s2m[2];
    if (pipe(m2s) == -1 || pipe(s2m) == -1) {
        free(slave);
        return NULL;
    }
    if ( aux2[0] == 0 ){
        aux2[0] = m2s[0];
        aux2[1] = m2s[1];
    }
    int pid = fork();
    if (pid < 0) {
        // Fork failed
        close(m2s[PIPE_READ]);
        close(m2s[PIPE_WRITE]);
        close(s2m[PIPE_READ]);
        close(s2m[PIPE_WRITE]);
        free(slave);
        return NULL;
    } 
    if (pid == 0) {
         // Child process
        close(m2s[PIPE_WRITE]); 
        close(s2m[PIPE_READ]); 

        dup2(m2s[PIPE_READ], STDIN_FILENO);
        close(m2s[PIPE_READ]);

        dup2(s2m[PIPE_WRITE], STDOUT_FILENO);
        close(s2m[PIPE_WRITE]);

        execve(path, argv, envp);
        perror("execve");
        exit(EXIT_FAILURE);
    }

    // Parent process
    close(m2s[PIPE_READ]);
    close(s2m[PIPE_WRITE]);

    slave->pid = pid;
    slave->sstdin = m2s[PIPE_WRITE];
    slave->sstdout = s2m[PIPE_READ];

    return slave;

}

int anyReadable(slaveADT * slaves , size_t count){
    if (count == 0 || slaves == NULL){
        return 0;
    }
    FD_ZERO(&read_fds);

    int maxStdoutFd = -1;

    for (size_t i = 0; i < count; i++) {
        FD_SET(slaves[i]->sstdout, &read_fds);
        if (slaves[i]->sstdout > maxStdoutFd) {
            maxStdoutFd = slaves[i]->sstdout;
        }
    }

    int ret = select(maxStdoutFd + 1, &read_fds, NULL, NULL, NULL);

    if (ret < 0) {
        perror("select");
        return -1;
    }

    return ret;
}

int isReadable(slaveADT slave) {
    if (slave == NULL) {
        return 0;
    }
    return FD_ISSET(slave->sstdout,&read_fds);
}

int readFromSlave(slaveADT slave, char *buff, size_t buffSize) {
    if (slave == NULL) {
        return -1;
    }

    int count = read(slave->sstdout, buff, buffSize);
    if (count < 0) {
        return -1;
    }

    FD_CLR(slave->sstdout,&read_fds);
    return count;
}

int writeToSlave(slaveADT slave, char *buff, size_t buffSize) {
    if (slave == NULL) {
        return -1;
    }
    int count = write(slave->sstdin , buff, buffSize);
    if (count < 0) {
        return -1;
    }
    return count;
}

void closeSlaves(slaveADT * slaves , size_t count){
    for (size_t i = 0; i < count; i++)
    {
        close(slaves[i]->sstdin);
        close(slaves[i]->sstdout);
        free(slaves[i]);
    }
    
}