/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a program to find out total number of directories on the pwd.
                      execute ls -l | grep ^d | wc ? Use only dup2.
 * */

#include <unistd.h>    // For using fork(), pipe(), dup2(), and execlp().

int main() {
    int fd1[2], fd2[2];
    pipe(fd1);                  // Create a pipe1
    pipe(fd2);                  // Create a pipe2

    if (!fork()) {                // First child process
        close(fd1[0]);
        dup2(fd1[1], 1);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);
        execlp("ls", "ls", "-l", NULL);             // ls -l command
    } else {
        if (!fork()) {           // Second child process 
            close(fd1[1]);
            dup2(fd1[0], 0);
            close(fd1[0]);
            close(fd2[0]);
            dup2(fd2[1], 1);
            close(fd2[1]);
            execlp("grep", "grep", "^d", NULL);       // grep ^d command
        } else {                  // Parent process
            close(fd1[0]);
            close(fd1[1]);
            close(fd2[1]);
            dup2(fd2[0], 0);
            close(fd2[0]);
            execlp("wc", "wc", "-l", NULL);          // wc -l command
        }
    }

    return 0;
}


/*


2


*/