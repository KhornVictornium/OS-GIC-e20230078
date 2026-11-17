/* three_children.c — Forks 3 children and waits for all of them */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("Parent (PID %d): Creating 3 child processes...\n", getpid());

    for (int i = 1; i <= 3; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(1);
        }

        if (pid == 0) {
            printf("  Child %d (PID %d, PPID %d): Sleeping 30 seconds...\n", i, getpid(), getppid());
            sleep(30);
            printf("  Child %d (PID %d): Exiting.\n", i, getpid());
            exit(0);
        }
    }

    /* Parent waits for all 3 children */
    for (int i = 1; i <= 3; i++) {
        int status;
        pid_t finished_pid = wait(&status);
        printf("Parent: Child PID %d finished (collected %d/3).\n", finished_pid, i);
    }

    printf("Parent (PID %d): All 3 children reaped. Exiting.\n", getpid());
    return 0;
}
