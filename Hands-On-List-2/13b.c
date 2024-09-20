/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write two programs: first program is waiting to catch SIGSTOP signal, the second program
                      will send the signal (using kill system call). Find out whether the first program is able to catch
                      the signal or not.
 * */


#include <stdio.h>   // For using Standard Input Output Functions like printf() and scanf().
#include <signal.h>  // For using kill().

int main() {
    int pid;

    printf("Enter the PID of the first program: ");         // Input Process Id.
    scanf("%d", &pid);


    if (kill(pid, SIGSTOP) == 0) {                          // Send SIGSTOP signal to the first program
        printf("SIGSTOP signal sent to the process %d\n", pid);
    }
    return 0;
}


