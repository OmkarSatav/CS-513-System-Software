/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a separate program using signal system call to catch the following signals.
                        F. SIGPROF.
 * */


#include <stdio.h>      // For using Standard Input Output functions like printf().
#include <signal.h>     // For using signal handling functions like signal().
#include <stdlib.h>     // For using exit().
#include <sys/time.h>   // For using setitimer() and itimerval.
#include <unistd.h>     // For using pause().


void profileTimerHandler(int signum) {              // Signal handler for SIGPROF
    printf("Caught Profiling timer signal.\n");
    exit(0);
}

int main() {
    struct itimerval clock;

    signal(SIGPROF, profileTimerHandler);  // Set signal handler for SIGPROF

   
    clock.it_value.tv_sec = 5;            // Set the timer. First expiration in 2 seconds
    clock.it_value.tv_usec = 0;

    clock.it_interval.tv_sec = 0;         // No repeat
    clock.it_interval.tv_usec = 0;
    
    setitimer(ITIMER_PROF, &clock, NULL); // Set the profiling timer

    pause();                              // Wait for the signal

    return 0;
}



/*


Caught Profiling timer signal.


*/