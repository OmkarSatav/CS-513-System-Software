/*============================================================================
Name : 33b.c
Author : Omkar Satav
Description : Write a program to communicate between two machines using socket.
Date: 21th Sept, 2024.
============================================================================*/


#include <sys/types.h>   // For using data types.
#include <sys/socket.h>  // For using socket(), bind(), listen().
#include <unistd.h>      // For using close() function.
#include <stdio.h>       // For using Standard Input Output functions like printf().
#include <arpa/inet.h>   // For using internet addresses.


int main() {
    struct sockaddr_in serveraddr, clientaddr;
    int servskt = socket(AF_INET, SOCK_STREAM, 0);            // Create socket for TCP
    int cl_size;
    int cliskt;
    char buff[80];

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;                  // Accept connections from any IP
    serveraddr.sin_port = htons(9999);

    bind(servskt, (void *)(&serveraddr), sizeof(serveraddr)); // Bind socket to address
    listen(servskt, 5);                                       // Listen for incoming connections
    cl_size = sizeof(clientaddr);
    cliskt = accept(servskt, (void *)(&clientaddr), &cl_size);   // Accept a client connection

    read(cliskt, buff, sizeof(buff));                       // Read message from client
    printf("Client End: %s", buff);                
    write(cliskt, "Welcome to IIITB\n", 19);                // Send response to client

    close(cliskt); 
    close(servskt);
    return 0;
}


/*


Client End: Hello IIITB, I am new MTECH student.

Server End : Welcome to IIITB


*/

