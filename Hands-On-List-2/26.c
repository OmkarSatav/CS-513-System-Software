/*============================================================================
Name : 26.c
Author : Omkar Satav
Description : Write a program to send messages to the message queue. Check $ipcs -q.
Date: 19th Sept, 2024.
============================================================================
*/


#include <sys/types.h>  // For using key_t and IPC_CREAT.
#include <sys/ipc.h>    // For using ftok() and msgget().
#include <sys/msg.h>    // For using msgget() and msgsnd().
#include <stdio.h>      // For using Standard Input Output Functions like printf().

int main() {
    key_t key = ftok("msgQue", 'O');                // Generate a unique key for the message queue using the file "mq" and project identifier 1
   
    int msgQueId = msgget(key, IPC_CREAT | 0666);    // Create a message queue with read and write permissions

    struct msgBuffer {                              // Define the message buffer structure
        long mtype;                                 // Message type
        char mtext[80];                             // Message text
    };

    // Initialize two messages with different types
    struct msgBuffer message1 = {1, "Hello, this is the first message with mtype 1"};
    struct msgBuffer message2 = {2, "Greetings, this is the second message with mtype 2"};

    msgsnd(msgQueId, (void *)&message1, sizeof(message1.mtext), IPC_NOWAIT);     // Send the first message to the message queue

    msgsnd(msgQueId, (void *)&message2, sizeof(message2.mtext), IPC_NOWAIT);     // Send the second message to the message queue

    return 0;
}



/*


------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages    
0x4f035977 0          omkar      777        0            0           
0x4f035e6f 1          omkar      666        160          2  


*/