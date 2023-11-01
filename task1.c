#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    char * argv1[] = {"cat", "Makefile", 0};
    //char * argv2[] = {"head", "-4", 0};
    char * argv2[] = {"wc", "-l", 0};
    
    setbuf(stdout, NULL);

    int fd[2];
    pid_t child1, child2;

    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if ((child1 = fork()) == -1) {
        perror("fork C1");
        exit(EXIT_FAILURE);
    }

    if (child1 == 0) { 
        printf("In CHILD-1 (PID=%d): executing command %s ...\n", getpid(), argv1[0]);
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        execvp(argv1[0], argv1);
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    if ((child2 = fork()) == -1) {
        perror("fork C2");
        exit(EXIT_FAILURE);
    }

    if (child2 == 0) {
        printf("In CHILD-2 (PID=%d): executing command %s ...\n", getpid(), argv2[0]);
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        execvp(argv2[0], argv2);
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);
    printf("In PARENT (PID=%d): successfully reaped child (PID=%d)\n", getpid(), child1);
    printf("In PARENT (PID=%d): successfully reaped child (PID=%d)\n", getpid(), child2);

    return 0;
}
