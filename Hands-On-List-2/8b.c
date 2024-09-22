/*============================================================================
Name : 8b.c
Author : Omkar Satav
Description : Write a separate program using signal system call to catch the following signals.
            b. SIGINT
Date: 17th Sept, 2024.
============================================================================
*/


#include <stdio.h>   // For using Standard Input Output functions like printf().
#include <signal.h>  // For using signal handling functions like signal().
#include <stdlib.h>  // For using exit().


void interruptHandler(int id) {                     // Signal handler for SIGINT
    printf("\nInterrupt Signal Caught.\n");
    exit(0);
}

int main() {
    signal(SIGINT, interruptHandler); // Register signal handler for SIGINT

    printf("Press Ctrl+C to trigger SIGINT...\n");    // Unexpected exit, Interrupt generated.
    
    while (1) {}   // Wait for the signal

    return 0;
}


/*


Press Ctrl+C to trigger SIGINT...
^C
Interrupt Signal Caught.


*/



