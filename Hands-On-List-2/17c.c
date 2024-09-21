/*============================================================================
Name : 17c.c
Author : Omkar Satav
Description :Write a program to execute ls -l | wc
             c.Use fcntl
Date: 20th Sept, 2024.
============================================================================
*/


#include <stdio.h>    // For using Standard Input Output Functions like printf().
#include <unistd.h>   // For using pipe(), fork(), dup2(), execlp(), and close().
#include <fcntl.h>    // For using fcntl() to manipulate file descriptors
#include <sys/wait.h> // For using wait().

int main() {
    int filedescript[2];              // Array for pipe file descriptors
    pipe(filedescript);               // Create a pipe

    if (fork()) {           // Create a child process
        close(0);           // Close standard input
        close(filedescript[1]);       // Close the write end of the pipe
        
        fcntl(filedescript[0], F_DUPFD, 0);   // Duplicate read end of the file.
        
        execlp("wc", "wc", NULL);   // Execute 'wc'.
    } else {
        close(1);           // Close standard output
        close(filedescript[0]);       // Close the read end of the pipe
        
     
        fcntl(filedescript[1], F_DUPFD, 1);   // Duplicate write end of the file.
        
        execlp("ls", "ls", "-l", NULL);  // Execute 'ls -l'
    }

    return 0;
}




/*


8      65     372


*/
