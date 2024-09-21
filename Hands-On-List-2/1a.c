/*============================================================================
Name : 1a.c
Author : Omkar Satav
Description :Write a separate program (for each time domain) to set a interval timer in 10s and 10ms 
             a.ITIMER_REAL (10 s)
Date: 17th Sept, 2024.
============================================================================
*/


#include <stdio.h>      // For using Standard Input Output Functions like printf().
#include <sys/time.h>   // For using itimerval and setitimer().
#include <signal.h>     // For using signal() and SIGALRM


void alarmed(int signalId) {    // Signal handler function
    printf("Time Over - (10 sec and 10 microsec)!\n");
}

int main() {
    struct itimerval clock;
    
    signal(SIGALRM, alarmed);   // Install the signal handler for SIGALRM

 
    clock.it_value.tv_sec = 10;        // First time expiration in 10 seconds and 10 microseconds.
    clock.it_value.tv_usec = 10;   

    clock.it_interval.tv_sec = 10;     // repeat every 10 seconds and 10 microseconds
    clock.it_interval.tv_usec = 10;  

    setitimer(ITIMER_REAL, &clock, NULL);   // Set the interval timer

    while (1){}   // Wait for timer signal.

    return 0;
}


/*


omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList2/Assignment1$ ./a.out
Time Over - (10 sec and 10 microsec)!
Time Over - (10 sec and 10 microsec)!
Time Over - (10 sec and 10 microsec)!
Time Over - (10 sec and 10 microsec)!
^C

*/
