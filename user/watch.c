#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

/* Syscalls:
    int fork
    int exec
    int wait
    int pause*/
void watch(char *cmd[], int sleep) {
    int pid;
    int ret;
    while(1) {
        pid = fork();
        if (pid == 0) {
            if(exec(cmd[0], cmd) < 0) exit(1);
        }
        pid = wait(&ret);
        if (!ret) {
            fprintf(2, "%s failed\n", cmd[0]);
            exit(1);
        }
        pause(sleep);
    }
}
        
int main(int argc, char *argv[]) {
    if(argc < 2) exit(1);
    if(strcmp(argv[1], "-n") == 0) {
        watch(&argv[3], 20 * atoi(argv[2]));
    }
    else watch(&argv[1], 20);    
    exit(0);
}