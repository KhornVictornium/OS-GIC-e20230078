/* zombie_reaped.c — Parent sleeps 10s then calls wait() to reap zombie */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        printf("Child (PID %d): Exiting immediately...\n", getpid());
        exit(0);
    }

    printf("Parent (PID %d): Child PID is %d. Sleeping 10s before wait()...\n", getpid(), pid);
    sleep(10);

    int status;
    pid_t reaped = wait(&status);
    printf("Parent: Called wait() and reaped child PID %d (exit status %d).\n", reaped, WEXITSTATUS(status));
    printf("Parent: Sleeping 10s more so you can verify the zombie is gone...\n");
    sleep(10);

    return 0;
}

