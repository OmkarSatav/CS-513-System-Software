/*======================================================================================================
Name : 34b_srv.c
Author : Omkar Satav
Description : Write a program to create a concurrent server.
              b. use pthread_create
Date: 21st Sept, 2024.
========================================================================================================
*/

#include <sys/types.h>      // For using sockets data types.
#include <sys/socket.h>     // For using socket(), bind(), listen(), accept()
#include <netinet/in.h>     // For sockaddr_in structure
#include <unistd.h>         // For using close(), read() and write().
#include <pthread.h>        // For using pthread_create().
#include <stdio.h>          // For using Standard Input Output Functions like printf().
#include <stdlib.h>         // For using malloc() and free().

void *connhandling(void *connfd_ptr) {
    int connectionfiledesc = *(int*)connfd_ptr; // Retrieve client socket descriptor
    free(connfd_ptr);              
    char buffer[256];                

    read(connectionfiledesc, buffer, sizeof(buffer)); // Read message from client
    printf("Client: %s\n", buffer);     
    write(connectionfiledesc, "Welcome to IIITB Server", 24); // Send response to client
    close(connectionfiledesc);                    
    return NULL;                      // Exit thread routine
}

int main() {
    int socketfiledesc, connectionfiledesc;               // Socket file descriptors
    struct sockaddr_in servaddr, cliaddr;         // Server and client address structures
    socklen_t climsglen;                             // Length of client address

    socketfiledesc = socket(AF_INET, SOCK_STREAM, 0);     // Create TCP socket
    servaddr.sin_family = AF_INET;                // Set address family to IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;        // Accept connections from any IP
    servaddr.sin_port = htons(8080);              // Set port number

    bind(socketfiledesc, (struct sockaddr*)&servaddr, sizeof(servaddr)); // Bind socket to address
    listen(socketfiledesc, 5);                          // Start listening for connections

    while (1) {
        climsglen = sizeof(cliaddr);               // Get size of the client address structure
        connectionfiledesc = accept(socketfiledesc, (struct sockaddr*)&cliaddr, &climsglen); // Accept new connection
        
        pthread_t thread;                       
        int *connfd_ptr = malloc(sizeof(int)); 
        *connfd_ptr = connectionfiledesc;        // Store the accepted connection descriptor
        
        pthread_create(&thread, NULL, connhandling, connfd_ptr); // Create thread to handle connection
        pthread_detach(thread);                 // Detach thread to free resources when done
    }

    close(socketfiledesc);                             // Close server socket
    return 0;
}


/*


Assignment34$ ./34b_srv
Client: New Student Enrolled



Assignment34$ ./a.out
Server: Welcome to IIITB Server

*/