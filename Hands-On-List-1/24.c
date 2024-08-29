/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a program to create an orphan process.
 * */


#include <stdio.h>      // For Standard Input Output Functions like printf() and perror().
#include <unistd.h>     // For functions like fork()
#include <sys/wait.h>   // For sleep() function
#include <stdlib.h>     // For exit() function

int main() {
    int childPid;

    // Create a child process
    childPid = fork();

    if (childPid < 0) {
        perror("fork");   // Fork failed to execute.
        return 1;
    }
    else if (childPid == 0) {         // Child process control
        sleep(7);
        printf("Child process PID: %d started execution and will be sleeping for 10 seconds.\n", getpid());
        sleep(10);
        printf("Child process PID: %d is terminating now.\n", getpid());      // Child process id
                                 
    }
    else {                                // Parent process control
        printf("Parent process PID %d started execution. \n", getpid());
        printf("Parent process PID: %d finished execution.\n", getpid());
        exit(0);                   // Parent process exits immediately
    }

    return 0;
}


/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment24$ Child process PID: 19499 is terminating now.
gcc orphanProcess.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment24$ ./a.out
Parent process PID 19593 started execution. 
Parent process PID: 19593 finished execution.
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment24$ Child process PID: 19594 started execution and will be sleeping for 30 seconds.
Child process PID: 19594 is terminating now.




Command --> ps -ef | grep orphan_process
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment23$ ps -ef | grep orphan_process
omkar      19596   18173  0 18:15 pts/1    00:00:00 grep --color=auto orphan_process
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assign

status of child process (Z+) zombie


*/
