/*============================================================================
Name : 20w.c
Author : Omkar Satav
Description : Write two programs so that both can communicate by FIFO - Use one way communication.
Date: 12th Sept, 2024.
============================================================================
*/


#include <fcntl.h>    // For using open() and O_WRONLY.
#include <stdio.h>    // For using Standard Input Output Functions like printf() and scanf().
#include <unistd.h>   // For using write() and close().


int main() {
    int fd = open("./communicationPath", O_WRONLY);  // Open the FIFO in write mode
    char buffer[20];

    printf("Enter the text : ");  
    scanf("%[^\n]", buffer);  
    write(fd, buffer, sizeof(buffer));  // Write input to the FIFO
    close(fd); 
    return 0;
}


/*


Assignment20$ ./read.out
Received: Hello World!!


Assignment20$ ./write.out
Enter the text : Hello World!!


*/