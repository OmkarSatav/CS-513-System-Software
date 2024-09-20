/*============================================================================
Name        : 10c.c
Author      : Omkar Satav
Description : Write a separate program using sigaction system call to catch the following signals. 
              c. SIGFPE
Date        : 20th Sept, 2024.
============================================================================
*/

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>


void floatingexceptionhandler(int signalid) {           // Signal handler for Floating Point Error.
    printf("Floating-point exception caught : Integer division by zero!\n");
    exit(1);
}

int main() {
    struct sigaction sigAction;
    sigAction.sa_handler = floatingexceptionhandler;     // Set Floating Point Error Handler.
    sigemptyset(&sigAction.sa_mask);                 // Clear all signals from the mask
    sigAction.sa_flags = 0;                          // No special flags

    sigaction(SIGFPE, &sigAction, NULL);            // Setup the sigaction for SIGFPE

    int x = 1 / 0;                                  // Floating-point exception (division by zero).

    return 0;
}


/*


Floating-point exception caught : Integer division by zero!


*/