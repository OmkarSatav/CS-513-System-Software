/*============================================================================
Name : 16.c
Author : Omkar Satav
Description :Write a program to send and receive data from parent to child vice versa. Use two way communication.
Date: 12th Sept, 2024.
============================================================================
*/


#include <unistd.h>  // For using functions like pipe(), fork(), read(), write(), close().
#include <stdio.h>   // For using Standard Input Output Functions like printf().

int main() {
    int parentPipe[2], childPipe[2];  // Two pipes for two-way communication

    pipe(parentPipe);            // Parent Pipe : Parent writes, Child reads
    pipe(childPipe);            // Child Pipe : Child writes, Parent reads

    if (!fork()) {                // Child process
        close(parentPipe[0]);  // Close read end of the Parent pipe
        char buffer[23];
        
        write(parentPipe[1], "Child : This side !!\n", 22);    // Writing message to parent through Parent pipe

        close(childPipe[1]);  // Close the write end of the Child pipe

        read(childPipe[0], &buffer, 23);     // Reading message from parent through Child pipe
        
        printf("Received from parent: %s", buffer);  
    } else {                      // Parent process
        close(parentPipe[1]);  // Close write end of the Parent pipe
        char buffer[22];

        read(parentPipe[0], &buffer, 22);        // Reading message from child through Parent pipe
        printf("Received from child: %s", buffer);  

        close(childPipe[0]);                    // Close the read end of the Child pipe

        write(childPipe[1], "Parent : This side !!\n", 23);    // Writing message to child through Child pipe
    }

    return 0;
}



/*


Received from child: Child : This side !!
Received from parent: Parent : This side !!


*/
