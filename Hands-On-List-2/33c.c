/*============================================================================
Name : 33a.c
Author : Omkar Saatav
Description : Write a program to communicate between two machines using socket.
Date: 21th Sept, 2024.
============================================================================*/

#include <sys/types.h>   // For using data types for socket.
#include <sys/socket.h>  // For using socket(), connect().
#include <unistd.h>      // For using close() function.
#include <stdio.h>       // For using Standard Input Output functions like printf().
#include <arpa/inet.h>   // For using internet addresses and socket functions.


int main() {
    struct sockaddr_in serveraddr;
    int skt = socket(AF_INET, SOCK_STREAM, 0);           // Create socket for TCP
    char buff[80];

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Change IP as needed
    serveraddr.sin_port = htons(9999);

    connect(skt, (void *)(&serveraddr), sizeof(serveraddr);  

    write(skt, "Hello IIITB, I am new MTECH student.\n", 41);        // Send message to server
    read(skt, buff, sizeof(buff));                                   // Read response from server
    printf("Server End : %s", buff); 

    close(skt);
    return 0;
}


/*


Client End: Hello IIITB, I am new MTECH student.

Server End : Welcome to IIITB


*/
