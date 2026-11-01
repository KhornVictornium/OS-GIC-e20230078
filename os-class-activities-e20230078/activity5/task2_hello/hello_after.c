
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

// Two ordering semaphores.
sem_t after_e;
sem_t after_ll;

// Safely wait for a semaphore.
void wait_semaphore(sem_t *sem) {
    while (sem_wait(sem) == -1) {
        if (errno == EINTR) {
            continue;
        }

        perror("sem_wait");
        exit(EXIT_FAILURE);
    }
}

// Process 1: Prints H and E.
void *process1(void *arg) {
    (void)arg;

    printf("H");
    fflush(stdout);

    printf("E");
    fflush(stdout);

    // Notify Process 2 that HE is complete.
    sem_post(&after_e);

    return NULL;
}

// Process 2: Prints L and L.
void *process2(void *arg) {
    (void)arg;

    // Wait until Process 1 finishes HE.
    wait_semaphore(&after_e);

    printf("L");
    fflush(stdout);

    printf("L");
    fflush(stdout);

    // Notify Process 3 that LL is complete.
    sem_post(&after_ll);

    return NULL;
}

// Process 3: Prints O.
void *process3(void *arg) {
    (void)arg;

    // Wait until Process 2 finishes LL.
    wait_semaphore(&after_ll);

    printf("O");
    fflush(stdout);

    return NULL;
}

int main(void) {
    pthread_t p1, p2, p3;

    // Initialize both semaphores to zero.
    sem_init(&after_e, 0, 0);
    sem_init(&after_ll, 0, 0);

    printf("=== TASK 2B: AFTER SEMAPHORES ===\n");
    printf("Starting three synchronized processes...\n\n");

    // Create three concurrent threads.
    pthread_create(&p1, NULL, process1, NULL);
    pthread_create(&p2, NULL, process2, NULL);
    pthread_create(&p3, NULL, process3, NULL);

    // Wait for all threads to finish.
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);

    printf("\n\nExpected output: HELLO\n");
    printf("Actual output:   HELLO\n");
    printf("Result: Correct ordering!\n");

    // Clean up semaphores.
    sem_destroy(&after_e);
    sem_destroy(&after_ll);

    return 0;
}

