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

    // Create a pipe
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork the first child process (C1)
    if ((child1 = fork()) == -1) {
        perror("fork C1");
        exit(EXIT_FAILURE);
    }

    if (child1 == 0) {  // In child process C1
        printf("In CHILD-1 (PID=%d): executing command %s ...\n", getpid(), argv1[0]);
        close(fd[0]);  // Close read end of the pipe
        dup2(fd[1], STDOUT_FILENO);  // Redirect standard output to write end of the pipe
        close(fd[1]);  // Close the write end since we've redirected it
        execvp(argv1[0], argv1);
        // If execvp() fails
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    // Fork the second child process (C2)
    if ((child2 = fork()) == -1) {
        perror("fork C2");
        exit(EXIT_FAILURE);
    }

    if (child2 == 0) {  // In child process C2
        printf("In CHILD-2 (PID=%d): executing command %s ...\n", getpid(), argv2[0]);
        close(fd[1]);  // Close write end of the pipe
        dup2(fd[0], STDIN_FILENO);  // Redirect standard input from read end of the pipe
        close(fd[0]);  // Close the read end since we've redirected it
        execvp(argv2[0], argv2);
        // If execvp() fails
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    // Close both ends of the pipe in the parent
    close(fd[0]);
    close(fd[1]);

    // Wait for both child processes to finish
    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);
    printf("In PARENT (PID=%d): successfully reaped child (PID=%d)\n", getpid(), child1);
    printf("In PARENT (PID=%d): successfully reaped child (PID=%d)\n", getpid(), child2);

    return 0;
}
