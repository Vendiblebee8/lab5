#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    char *argv1[] = {"cat", "Makefile", NULL};
    char *argv2[] = {"head", "-4", NULL};
    char *argv3[] = {"wc", "-l", NULL};
    int fd1[2], fd2[2];
    pid_t child1, child2, child3;

    setbuf(stdout, NULL);  // to disable buffering

    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("pipe");
        exit(1);
    }

    // First child: cat Makefile
    if ((child1 = fork()) == 0) {
        printf("In CHILD-1 (PID=%d): executing command cat ...\n", getpid());
        close(fd1[0]);
        dup2(fd1[1], STDOUT_FILENO);
        close(fd1[1]);
        execvp(argv1[0], argv1);
        perror("execvp");
        exit(1);
    }

    // Second child: head -4
    if ((child2 = fork()) == 0) {
        printf("In CHILD-2 (PID=%d): executing command head ...\n", getpid());
        close(fd1[1]);
        dup2(fd1[0], STDIN_FILENO);
        close(fd1[0]);

        close(fd2[0]);
        dup2(fd2[1], STDOUT_FILENO);
        close(fd2[1]);
        execvp(argv2[0], argv2);
        perror("execvp");
        exit(1);
    }

    close(fd1[0]);
    close(fd1[1]);

    // Third child: wc -l
    if ((child3 = fork()) == 0) {
        printf("In CHILD-3 (PID=%d): executing command wc ...\n", getpid());
        close(fd2[1]);
        dup2(fd2[0], STDIN_FILENO);
        close(fd2[0]);
        execvp(argv3[0], argv3);
        perror("execvp");
        exit(1);
    }

    close(fd2[0]);
    close(fd2[1]);

    // Parent waits for all children
    int status;
    pid_t wpid;
    while ((wpid = wait(&status)) > 0) {
        printf("In PARENT (PID=%d): successfully reaped child (PID=%d)\n", getpid(), wpid);
    }

    return 0;
}
