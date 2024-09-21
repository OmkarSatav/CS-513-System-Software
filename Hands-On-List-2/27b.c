/*============================================================================
Name : 27b.c
Author : Omkar Satav
Description : Write a program to receive messages from the message queue 
            b. With IPC_NOWAIT as a flag
Date: 19th Sept, 2024.
============================================================================
*/


#include <sys/ipc.h>   // For using ftok().
#include <sys/msg.h>   // For using message queue functions like msgget(), msgrcv().
#include <stdio.h>     // For using Standard Input Output Functions like printf().

struct msgbuf {
    long int mtype;
    char mtext[150];
};

int main() {
    key_t key = ftok("mq", 1);  
    int msgqueid = msgget(key, 0);                              // Get the message queue identifier
    struct msgbuf message;

    msgrcv(msgq_id, &msg1, sizeof(msg1.mtext), 10, IPC_NOWAIT);

    printf("Message Type: %ld\n", message.mtype);          
    printf("Message: %s\n", message.mtext);  

    return 0;
}



/*


Message Type: 10
Message: Hello from 26.c!


*/