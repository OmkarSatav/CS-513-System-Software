/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a program to create three child processes. The parent should wait for a particular child (use
                    waitpid system call).
 * */

#include <stdio.h>  // For using Standard Input Output Functions like printf().
#include <unistd.h>  // For using fork(), getpid(), getppid(), and waitpid()
#include <sys/wait.h> // For using waitpid()

int main() {
    int kid1 = fork();

    if (kid1 == 0) {
        printf("Kid 1: Process ID = %d, Parent Process ID = %d\n", getpid(), getppid());     // Kid1 processId and Main Parent Id
    } else if (kid1 > 0) {
        int kid2 = fork();

        if (kid2 == 0) {
            printf("Kid 2: Process ID = %d, Parent Process ID = %d\n", getpid(), getppid());    // Kid2 processId and Main Parent Id
        } else if (kid2 > 0) {
            int kid3 = fork();

            if (kid3 == 0) {    
                printf("Kid 3: Process ID = %d, Parent Process ID = %d\n", getpid(), getppid());    // Kid3 processId and Main Parent Id
            } else {
                int status;
                waitpid(kid2, &status, 0);
                printf("Parent: Kid 2 {Process ID = %d} has finished execution.\n", kid2);        
            }
        }
    }

    return 0;
}


/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment25$ gcc waitPid.c
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment25$ ./a.out
Kid 1: Process ID = 15387, Parent Process ID = 15386
Kid 2: Process ID = 15388, Parent Process ID = 15386
Kid 3: Process ID = 15389, Parent Process ID = 15386
Parent: Kid 2 {Process ID = 15388} has finished execution.


*/