/*======================================================================================================
Name : 34b_clt.c
Author : Omkar Satav
Description : Write a program to create a concurrent server.
              b. use pthread_create
Date: 21st Sept, 2024.
========================================================================================================
*/


#include <sys/types.h>      // For using sockets data types.
#include <sys/socket.h>     // For using socket(), connect(), send(), and recv().
#include <netinet/in.h>     // For sockaddr_in structure.
#include <unistd.h>         // For using close().
#include <stdio.h>          // For using standard input/output functions like printf().
#include <stdlib.h>         // For using exit().

int main() {
    int sockfd;                               // Socket file descriptor
    struct sockaddr_in servaddr;              // Server address structure

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket
    if (sockfd < 0) {                         // Check for socket creation errors
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;            // Set address family to IPv4
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Connect to localhost
    servaddr.sin_port = htons(8080);          // Set port number

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) { // Connect to server
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    const char *message = "New Student Enrolled"; // Message to send to server
    write(sockfd, message, strlen(message));       // Send message to server

    char buff[1024];                               // Buffer for receiving response
    read(sockfd, buff, sizeof(buff));              // Receive response from server
    printf("Server: %s\n", buff);                  // Print server response

    close(sockfd);                                 // Close socket
    return 0;                                      // Exit successfully
}
