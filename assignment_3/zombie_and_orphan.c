#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define ORPHANED
// #define ZOMBIE

int main() {
    pid_t pid = fork();  // Create a new process

#ifdef ZOMBIE
    printf("Zombie process simulation\n");
    if (pid == 0) {
        // Child process: Sleep to become orphaned after parent exits
        printf("This is the child process: %d\n", getpid());
        printf("Child process exit immediately...\n");
        _exit(0);
    } else {
        printf("This is the parent process: %d\n", getpid());
        while (1);
    }
#else
    printf("Orphaned process simulation\n");
    if (pid == 0) {
        // Child process: Sleep to become orphaned after parent exits
        printf("This is the child process: %d\n", getpid());
        printf("Child process sleep for some seconds...\n");
        sleep(50);
    } else {
        printf("This is the parent process: %d\n", getpid());
        printf("Parent process exit immediately...\n");
    }
#endif
    return 0;
}
