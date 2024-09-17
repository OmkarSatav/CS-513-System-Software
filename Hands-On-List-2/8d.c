/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a separate program using signal system call to catch the following signals.
                        D. SIGALRM (use alarm system call).
 * */


#include <stdio.h>   // For using Standard Input Output functions like printf().
#include <signal.h>  // For using signal handling functions like signal().
#include <stdlib.h>  // For using exit().

void timeOut(int id) {      // Signal handler for SIGALRM
    printf("SIGALRM signal caught.\n");
    exit(0);
}

int main() {
    signal(SIGALRM, timeOut);    // Set signal handler for SIGALRM

    alarm(2);                           // Set alarm for 5 seconds

    pause();                            // Wait for the signal

    return 0;
}


/*


SIGALRM signal caught.


*/