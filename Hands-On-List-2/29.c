/*======================================================================================================
Name : 29.c
Author : Omkar Satav
Description : Write a program to remove the message queue.
Date: 19th Sept, 2024.
========================================================================================================
*/


#include <sys/ipc.h>     // For using ftok() and msgget().
#include <sys/msg.h>     // For using msgget(), msgctl(), and msqid_ds.
#include <stdio.h>       // For using Standard Input Output Functions like printf().

int main() {
    key_t key = ftok("msgQue", 1);                
    int msgqueid = msgget(key, IPC_CREAT);       // Get or Create the message queue.

    msgctl(msgqueid, IPC_RMID, NULL);        // Remove the message queue.    

    return 0;
}
