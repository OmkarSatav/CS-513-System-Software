#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    pid_t pid1, pid2;

    pipe(fd);  // Create a pipe

    if ((pid1 = fork()) == 0) {  // First child (ls -l)
        dup2(fd[1], STDOUT_FILENO);  // Redirect stdout to pipe
        close(fd[0]);  // Close unused read end
        close(fd[1]);  // Close original write end
        execlp("ls", "ls", "-l", NULL);
        perror("execlp ls");  // Error handling
        return 1;
    }

    if ((pid2 = fork()) == 0) {  // Second child (wc)
        dup2(fd[0], STDIN_FILENO);  // Redirect stdin to pipe
        close(fd[0]);  // Close original read end
        close(fd[1]);  // Close unused write end
        execlp("wc", "wc", NULL);
        perror("execlp wc");  // Error handling
        return 1;
    }

    close(fd[0]);  // Close both ends in parent
    close(fd[1]);
    wait(NULL);  // Wait for both children
    wait(NULL);

    return 0;
}




/*


8      65     372


*/