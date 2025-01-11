#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

int num_odd = 0;
int num_even = 0;
#define ARR_SIZE    100
#define MAX_NUM     100
#define MIN_NUM     0

// Function to be executed by each thread
void *thread_count_odd(void *arg) {
    printf("Thread %ld start counting odd numbers.\n", pthread_self());
    for (int i = 0; i < ARR_SIZE; i++) {
        if((*((int *)arg + i) % 2) != 0)
            num_odd++;
    }
    pthread_exit(NULL);  
}

void *thread_count_even(void *arg) {
    printf("Thread %ld sttart counting even numbers.\n", pthread_self());
    for (int i = 0; i < ARR_SIZE; i++) {
        if((*((int *)arg + i) % 2)  == 0)
            num_even++;    
    }
    pthread_exit(NULL);  
}

int main() {
    __uint8_t num_threads = 2;
    pthread_t thread[num_threads]; // Array to hold thread identifiers

    // Create a random array of numbers
    srand(time(0));
    int num_arr[ARR_SIZE];

    printf("Input array: [ ");
    for (int i = 0; i < ARR_SIZE; i++) {
        num_arr[i] = rand() % (MAX_NUM - MIN_NUM);
        printf("%d ", num_arr[i]);
    }
    printf("]\n");

    // Create a new thread and run
    if (pthread_create(&thread[0], NULL, thread_count_odd, (void *)num_arr) != 0) {
        perror("Failed to create thread");
        exit(1);
    }

    if (pthread_create(&thread[1], NULL, thread_count_even, (void *)num_arr) != 0) {
        perror("Failed to create thread");
        exit(1);
    }
        
    for (int i = 0; i < num_threads; i++){
        if (pthread_join(thread[i], NULL) != 0) {
            perror("Failed to join thread");
            exit(1);
        }
    }

    printf("Number of odd: %d\n", num_odd);
    printf("Number of even: %d\n", num_even);

    return 0;
}
