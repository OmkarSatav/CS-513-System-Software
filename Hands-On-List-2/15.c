/*============================================================================
Name : 15.c
Author : Omkar Satav
Description :Write a simple program to send some data from parent to the child process.
Date: 11th Sept, 2024.
============================================================================
*/


#include <unistd.h>        // For using pipe(), read(), write() and close(). 
#include <stdio.h>         // For using Standard Input Ouput Functions like printf(); 
#include <stdlib.h>        // For using Standar Library Funcitions like exit().

int main() {
    int fd[2];
    char buffer[20];

    pipe(fd);             // Create a pipe

    if (fork()) {         // Child process
        close(fd[1]);       // Close the write end of the pipe
        
        read(fd[0], buffer, sizeof(buffer));   // Read data from the pipe
        printf("Child received: %s\n", buffer); 
        close(fd[0]);       // Close the read end of the pipe
        exit(0);
    } else {                // Parent process
        close(fd[0]);       // Close the read end of the pipe
        write(fd[1], "Hello World!!", 14);   // Write data to the pipe
        close(fd[1]);       // Close the write end of the pipe
        wait(NULL);         // Wait for child process to finish execution.
    }

    return 0;
}




/*


Child received: Hello World!!


*/