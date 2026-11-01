
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <errno.h>
#include <unistd.h>

#define BUFFER_SIZE 100
#define PAIR_CAPACITY 50
#define NUM_PRODUCERS 3

typedef struct {
    int machine;
    int pair_id;
    int part;
} Particle;

// Shared buffer: exactly 100 particle slots.
Particle buffer[BUFFER_SIZE];

int in = 0;
int out = 0;
int count = 0;

int produced = 0;
int packaged = 0;

// Semaphores.
sem_t empty_pairs;
sem_t full_pairs;
sem_t mutex;

atomic_int error_detected = 0;

void report_error(const char *message) {
    if (!atomic_exchange(&error_detected, 1)) {
        printf("\nERROR: %s\n", message);
        fflush(stdout);
    }
}

// Wait safely if interrupted by a signal.
int wait_semaphore(sem_t *sem) {
    while (sem_wait(sem) == -1) {
        if (errno == EINTR) {
            continue;
        }

        perror("sem_wait");
        return -1;
    }

    return 0;
}

// Producer machines.
void *producer(void *arg) {
    int machine = *(int *)arg;
    int pair_id = 0;

    while (!atomic_load(&error_detected)) {
        pair_id++;

        Particle p1 = {machine, pair_id, 1};
        Particle p2 = {machine, pair_id, 2};

        // Wait for space for one complete pair.
        if (wait_semaphore(&empty_pairs) != 0) {
            report_error("Semaphore error");
            break;
        }

        if (wait_semaphore(&mutex) != 0) {
            report_error("Semaphore error");
            break;
        }

        if (atomic_load(&error_detected)) {
            sem_post(&mutex);
            break;
        }

        // Full buffer validation.
        if (count > BUFFER_SIZE - 2) {
            sem_post(&mutex);
            report_error("The producing machine is broken");
            break;
        }

        // Insert two consecutive particles.
        buffer[in] = p1;
        in = (in + 1) % BUFFER_SIZE;

        buffer[in] = p2;
        in = (in + 1) % BUFFER_SIZE;

        count += 2;
        produced++;

        printf("Machine %d creates pair %d\n",
               machine, pair_id);

        printf("Produced pairs: %d | Packaged pairs: %d | "
               "Buffer particles: %d\n",
               produced, packaged, count);

        fflush(stdout);

        sem_post(&mutex);

        // Notify consumer that one complete pair is ready.
        sem_post(&full_pairs);

        usleep(10000);
    }

    return NULL;
}

// One packaging machine.
void *consumer(void *arg) {
    (void)arg;

    while (!atomic_load(&error_detected)) {

        // Wait until a complete pair is available.
        if (wait_semaphore(&full_pairs) != 0) {
            report_error("Semaphore error");
            break;
        }

        if (wait_semaphore(&mutex) != 0) {
            report_error("Semaphore error");
            break;
        }

        if (atomic_load(&error_detected)) {
            sem_post(&mutex);
            break;
        }

        // Empty buffer validation.
        if (count < 2) {
            sem_post(&mutex);
            report_error("The packaging machine is broken");
            break;
        }

        // Remove the first particle.
        Particle p1 = buffer[out];
        out = (out + 1) % BUFFER_SIZE;

        // Remove the second particle.
        Particle p2 = buffer[out];
        out = (out + 1) % BUFFER_SIZE;

        count -= 2;

        // Verify pair identity and particle order.
        if (p1.machine != p2.machine ||
            p1.pair_id != p2.pair_id ||
            p1.part != 1 ||
            p2.part != 2) {

            sem_post(&mutex);
            report_error("Pairs are incorrect");
            break;
        }

        packaged++;

        printf("Packaged: M%d-%d-P1 + M%d-%d-P2\n",
               p1.machine, p1.pair_id,
               p2.machine, p2.pair_id);

        printf("Produced pairs: %d | Packaged pairs: %d | "
               "Buffer particles: %d\n",
               produced, packaged, count);

        fflush(stdout);

        sem_post(&mutex);

        // Release space for one complete pair.
        sem_post(&empty_pairs);

        usleep(15000);
    }

    return NULL;
}

int main(void) {
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumer_thread;

    int machine_ids[NUM_PRODUCERS] = {1, 2, 3};

    // Initialize semaphores.
    if (sem_init(&empty_pairs, 0, PAIR_CAPACITY) == -1 ||
        sem_init(&full_pairs, 0, 0) == -1 ||
        sem_init(&mutex, 0, 1) == -1) {
        perror("sem_init");
        return EXIT_FAILURE;
    }

    printf("====================================\n");
    printf(" TASK 1B: AFTER SEMAPHORES\n");
    printf("====================================\n");

    printf("Producer machines: %d\n", NUM_PRODUCERS);
    printf("Packaging machines: 1\n");
    printf("Buffer capacity: %d particles\n", BUFFER_SIZE);
    printf("Maximum pairs: %d\n", PAIR_CAPACITY);
    printf("empty_pairs: 50\n");
    printf("full_pairs: 0\n");
    printf("mutex: 1\n");
    printf("Semaphore synchronization: ENABLED\n\n");

    // Start consumer.
    if (pthread_create(&consumer_thread, NULL,
                       consumer, NULL) != 0) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    // Start three producers.
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        if (pthread_create(&producers[i], NULL,
                           producer, &machine_ids[i]) != 0) {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    // Keep the simulation running.
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }

    pthread_join(consumer_thread, NULL);

    // Cleanup after an error.
    sem_destroy(&empty_pairs);
    sem_destroy(&full_pairs);
    sem_destroy(&mutex);

    return EXIT_SUCCESS;
}

