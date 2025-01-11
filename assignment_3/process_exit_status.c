#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/* 
Process termination status (int) 16 bit number
1. status value if  child process has normal exit/termination
   15......8    |     7......0
   XXXXXXXX     |     XXXXXXXX 
   exitStaus    |     0


2. killed by signal
    15......8    |     7   ......0
    unused       |     X   termination signal
                       |
                       |-----> core dump flag

*/
int main() {

    printf("Before fork: %d\n", getpid());

    pid_t pid = fork();
    __uint16_t exitStat = 0;

    if (pid == -1) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        printf("This is the child process: %d\n", getpid());
        exit(5);
    } else {
        // Parent process
        printf("This is the parent process: %d\n", getpid());
        waitpid(pid, (int *)&exitStat, 0);
        if(WIFEXITED(exitStat) && (WEXITSTATUS(exitStat) == (exitStat >> 8))){
            printf("Child process exited - ID: %d\n", pid);
            printf("Child process exit code: %d\n", (exitStat >> 8));
        } else {
            perror("Process exit failed");
        }
    }
    return 0;
}
