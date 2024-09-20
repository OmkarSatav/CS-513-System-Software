/*======================================================================================================
Name : 28.c
Author : Omkar Satav
Description : Write a program to change the exiting message queue permission. (use msqid_ds structure).
Date: 19th Sept, 2024.
========================================================================================================
*/


#include <sys/types.h>   // For using key_t and IPC_CREAT.
#include <sys/ipc.h>     // For using ftok() and msgget().
#include <sys/msg.h>     // For using msgget(), msgctl(), and msqid_ds.
#include <stdio.h>       // For using Standard Input Output Functions like printf().

int main() {
    key_t key = ftok("msgQue", 1);                                                   // Generate a unique key

    int msgQueId = msgget(key, 0);                                                   // Get the existing message queue ID

    struct msqid_ds msgQueDataStruct;                                                // Define the message queue data structure
    
    msgctl(msgQueId, IPC_STAT, &msgQueDataStruct);                                   // Get the current message queue information

    printf("Current permissions: %o\n", msgQueDataStruct.msg_perm.mode & 0777);      // Print current permissions

    msgQueDataStruct.msg_perm.mode = 0660;                                           // Modify the permissions

    msgctl(msgQueId, IPC_SET, &msgQueDataStruct);                                    // Set the new permissions

    printf("Updated permissions: %o\n", msgQueDataStruct.msg_perm.mode & 0777);      // Print new permissions

    return 0;
}



/*


Current permissions: 0
Updated permissions: 660


*/