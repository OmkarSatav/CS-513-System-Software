/*============================================================================
Name : 10b.c
Author : Omkar Satav
Description : Write a separate program using sigaction system call to catch the following signals. 
              b.SIGINT
Date: 20th Sept, 2024.
============================================================================
*/


#include <stdio.h>   // For using Standard Input Output Functions like printf().
#include <signal.h>  // For using sigaction(). 
#include <stdlib.h>  // For using exit().

void interrupthandler(int signalid) {
    printf("\nCaught interrupt signal.\n");
    exit(1);
}

int main() {
    struct sigaction sigAction;
    sigAction.sa_handler = interrupthandler;    // Set the signal handler
    sigaction(SIGINT, &sigAction, NULL);        // Set up handling of SIGINT
    
    while (1){
        sleep(5);                               // Wait for Interrupt. SIGINT (Ctrl+C)
    }   

    return 0;
}


/*


^C
Caught interrupt signal.


*/