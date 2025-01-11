#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

// Signal handler function
void signal_handler(int sig) {
    switch (sig) {
        case SIGINT:
            printf("Caught SIGINT (Ctrl+C)\n");
            break;
        case SIGTERM:
            printf("Caught SIGTERM (Termination signal)\n");
            break;
        default:
            printf("Caught signal %d\n", sig);
    }
}

int main() {
    // Register signal handlers
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("Unable to register SIGINT handler");
        exit(1);
    }

    if (signal(SIGTERM, signal_handler) == SIG_ERR) {
        perror("Unable to register SIGTERM handler");
        exit(1);
    }

    if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
        perror("Unable to register SIGUSR1 handler");
        exit(1);
    }

    printf("Program running with ID: %d \n", getpid());
    printf("Send signals to test.\n");
    while (1) {
        sleep(1); // Wait for signals
    }

    return 0;
}
