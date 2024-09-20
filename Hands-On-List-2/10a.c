/*============================================================================
Name : 10a.c
Author : Omkar Satav
Description : Write a separate program using sigaction system call to catch the following signals. 
              a. SIGSEGV (Segmentation Fault)
Date: 20th Sept, 2024.
============================================================================*/

#include <stdio.h>   // For using Standard Input Output Functions like printf().
#include <signal.h>  // For using sigaction() 
#include <stdlib.h>  // For using exit().


void segmentationhandler(int signalId) {             // Handler function for SIGSEGV
    printf("SIGSEGV signal caught.\n");
    exit(1); 
}

int main() {
    struct sigaction sigAction;
    sigAction.sa_handler = segmentationhandler;  // User defined handler for SIGSEGV.

    sigaction(SIGSEGV, &sigAction, NULL);      // Install the handler for SIGSEGV signal

    *(int *)0 = 0;                             // Segmentation Fault

    return 0;
}

/*


SIGSEGV signal caught.


*/
