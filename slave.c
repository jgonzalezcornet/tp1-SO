#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/select.h>

#define BUFFMAX 100
#define BLOCK 5
#define MAX_PATH 200
#define PIPE_R 0
#define PIPE_W 1
#define MAX_PID 5
#define MD5_LEN 32

int createPipe(int fds[]){
    if(pipe(fds) < 0){
        perror("pipe");
        return -1;
    }
    return 0;
}

int main() {
   
    char file[MAX_PATH] = {0};

    while(scanf("%s", file) != EOF) {

        // Creamos un pipe y redirigimos la escritura de md5 a el.
        int p[2];
        if(createPipe(p) == -1)
            break;

        if (fork() == 0) {
            dup2(p[PIPE_W], STDOUT_FILENO);
            close(p[PIPE_W]);
            close(p[PIPE_R]);
            
            char * paths[] = {"/bin/md5sum", file, NULL};
            execve("/bin/md5sum", paths , NULL);
            perror("execve\n");
            exit(EXIT_FAILURE);
        }

        char md5[MD5_LEN];
        ssize_t md5Len = read(p[PIPE_R], md5, MD5_LEN);
        md5[md5Len] = 0;

        //Modularizar
        int pid = getpid();
        char result[MAX_PATH + md5Len + MAX_PID + 21];                      //21 por el siguiente texto
        sprintf(result, "%s: %s - processed by %d\n", file, md5, pid);

        write(STDOUT_FILENO, result, strlen(result));
    }

    return 0;
}
