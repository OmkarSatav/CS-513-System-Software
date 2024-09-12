/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write two programs so that both can communicate by FIFO - Use one way communication.
 * */


#include <fcntl.h>         // For using open() and O_RDONLY.
#include <stdio.h>         // For using Standard Input Output Functions like printf() and scanf().
#include <unistd.h>        // For using read() and close().

int main() {
    int fileDescript = open("./communicationPath", O_RDONLY);  // Open the FIFO in read mode
    char buffer[20];

    read(fileDescript, buffer, sizeof(buffer));  // Read data from the FIFO
    printf("Received: %s\n", buffer); 
    close(fileDescript);                         // Close the FIFO after reading
    return 0;
}


/*


Assignment20$ ./read.out
Received: Hello World!!


Assignment20$ ./write.out
Enter the text : Hello World!!


*/