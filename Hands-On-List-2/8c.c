/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a separate program using signal system call to catch the following signals.
                        C. SIGFPE
 * */


#include <stdio.h>   // For using Standard Input Output functions like printf().
#include <signal.h>  // For using signal handling functions like signal().
#include <stdlib.h>  // For using exit().


void floatPtExceptHandler(int id) {                    // Signal handler for SIGFPE
    printf("Floating Point Exception Caught.\n");
    exit(1);
}

int main() {
    signal(SIGFPE, floatPtExceptHandler); // Register signal handler for SIGFPE

    int x = 127 / 0;                      // Cause a floating-point exception

    return 0;
}


/*


Floating Point Exception Caught.


*/