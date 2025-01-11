#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    printf("Before fork: %d\n", getpid());

    pid_t pid = fork();

    if (pid == -1) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        printf("This is the child process: %d\n", getpid());
    } else {
        // Parent process
        printf("This is the parent process: %d\n", getpid());
        wait(NULL);  // Wait for the child process to finish
    }
    return 0;
}
