/* 
==========================================================================
Name: 19.c
Author: Omkar Dhananjay Satav
Description :  Write a program to find out time taken to execute getpid system call. Use time stamp counter.
Date : 30th Aug, 2024
===========================================================================
*/


#include <stdio.h>  // For Standard Input Output Functions like printf().
#include <unistd.h>  // For using getpid() function.

unsigned long long rdtsc() {
    unsigned long long destination;
    __asm__ __volatile__ ("rdtsc":"=A" (destination));
}

int main() {
    unsigned long long startTime, endTime;
    int pid;

    
    startTime = rdtsc();  // Current timestamp noted(Clock Cycle), Start Time

    
    pid = getpid();       // Call to getpid() system call

    
    endTime = rdtsc();    // Current timestamp noted(Clock Cycle), End Time

   
    unsigned long long timeDuration = endTime - startTime;   // Clock cycle required by getpid().

    printf("PID: %d\n", pid);
    printf("Time taken by getpid() function: %llu clock time duration\n", timeDuration);

    return 0;
}


/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment19$ gcc timeStamp.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment19$ ./a.out
PID: 8874
Time taken by getpid() function: 34092 clock time duration


*/