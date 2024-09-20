/*======================================================================================================
Name : 23.c
Author : Omkar Satav
Description : Write a program to print the maximum number of files can be opened within a process and
              size of a pipe (circular buffer).
Date: 20th Sept, 2024.
========================================================================================================
*/


#include <unistd.h> // For using pathconf() and sysconf().
#include <stdio.h>  // For using Standard Input Output Functions like printf().

int main() {
    long pipesize, maxopenfiles;

    pipesize = pathconf(".", _PC_PIPE_BUF);   // Get the size of a pipe (circular buffer)
    
    maxopenfiles = sysconf(_SC_OPEN_MAX);     // Get the maximum number of files that can be opened within a process
    
    printf("Max size of pipe : %ld", pipesize);
    printf("\nMax file can be opened : %ld", maxopenfiles);

    return 0;
}


/*


max size of pipe : 4096
max file can be opened : 1024


*/