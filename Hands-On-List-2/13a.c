/*============================================================================
Name : 13a.c
Author : Omkar Satav
Description : Write two programs: first program is waiting to catch SIGSTOP signal, 
              the second program will send the signal (using kill system call).
              Find out whether the first  program is able to catch the signal or not.
Date: 19th Sept, 2024.
============================================================================
*/


#include <stdio.h>   // For using Standard Input Output Functions like printf().
#include <signal.h>  // For using SIGSTOP


void signalReceived(int id) {           // Signal handler function
    printf("Signal %d caught!\n", id);
}

int main() {
    
    signal(SIGSTOP, signalReceived);  // Set SIGSTOP signal a user-defined handler function.

    printf("Waiting for SIGSTOP signal...\n");  

    while (1) {} 

    return 0;
}


/*


A program can't catch the SIGSTOP signal, as we see the handler function never runs and the program just stops instead.


*/