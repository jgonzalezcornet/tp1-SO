#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFMAX 100
#define BLOCK 5

int main(){
    char buffer[BUFFMAX];
    char ** paths = malloc(sizeof(char *) * BLOCK + 1);
    paths[0] = "/bin/md5sum";
    int n;
    while ((n = read(STDIN_FILENO , buffer , BUFFMAX )))
    {
        if (n < 0)
        {
            perror("");
            exit(1);
        }
        if (n == 1)
        {
            continue;
        }
        buffer[n-1] = 0;
        int count = 0;
        char * token = strtok(buffer , " ");
        while (token)
        {
            if( ((count+1) % BLOCK) == 0 ) paths = realloc(paths,count+BLOCK+1);
            paths[1+count] = token;
            count++;
            token = strtok(NULL," ");
        }
        paths[count+1] = NULL;
        /* int p[2];
        pipe(p); */
        if (fork() == 0)
        {
            /* close(STDOUT_FILENO);
            dup(p[1]);
            close(p[1]);
            close(p[0]); */
            execve("/bin/md5sum", paths , NULL);
        }
        /* int stdin = dup(STDIN_FILENO);
        close(STDIN_FILENO);
        int aux = dup(p[0]);
        close(p[1]);
        close(p[0]);
        char rta[1000];
        for (size_t i = 0; i < count; i++)
        {
            int len = read(0,rta,1000);
            rta[len-1] = 0;
            puts(rta);
        }
        close(aux);
        dup(stdin);
        close(stdin); */
    }
    return 0;
}