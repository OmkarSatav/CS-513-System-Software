/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a program to wait for a STDIN for 10 seconds using select. Write a proper print statement to
                    verify whether the data is available within 10 seconds or not (check in $man 2 select).
 * */

#include <stdio.h>   // For Standard Input Output Functions like printf() and perror().
#include <stdlib.h>  // For using Standard library functions like exit().
#include <unistd.h>   // For using STDIN_FILENO
#include <sys/select.h>  // For using select, FD_SET, FD_ZERO

int main() {
    fd_set readFileDescript;
    struct timeval timer;
    
    timer.tv_sec = 10;   // Set timer of 10 seconds.
    timer.tv_usec = 0;

    FD_ZERO(&readFileDescript);                   // Set FD value to an empty set.
    FD_SET(STDIN_FILENO, &readFileDescript);      // Accept standard input using FD.


    int result;
    result = select(STDIN_FILENO + 1, &readFileDescript, NULL, NULL, &timer);        // Wait 10 seconds for standard input.

    if (result < 0) {
        printf("Select function fails to perform its function.");
        exit(1);
    } else if (result == 0) {
        printf("No data available at the Standard Input.\n");
        exit(0);
    } else if (result > 0) {
        printf("Data available at Standard Input File Descriptor.\n");
        exit(0);
    }

    return 0;
}


/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment13$ gcc selectWrite.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment13$ ./a.out
Hello World!!
Data available

*/