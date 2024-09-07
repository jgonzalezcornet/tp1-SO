#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "slaveADT.h"

#define BUFFMAX 100
#define BLOCK 5
#define MAX_PATH 200
#define PIPE_R 0
#define PIPE_W 1
#define MAX_PID 5
#define MD5_LEN 32
#define MD5_PATH "/bin/md5sum"
#define OUTPUT_FORMAT "%s - %s - processed by %d\n"
#define FORMAT_LENGHT 21

int main(int argc, char * argv[]) {
    char file[MAX_PATH] = {0};

    char * paths[] = {MD5_PATH, file, NULL};
    while(scanf("%s", file) != EOF) {
        slaveADT slaveMd5 = createSlave( MD5_PATH, paths , NULL);
        if (slaveMd5 == NULL)
        {
            perror("createSlave");
            exit(EXIT_FAILURE);
        }
        
        char md5[MD5_LEN] = {0};
        int md5Len = readFromSlave(slaveMd5 , md5 , MD5_LEN );
        if (md5Len == -1)
        {
            perror("readFromSlave");
            exit(EXIT_FAILURE);
        }
        md5[md5Len] = 0;

        int pid = getpid();
        char result[MAX_PATH + md5Len + MAX_PID + FORMAT_LENGHT];                     
        sprintf(result, OUTPUT_FORMAT , file, md5, pid);
        write(STDOUT_FILENO, result, strlen(result));
        closeSlaves(&slaveMd5,1);
    }
    return 0;
}