/* 
==========================================================================
Name: 30.c
Author: Omkar Dhananjay Satav
Description : Write a program to run a script at a specific time using a Daemon process.
Date : 30th Aug, 2024
===========================================================================
*/

#include <time.h>      // For time-related functions
#include <stdio.h>     // For Standard Input Output Functions like printf.
#include <stdlib.h>    // For functions like atoi -string to integer conversion
#include <unistd.h>    // For using functions like fork, setsid and chdir.
#include <sys/stat.h>  // For using umask function.
#include <sys/types.h> // For various datatypes used in fork and setsid. 

int main(int argumentCount, char *argumentValues[]) {
    time_t crtEpoch, deadlineEpoch;    // Current system time & deadline time in epoch
    struct tm *deadline;               // Deadline in user-readable format. User-readable timestamp representation.
    pid_t pid;

    if (argumentCount < 4) {
        printf("Please provide 'hour' 'minutes' 'seconds' as command line arguments.\n");
        exit(EXIT_FAILURE);
    }

    time(&crtEpoch); // Get current time
    deadline = localtime(&crtEpoch);

    deadline->tm_hour = atoi(argumentValues[1]);     // Set the deadline value based on command line argument values.
    deadline->tm_min = atoi(argumentValues[2]);
    deadline->tm_sec = atoi(argumentValues[3]);

    deadlineEpoch = mktime(deadline);      // Convert deadline to epoch


    pid = fork();     // Create child process using fork().

    if (pid < 0) {     // fork() fails to create child process.
        exit(1);
    }
    
    if (pid > 0) {     // Exit the parent process
        exit(0);
    }

    if(pid == 0){
        umask(0);  // Full rights to change permissions. No dependence

        
        pid_t sid = setsid();   // Create a new session for the child process.
        if (sid < 0) {     // Fails to create new session.
            exit(1);
        }

        if (chdir("/") < 0) {         // Change the working directory to root. To avoid any dependence on directory structure.
            exit(1);                  // If fails to change working directory.
        }

        do
        {
            time(&crtEpoch);
        } while (difftime(deadlineEpoch, crtEpoch) > 0);
        // Daemon process waits until the deadline is met.

        printf("Execute a script.");  // Execute the script at this moment.

    }

    exit(0);
}


/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment30$ gcc runScript.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment30$ ./a.out 22 48 0
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment30$ Execute a script.

*/