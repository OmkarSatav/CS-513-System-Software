/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a separate program (for each time domain) to set a interval timer in 10sec and
                      10micro second
                        b. ITIMER_VIRTUAL
 * */




#include <stdio.h>      // For using Standard Input Output Functions like printf().
#include <sys/time.h>   // For using itimerval and setitimer().
#include <signal.h>     // For using signal() and SIGVTALRM.


void alarmed(int signalId) {    // Signal handler function
    printf("Virtual Timer is Over - (10 sec and 10 microsec)!\n");
}

int main() {
    struct itimerval clock;
    
    signal(SIGVTALRM, alarmed);           // Install the signal handler for SIGVTALRM.
 
    clock.it_value.tv_sec = 10;        // First time expiration in 10 seconds and 10 microseconds.
    clock.it_value.tv_usec = 10;   

    clock.it_interval.tv_sec = 10;     // repeat every 10 seconds and 10 microseconds.
    clock.it_interval.tv_usec = 10;  

    setitimer(ITIMER_VIRTUAL, &clock, NULL);  // Set the interval timer. ITIMER_VIRTUAL counts down only during user-mode execution.

    while (1){}   // Wait for timer signal.

    return 0;
}


/*


omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList2/Assignment1$ ./a.out
Virtual Timer is Over - (10 sec and 10 microsec)!
Virtual Timer is Over - (10 sec and 10 microsec)!
Virtual Timer is Over - (10 sec and 10 microsec)!
Virtual Timer is Over - (10 sec and 10 microsec)!
Virtual Timer is Over - (10 sec and 10 microsec)!
Virtual Timer is Over - (10 sec and 10 microsec)!
Virtual Timer is Over - (10 sec and 10 microsec)!
Virtual Timer is Over - (10 sec and 10 microsec)!
Virtual Timer is Over - (10 sec and 10 microsec)!
Virtual Timer is Over - (10 sec and 10 microsec)!
^C

*/