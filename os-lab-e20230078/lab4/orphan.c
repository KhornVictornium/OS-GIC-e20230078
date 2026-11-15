/* orphan.c — Demonstrates an orphan process */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid > 0) {
        /* Parent — exits immediately, leaving child as orphan */
        printf("Parent (PID %d): I'm exiting now. Child PID is %d.\n", getpid(), pid);
        exit(0);
    }

    /* Child — continues running after parent dies */
    printf("Child (PID %d): My parent was PID %d.\n", getpid(), getppid());
    printf("Child: Sleeping 60 seconds... check my PPID with 'ps -o pid,ppid,cmd -p %d'\n", getpid());
    sleep(60);
    printf("Child (PID %d): My parent is now PID %d (adopted by init/systemd).\n", getpid(), getppid());

    return 0;
}
