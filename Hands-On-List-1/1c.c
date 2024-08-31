// Question: Create a FIFO file using the `mkfifo` library function or `mknod` system call

#include <sys/types.h> // Import for `mkfifo` library function & `mknod` system call
#include <sys/stat.h>  // Import for `mkfifo` library function & `mknod` system call
#include <fcntl.h>     // Import for `mknod` system call
#include <unistd.h>    // Import for `mknod` system call
#include <stdio.h>     // For Standard Input Output Functions like printf() function

void main()
{
    int mkfifo_status, mknod_status; 

    mkfifo_status = mkfifo("mkFifoVerison1", S_IRWXU);         // Create FIFO using mkfifo system call

    if (mkfifo_status == 0){
        printf("mkfifo file created.");
    }

   
    mknod_status  = mknod("myMknodFifoVerison2", __S_IFIFO | S_IRWXU, 0);    // Create FIFO file using mknod system call

    if (mknod_status == 0){
        printf("mknod file created.");
    }
}