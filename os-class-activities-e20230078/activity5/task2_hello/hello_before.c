
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *process1(void *arg) {
    (void)arg;

    // Process 1 prints H and E.
    usleep(40000);

    printf("HE");
    fflush(stdout);

    return NULL;
}

void *process2(void *arg) {
    (void)arg;

    // Process 2 prints L and L.
    usleep(10000);

    printf("LL");
    fflush(stdout);

    return NULL;
}

void *process3(void *arg) {
    (void)arg;

    // Process 3 prints O.
    usleep(20000);

    printf("O");
    fflush(stdout);

    return NULL;
}

int main(void) {
    pthread_t p1, p2, p3;

    printf("=== TASK 2A: BEFORE SEMAPHORES ===\n");
    printf("Starting three concurrent processes...\n\n");

    pthread_create(&p1, NULL, process1, NULL);
    pthread_create(&p2, NULL, process2, NULL);
    pthread_create(&p3, NULL, process3, NULL);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);

    printf("\n\nExpected output: HELLO\n");
    printf("Actual output:   LL O HE\n");
    printf("Result: Incorrect ordering!\n");

    return 0;
}

