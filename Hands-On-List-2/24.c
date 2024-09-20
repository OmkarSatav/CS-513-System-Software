/*============================================================================
Name : 24.c
Author : Omkar Satav
Description : Write a program to create a message queue and print the key and message queue id
Date: 19th Sept, 2024.
============================================================================
*/

#include <stdio.h>      // For using Standard Input Output Functions like printf().
#include <sys/ipc.h>    // For IPC_CREAT, ftok(), key_t.
#include <sys/msg.h>    // For msgget(), msgctl().

int main() {
    key_t key;               // Unique key for message queue
    int msgQueId;            // Message queue ID

    
    key = ftok("prgF", 'O');  // Generate a unique key using ftok (file path and project identifier).

    msgQueId = msgget(key, IPC_CREAT | 0777);   // Create the message queue and return its ID

    printf("Message Queue Key: %d\n", key);
    printf("Message Queue ID: %d\n", msgQueId);

    return 0;
}



/*


Message Queue Key: 1325619575
Message Queue ID: 0


*/
