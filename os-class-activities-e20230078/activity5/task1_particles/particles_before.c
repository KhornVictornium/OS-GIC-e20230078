
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sched.h>
#include <unistd.h>

#define BUFFER_SIZE 100
#define NUM_PRODUCERS 3

typedef struct {
    int machine;
    int pair_id;
    int part;
} Particle;

Particle buffer[BUFFER_SIZE];

int in = 0, out = 0, count = 0;
int produced = 0, packaged = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
atomic_int error_detected = 0;

void report_error(const char *message) {
    if (!atomic_exchange(&error_detected, 1)) {
        printf("\nERROR: %s\n", message);
        fflush(stdout);
    }
}

int add_particle(Particle p) {
    pthread_mutex_lock(&mutex);

    if (count >= BUFFER_SIZE) {
        pthread_mutex_unlock(&mutex);
        report_error("The producing machine is broken");
        return 0;
    }

    buffer[in] = p;
    in = (in + 1) % BUFFER_SIZE;
    count++;

    pthread_mutex_unlock(&mutex);
    return 1;
}

int remove_particle(Particle *p) {
    pthread_mutex_lock(&mutex);

    if (count == 0) {
        pthread_mutex_unlock(&mutex);
        report_error("The packaging machine is broken");
        return 0;
    }

    *p = buffer[out];
    out = (out + 1) % BUFFER_SIZE;
    count--;

    pthread_mutex_unlock(&mutex);
    return 1;
}

void *producer(void *arg) {
    int machine = *(int *)arg;
    int pair_id = 0;

    while (!atomic_load(&error_detected)) {
        pair_id++;

        Particle p1 = {machine, pair_id, 1};
        Particle p2 = {machine, pair_id, 2};

        // Insert P1 separately.
        if (!add_particle(p1)) break;

        sched_yield();

        // Another thread may interfere before P2 is inserted.
        if (!add_particle(p2)) break;

        pthread_mutex_lock(&mutex);
        produced++;

        printf("Machine %d creates pair %d\n",
               machine, pair_id);
        pthread_mutex_unlock(&mutex);

        usleep(1000);
    }

    return NULL;
}

void *consumer(void *arg) {
    (void)arg;

    // Allow producers to begin working.
    usleep(10000);

    while (!atomic_load(&error_detected)) {
        Particle p1, p2;

        // Remove the first particle.
        if (!remove_particle(&p1)) break;

        sched_yield();

        // Remove the second particle separately.
        if (!remove_particle(&p2)) break;

        // Check whether both particles belong to the same pair.
        if (p1.machine != p2.machine ||
            p1.pair_id != p2.pair_id ||
            p1.part != 1 ||
            p2.part != 2) {

            report_error("Pairs are incorrect");
            break;
        }

        pthread_mutex_lock(&mutex);
        packaged++;

        printf("Packaged: M%d-%d-P1 + M%d-%d-P2\n",
               p1.machine, p1.pair_id,
               p2.machine, p2.pair_id);

        printf("Produced pairs: %d | Packaged pairs: %d | "
               "Buffer particles: %d\n",
               produced, packaged, count);

        pthread_mutex_unlock(&mutex);

        usleep(1000);
    }

    return NULL;
}

int main(void) {
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumer_thread;

    int machine_ids[NUM_PRODUCERS] = {1, 2, 3};

    printf("=== TASK 1A: BEFORE SEMAPHORES ===\n");
    printf("WARNING: Pair operations are not synchronized.\n\n");

    pthread_create(&consumer_thread, NULL, consumer, NULL);

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_create(&producers[i], NULL,
                       producer, &machine_ids[i]);
    }

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }

    pthread_join(consumer_thread, NULL);

    pthread_mutex_destroy(&mutex);

    return 0;
}

