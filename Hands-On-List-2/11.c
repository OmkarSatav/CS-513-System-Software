
/*============================================================================
Name : 11.c
Author : Omkar Satav
Description : Write a program to ignore a SIGINT signal then reset the default 
              action of the SIGINT signal - use sigaction system call.
Date: 11th Sept, 2024.
============================================================================
*/


#include <signal.h>     // For using signal(), SIG_INT and SIG_IGN.
#include <stdio.h>      // For using Standard Input Output Functions like printf().
#include <unistd.h>     // For using sleep().
#include <stdlib.h>     // For using exit().


void sigint_handler() {              // Signal handler function for SIGINT
    printf("\nSIGINT caught, exiting.\n");
    exit(0);
}

int main() {
    struct sigaction my_action;

    my_action.sa_handler = SIG_IGN;                     // Set handler to ignore SIGINT
    my_action.sa_flags = 0;                             // No special flags
    sigemptyset(&my_action.sa_mask);                    // No signals blocked
    sigaction(SIGINT, &my_action, NULL);                // Apply ignore action
    printf("SIGINT will be ignored for 5 seconds. Press Ctrl+C.\n");
    sleep(5);                                           // Wait for 5 seconds

    my_action.sa_handler = sigint_handler;              // Set user defined handler
    my_action.sa_flags = 0;                             // No special flags
    sigemptyset(&my_action.sa_mask);                    // No signals blocked
    sigaction(SIGINT, &my_action, NULL);                // Apply custom handler
    printf("\nSIGINT handling restored.\n");
    sleep(3);                                           // Wait for 3 seconds
    printf("No SIGINT received within 3 seconds\n");

    return 0;
}



/*


SIGINT will be ignored for 5 seconds. Press Ctrl+C.
^C^C^C^C^C
SIGINT handling restored.
^C
SIGINT caught, exiting.
omkar@omkar-TUF-Gaming-


*/