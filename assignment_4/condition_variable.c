#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define BUFFER_SIZE     5
#define NUM_THREAD      2 
#define MAX_NUM         100

__uint32_t buffer[BUFFER_SIZE] = {0};
__uint32_t count = 0;
pthread_mutex_t mutex;
pthread_cond_t cond_var;

void *thread_producer(void *arg) {
    while(1){
        pthread_mutex_lock(&mutex); 

        while (count == BUFFER_SIZE) {
            // Buffer full, producer waits...
            printf("\nBuffer full. Producer is waiting...\n\n");
            pthread_cond_wait(&cond_var, &mutex); 
        }  

        buffer[count] = rand() % MAX_NUM;
        printf("Write %d to buffer\n", buffer[count]);
        count++;
        pthread_cond_signal(&cond_var);
        pthread_mutex_unlock(&mutex);
        sleep(1);
        // pthread_exit(NULL);  
    }
}

void *thread_comsumer(void *arg) {
    while(1){   
        pthread_mutex_lock(&mutex); 
        
        while (count == 0) {
            // Buffer empty, comsumer waits...
            printf("\nBuffer empty. Consumer is waiting...\n\n");
            pthread_cond_wait(&cond_var, &mutex); 
        } 

        printf("Read %d from buffer\n", buffer[count - 1]); 
        count--;
        pthread_cond_signal(&cond_var);
        pthread_mutex_unlock(&mutex);
        sleep(2);
        // pthread_exit(NULL);  
    }
}

int main() {

    pthread_t thread[NUM_THREAD]; // thread identifiers

    // Initialize mutex and condition variable
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_var, NULL);

    // Create a new thread and run
    if (pthread_create(&thread[0], NULL, thread_producer, NULL) != 0) {
        perror("Failed to create thread");
        exit(1);
    }

    if (pthread_create(&thread[1], NULL, thread_comsumer, NULL) != 0) {
        perror("Failed to create thread");
        exit(1);
    }
    
    if ((pthread_join(thread[0], NULL) != 0) || (pthread_join(thread[1], NULL) != 0)) {
        perror("Failed to join thread");
        exit(1);
    }

    // Clean up
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_var);

    return 0;
}
