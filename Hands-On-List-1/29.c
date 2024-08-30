/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a program to get scheduling policy and modify the scheduling policy (SCHED_FIFO,SCHED_RR).
 * */


#include <sched.h>     // For sched_getscheduler() and sched_setscheduler() functions.
#include <sys/types.h> // For getprocessId datatype.
#include <unistd.h>    // For getprocessId() function.
#include <stdio.h>     // For Standard Input Output Functions like perror and printf.
#include <stdlib.h>    // For using exit() function.


void main()
{
    int crtPolicy;
    pid_t processId;
    processId = getpid();
    crtPolicy = sched_getscheduler(processId);
    struct sched_param priority;
    
    priority.sched_priority = 10;
    int algo = -1;

    switch (crtPolicy)
    {
    case SCHED_FIFO:
        printf("Current process policy is FIFO\n");
        algo = SCHED_RR;   // Changing to Robin Round Algorithm.
        break;
    case SCHED_RR:
        printf("Current process policy is RR\n");
        algo = SCHED_FIFO;  // Changing to First In First Out Algorithm.
        break;
    case SCHED_OTHER:
        printf("Current process policy is OTHER\n");
        algo = SCHED_RR;   // Changing to Robin Round Algorithm.
        break;
    default:
        perror("Error in selecting Scheduling algorithm name.");
        exit(1);
    }

    sched_setscheduler(processId, algo, &priority);      // Current scheduling policy changed.
    crtPolicy = sched_getscheduler(processId);
    printf("Current policy is %d\n", crtPolicy);


}

/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment29$ gcc Scheduling.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment29$ sudo ./a.out
Current process policy is OTHER
Current policy is 2
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment29$ 


*/

