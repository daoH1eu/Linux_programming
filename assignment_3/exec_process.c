#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>  

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    printf("Before fork: %d\n", getpid());

    pid_t pid = fork();

    if (pid == -1) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        printf("This is the child process: %d\n", getpid());

        if (atoi(argv[1]) == 1){
            printf("Child process info:\n");
            char pid_str[20];
            sprintf(pid_str, "%d", getpid());
            // Replace child process with new program
            execlp("ps", "ps", "-p", pid_str, "-f", NULL);
        } else if (atoi(argv[1])== 2){
            printf("List of files:\n");
            // Replace child process with new program
            execlp("ls", "ls", "-l", NULL);
        }
    } else {
        // Parent process
        printf("This is the parent process: %d\n", getpid());
        wait(NULL);  // Wait for the child process to finish
    }

    return 0;
}
