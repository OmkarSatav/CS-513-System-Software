/*============================================================================
Name : 9.c
Author : Omkar Satav
Description : Write a program to ignore a SIGINT signal then reset the 
              default action of the SIGINT signal - Use signal system call.
Date: 11th Sept, 2024.
============================================================================
*/

#include <signal.h>     // For using signal(), SIG_INT and SIG_IGN.
#include <stdio.h>      // For using Standard Input Output Functions like printf().
#include <unistd.h>     // For using sleep().
#include <stdlib.h>     // For using exit().


void sigint_handler() {          // Signal handler function for SIGINT
    printf("\nSIGINT caught, exiting.\n");
    exit(0);
}

int main() {
    signal(SIGINT, SIG_IGN);    // Ignore the SIGINT signal.
    printf("SIGINT will be ignored for 5 seconds. Press Ctrl+C.\n");
    sleep(5);

    signal(SIGINT, sigint_handler);    // Allow the program to handle SIGINT after 5 seconds.
    printf("\nSIGINT handling restored.\n");
    sleep(2);
    printf("No SIGINT triggered in 2 seconds.");
    return 0;
}

/*


SIGINT will be ignored for 5 seconds. Press Ctrl+C.
^C^C^C^C^C^C^C^C^C^C^C^C^C^C
SIGINT handling restored.
SIGINT caught, exiting.

*/
