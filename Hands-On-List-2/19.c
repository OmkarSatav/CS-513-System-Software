/*============================================================================
Name : 19.c
Author : Omkar Satav
Description : Create a FIFO file by 
              a. mknod command
              b. mkfifo command
              c. use strace command to find out, which command (mknod or mkfifo) is better.
              d. mknod system call
              e. mkfifo library function
Date: 19th Sept, 2024.
============================================================================
*/


#include <sys/types.h> // For using mkfifo and mknod
#include <sys/stat.h>  // For using mkfifo and mknod
#include <fcntl.h>     // For using mknod
#include <unistd.h>    // For using close().
#include <stdio.h>     // For using Standard Input Output Functions like printf().

int main() {
    char *mkFifo = "mkFifo1";                     // FIFO file name using mkfifo
    char *mknodFifo = "mknod1";                   // FIFO file name using mknod


    if (mkfifo(mkFifo, S_IRWXU) == 0) {                 // Create FIFO using mkfifo
        printf("FIFO created using mkfifo : %s\n", mkFifo);
    }


    if (mknod(mknodFifo, S_IFIFO | S_IRWXU, 0) == 0) {  // Create FIFO using mknod
        printf("FIFO created using mknod : %s\n", mknodFifo);
    }

    return 0;
}


/*


FIFO created using mkfifo : mkFifo1
FIFO created using mknod : mknod1


*/