/*============================================================================
Name : 12.c
Author : Omkar Satav
Description : Write a program to create an orphan process.Use kill system call to send 
              SIGKILL signal to the parent process from the child process.
Date: 11th Sept, 2024.
============================================================================
*/

#include <signal.h>     // For using kill().
#include <stdio.h>      // For using Standard Input Output Functions like printf().
#include <unistd.h>     // For using system calls like fork() and sleep().
#include <stdlib.h>     // For using Standard Library Functions like exit().

int main() {
    int pid = fork();  // Create a child process.

    if (pid == 0) {                 // Child process
        sleep(1);                      
        kill(getppid(), SIGKILL);              // Send SIGKILL to parent
        printf("\nChild process has sent SIGKILL to parent.\n");
        exit(0);
    } else if (pid > 0) {           // Parent process
        while (1) {
            printf("Parent process is executing.\n");
            sleep(1);  
        }
    }

    return 0;
}


/*


Parent process is executing.

Child process has sent SIGKILL to parent.
Killed


*/