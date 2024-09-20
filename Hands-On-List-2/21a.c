/*============================================================================
Name : 21a.c
Author : Omkar Satav
Description : Write two programs so that both can communicate by FIFO -Use two way communications.
Date: 19th Sept, 2024.
============================================================================
*/


#include <stdio.h>    // For using Standard Input Output Functions like printf() and fgets().
#include <fcntl.h>    // For using open()
#include <unistd.h>   // For using read(), write(), and close().

int main() {
    int readFd, writeFd;                    // File descriptors for FIFO
    char message[150];                      // Buffer to store messages

    readFd = open("clt", O_RDONLY);         // Open FIFO files: clt for reading, srv for writing
    writeFd = open("srv", O_WRONLY);
   

    read(readFd, message, sizeof(message));   // Read message from clt
    printf("Writer's message: %s", message);


    printf("Enter response: ");
    fgets(message, sizeof(message), stdin);   
    write(writeFd, message, strlen(message) + 1);  // Send response through srv

    
    close(readFd);                              // Close FIFO files
    close(writeFd);

    return 0;
}



/*


omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList2/Assignment21$ ./usr2
Enter message to send: Welcome to IIITB 
Reader's response: Thank You


omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList2/Assignment21$ ./usr1
Writer's message: Welcome to IIITB
Enter response: Thank You


*/


