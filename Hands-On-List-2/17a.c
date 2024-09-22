/*============================================================================
Name : 17a.c
Author : Omkar Satav
Description :Write a program to execute ls -l | wc
             a.Use dup
Date: 19th Sept, 2024.
============================================================================
*/

#include <stdio.h>    // For using Standard Input Output Functions like printf().
#include <unistd.h>   // For using pipe(), fork(), dup2(), execlp(), and close().
#include <sys/wait.h> // For using wait().

int main() {
    int fd[2];                            // File descriptors for the pipe
    pid_t prcId, prcId2;

    pipe(fd);                             // Create a pipe

    if ((prcId = fork()) == 0) {          // Fork the first child to run 'ls -l'
        close(fd[0]);                     // Close the unused read end of the pipe
        close(STDOUT_FILENO);            
        dup(fd[1]);                       // Redirect stdout to the write end of the pipe
        close(fd[1]);                   
        execlp("ls", "ls", "-l", NULL);   // Execute 'ls -l'
    }

    if ((prcId2 = fork()) == 0) {         // Fork the second child to run 'wc'
        close(fd[1]);                     // Close unused write end of the pipe
        close(STDIN_FILENO);     
        dup(fd[0]);                       // Link the stdin to the read end of the pipe
        close(fd[0]);                     
        execlp("wc", "wc", NULL);         // Execute 'wc'
    }

    close(fd[0]);                         // Close both ends of the pipe.
    close(fd[1]);

    wait(NULL);                           // Wait for both child processes to finish
    wait(NULL);

    return 0;
}



/*


5      38     212


*/