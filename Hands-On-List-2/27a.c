/*============================================================================
Name : 27a.c
Author : Omkar Satav
Description : Write a program to receive messages from the message queue 
            a.with 0 as a flag
Date: 21th Sept, 2024.
============================================================================
*/


#include <sys/ipc.h>   // For using ftok().
#include <sys/msg.h>   // For using message queue functions like msgget(), msgrcv()
#include <stdio.h>     // For using Standard Input Output Functions like printf()


int main() {
    key_t key = ftok("msgque", 1); 
    int msgqueid = msgget(key, 0);                             // Access existing message queue
    struct msgbuf {
        long int msgtype;   
        char msgtext[80];   
    } msg1;

    msgrcv(msgqueid, &msg1, sizeof(msg1.msgtext), 10, 0);     // Receive a message from the message queue with flag 0

    printf("\nMessage type: %ld", msg1.mtype);
    printf("\nMessage: %s", msg1.msgtext);
    
    return 0;
}



/*


Message Type: 10
Message: Hello from 26.c!


*/