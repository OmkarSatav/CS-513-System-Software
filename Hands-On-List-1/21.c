/* 
==========================================================================
Name: 21.c
Author: Omkar Dhananjay Satav
Description :  Write a program, call fork and print the parent and child process id
Date : 29th Aug, 2024
===========================================================================
*/


#include <sys/types.h> // For fork() & getpid() calls
#include <unistd.h>    // For fork() & getpid() calls
#include <stdio.h>     // For Standard Input Output Functions like printf().

int main()
{
    int pid = fork();  // Create a child process

    if (pid < 0) {           // Error while executing fork()
        perror("Fork failed");
        return 1;
    }else if (pid == 0) {         // Child process is currently have the control.
        printf("---------------- Child Process ------------------\n");
        printf("Parent PID: %d\n", getppid());
        printf("Child PID: %d\n", getpid());
    }else {                      // Parent process is currently have the control.
        printf("---------------- Parent Process -----------------\n");
        printf("Parent PID: %d\n", getpid());
        printf("Child PID: %d\n", pid);
    }
    return 0;   
}




/*
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment21$ gcc processIds.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment21$ ./a.out
---------------- Parent Process -----------------
Parent PID: 10848
Child PID: 10849
---------------- Child Process ------------------
Parent PID: 10848
Child PID: 10849
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment21$ 

*/