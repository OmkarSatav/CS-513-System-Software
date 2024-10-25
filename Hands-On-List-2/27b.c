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


int main() {
    key_t key = ftok("mq", 1);  
    int msgqueid = msgget(key, 0);  
    struct msgbuf {
        long mtype;                                 // Message type
        char mtext[80];
    }msg1;
                            // Get the message queue identifier
    struct msgbuf message;

    msgrcv(msgqueid, &msg1, sizeof(msg1.mtext), 2, IPC_NOWAIT);

    printf("Message Type: %ld\n", msg1.mtype);          
    printf("Message: %s\n", msg1.mtext);  

    return 0;
}



/*


Message Type: 2
Message: Greetings, this is the second message with mtype 2


*/