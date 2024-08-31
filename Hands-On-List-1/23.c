/* 
==========================================================================
Name: 23.c
Author: Omkar Dhananjay Satav
Description : Write a program to create a Zombie state of the running program.
Date : 29th Aug, 2024
===========================================================================
*/


#include <stdio.h>      // For Standard Input Output Functions like printf() and perror().
#include <unistd.h>     // For functions like fork()
#include <sys/wait.h>   // For sleep() function
#include <stdlib.h>     // For exit() function

int main() {
    int childPid;

    // Create a child process
    childPid = fork();

    if (childPid < 0) {
        // Fork failed
        perror("fork");
        return 1;
    }
    else if (childPid == 0) {         // Child process control
        
        printf("Child process PID: %d is terminating now.\n", getpid());      // Child process id
        exit(0);                           // Child exits immediately
    }
    else {                                // Parent process control
        
        printf("Parent process PID: %d is sleeping for 30 seconds. As child process completes its execution, it will wait for parent process. So that child can terminate. So child process becomes zombie.\n", getpid());
        
        sleep(30);  // Parent process sleeping for 30 seconds, so that child process can become zombie for that moment. 

        printf("Parent process PID: %d finished sleeping.\n", getpid());
    }

    return 0;
}



/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment23$ gcc zombieProcess.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment23$ ./a.out
Parent process PID: 18261 is sleeping for 30 seconds. As child process completes its execution, it will wait for parent process. So that child can terminate. So child process becomes zombie.
Child process PID: 18262 is terminating now.
Parent process PID: 18261 finished sleeping.
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment23$ 



Command --> ps aux
omkar      18261  0.0  0.0   2616  1280 pts/2    S+   17:56   0:00 ./a.out
omkar      18262  0.0  0.0      0     0 pts/2    Z+   17:56   0:00 [a.out] <defunct>

status of child process (Z+) zombie


*/
