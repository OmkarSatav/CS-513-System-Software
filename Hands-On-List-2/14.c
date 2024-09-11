/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a simple program to create a pipe, write to the pipe, read from pipe and display on
                      the monitor.
 * */


#include <unistd.h>        // For using pipe(), read(), write() and close(). 
#include <stdio.h>         // For using Standard Input Ouput Functions like printf(); 
#include <stdlib.h>        // For using Standar Library Funcitions like exit().

int main() {
    int fd[2];
    pipe(fd);                // Create the pipe

    write(fd[1], "Hello World!!", 14); // Write "Hello World!!" to pipe.

    char buffer[20];

    read(fd[0], buffer, sizeof(buffer)); // Read from pipe.
    printf("Received: %s\n", buffer);  

    close(fd[0]); // Close the read and write end of the pipe.
    close(fd[1]); 

    return 0;
}



/*


Received: Hello World!!


*/