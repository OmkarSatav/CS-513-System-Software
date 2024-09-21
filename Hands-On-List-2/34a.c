/*======================================================================================================
Name : 34a.c
Author : Omkar Satav
Description : Write a program to create a concurrent server.
              a. use fork.
Date: 21st Sept, 2024.
========================================================================================================
*/

#include <sys/types.h>     // For using socket and fork types.
#include <sys/socket.h>    // For using socket(), bind(), listen(), accept().
#include <netinet/in.h>    // For using sockaddr_in.
#include <stdio.h>         // For using Standard Input Output Functions like printf().
#include <unistd.h>        // For using read(), write(), close(), and fork() system calls.
#include <stdlib.h>        // For using exit() function.


int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);                             // Create server socket.
    struct sockaddr_in serverAddress = {AF_INET, htons(8080), htonl(INADDR_ANY)};  // IPv4, port 8080, and any incoming IP.

    bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));  // Link the socket to the specified address and port.
    listen(serverSocket, 5);                                          // Start listening for client connections, with a maximum backlog of 5.
    printf("Server is running...\n");

    while (1) {
        int clientSocket = accept(serverSocket, NULL, NULL);           // Accept an incoming client connection.
        if (fork() == 0) {                                             // Fork a new process to handle the client communication.
            close(serverSocket);                                       // Child process closes server socket as it's not needed.
            char buffer[256] = {0};
            read(clientSocket, buffer, sizeof(buffer) - 1);            // Read the message from the client 
            printf("Client: %s\n", buffer);                           
            write(clientSocket, "Message received by the server\n", 31); // Respond back to the client.
            close(clientSocket);                                       
            exit(0);                                                   // Child process terminates.
        }
        close(clientSocket);                                           // Parent process closes the client socket.
    }

    close(serverSocket);                                               // Close the server socket.
    return 0;
}



/*

----------------- Server Process -----------------

Server is running...
Client: 

Hello
Client: Hello

Nice meeting
Client: Pleasure is mine




--------------- Client Process ------------------

Assignment34$ telnet localhost 8080
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Hello
Message received by the server
Connection closed by foreign host.
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList2/Assignment34$ telnet localhost 8080
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Pleasure is mine
Message received by the server
Connection closed by foreign host.


*/


