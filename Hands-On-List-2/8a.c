/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a separate program using signal system call to catch the following signals.
                        a. SIGSEGV
 * */


#include <stdio.h>   // For using Standard Input Output functions like printf().
#include <signal.h>  // For using signal handling functions like signal().
#include <stdlib.h>  // For using exit().

void segmentationHandler(int id) {           // Signal handler for SIGSEGV
    printf("Segmentation Fault is caught.\n");
    exit(1);
}


int main() {
    signal(SIGSEGV, segmentationHandler);       // Set signal handler for SIGSEGV
    
    int *ptr = NULL;                        // Segmentation fault
    *ptr = 100;

    return 0;
}



/*


Segmentation Fault is caught.


*/