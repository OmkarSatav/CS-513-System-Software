/*============================================================================
Name : 22.c
Author : Omkar Satav
Description : Write a program to wait for data to be written into FIFO within 10 seconds, use select
              system call with FIFO.
Date: 19th Sept, 2024.
============================================================================
*/


#include <stdio.h>       // For using Standard Input Output Functions like printf()
#include <fcntl.h>       // For using open()
#include <unistd.h>      // For using read(), close()
#include <sys/select.h>  // For using select() system call

int main() {
    int fifoFileDescript;
    char buff[150];                     // buff to store read data
    fd_set readFileDescript;            // Set of file descriptors for select()
    struct timeval timeout;             // Timeout for select()

    fifoFileDescript = open("sendData", O_RDONLY);   // Open FIFO for reading mode

    
    FD_ZERO(&readFileDescript);                    // Initialize fd_set and timeout, Clear the set
    FD_SET(fifoFileDescript, &readFileDescript);   // Add FIFO file descriptor to the set
    timeout.tv_sec = 10;                // Wait for 10 seconds
    timeout.tv_usec = 0;                // No microseconds

   
    int result = select(fifoFileDescript + 1, &readFileDescript, NULL, NULL, &timeout);   // Wait for data on the FIFO

    if (result > 0){
        if (FD_ISSET(fifoFileDescript, &readFileDescript)) { 
            read(fifoFileDescript, buff, sizeof(buff));
            printf("Data received: %s\n", buff);        
        }
    }

    close(fifoFileDescript);   // Close FIFO
    return 0;
}



/*


Data received: System Software.


*/