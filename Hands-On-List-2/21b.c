/*============================================================================
Name : 21b.c
Author : Omkar Satav
Description : Write two programs so that both can communicate by FIFO -Use two way communications.
Date: 19th Sept, 2024.
============================================================================
*/


#include <stdio.h>    // For using Standard Input Output Functions like printf() and fgets().
#include <fcntl.h>    // For using open()
#include <unistd.h>   // For using read(), write(), and close()
#include <string.h>   // For using strlen().

int main() {
    int writeFd, readFd;                    // File descriptors for FIFO
    char message[150];                      // Buffer to store messages

    writeFd = open("clt", O_WRONLY);        // Open FIFO files: clt for writing, srv for reading
    readFd = open("srv", O_RDONLY);

    printf("Enter message to send: ");
    fgets(message, sizeof(message), stdin);   
    write(writeFd, message, strlen(message) + 1);  // Send message through clt

    read(readFd, message, sizeof(message));   // Read response from srv
    printf("Reader's response: %s\n", message);

    close(writeFd);                           // Close FIFO files
    close(readFd);

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