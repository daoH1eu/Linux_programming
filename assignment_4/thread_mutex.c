#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


int count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to be executed by each thread
void * thread_count_up(void *arg) {
    pthread_mutex_lock(&mutex);  // Lock the mutex

    printf("Thread %ld start counting up with mutex.\n", pthread_self());
    for (int i = 0; i < 1000000; i++)
        count += 1;
    
    pthread_mutex_unlock(&mutex);  // Unlock the mutex
    pthread_exit(NULL);  
}

int main() {
    __uint8_t num_threads = 3;
    pthread_t thread[num_threads]; // Array to hold thread identifiers

    printf("Before couting: %d\n", count);

    for (int i = 0; i < num_threads; i++){
        // Create a new thread and run
        if (pthread_create(&thread[i], NULL, thread_count_up, NULL) != 0) {
            perror("Failed to create thread");
            exit(1);
        }
    }

    for (int i = 0; i < num_threads; i++){
        if (pthread_join(thread[i], NULL) != 0) {
            perror("Failed to join thread");
            exit(1);
        }
    }

    printf("After couting: %d\n", count);
    return 0;
}
