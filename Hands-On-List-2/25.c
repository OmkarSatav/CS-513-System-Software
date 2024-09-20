/*======================================================================================================
Name : 25.c
Author : Omkar Satav
Description : Write a program to print a message queue's (use msqid_ds and ipc_perm structures)
                a. access permission
                b. uid, gid
                c. time of last message sent and received
                d. time of last change in the message queue
                d. size of the queue
                f. number of messages in the queue
                g. maximum number of bytes allowed
                h. pid of the msgsnd and msgrcv
Date: 20th Sept, 2024.
========================================================================================================
*/

#include <sys/types.h>     // For data types used in IPC.
#include <sys/ipc.h>       // For IPC key generation.
#include <sys/msg.h>       // For message queue functions and structures.
#include <stdio.h>         // For using Standard Input Output Functions like printf().
#include <time.h>          // For using ctime().
#include <stdlib.h>        // For using exit().

int main() {
    key_t key = ftok("msg", 'o');             
    int msgqueid = msgget(key, 0);              // Get the message queue ID

    struct msqid_ds msgque;                     // Structure to hold message queue information

    msgctl(msgqueid, IPC_STAT, &msgque);        // Retrieve the status of the message queue

    printf("Access Permissions: %03o\n", msgque.msg_perm.mode & 0777); // Access permissions in octal format.
    printf("Effective UID of Owner: %d\n", msgque.msg_perm.uid);      // UID of the owner
    printf("Effective GID of Owner: %d\n", msgque.msg_perm.gid);      // GID of the owner
    printf("Effective UID of Creator: %d\n", msgque.msg_perm.cuid);   // UID of the creator
    printf("Effective GID of Creator: %d\n", msgque.msg_perm.cgid);   // GID of the creator
    
    printf("Time of last msgsnd: %s", ctime(&msgque.msg_stime));     // Time of last message sent
    printf("Time of last msgrce: %s", ctime(&msgque.msg_rtime)); // Time of last message received
    printf("Time of last change: %s", ctime(&msgque.msg_ctime)); // Time of last change
    
    printf("Current number of message in queue: %lu\n", msgque.msg_qnum);      // Number of messages in the queue
    printf("Maximum number of bytes allowed in queue:  %lu\n", msgque.msg_qbytes); // Maximum bytes allowed in the queue
    
    printf("PID of last msgsnd: %d\n", msgque.msg_lspid);           // PID of last sender
    printf("PID of last msgrce: %d\n", msgque.msg_lrpid);           // PID of last receiver

    return 0;  // Exit successfully
}


/*


Access Permissions: 000
Effective UID of Owner: 0
Effective GID of Owner: 442560432
Effective UID of Creator: 9175848
Effective GID of Creator: 0
Time of last msgsnd: Thu Jan  1 05:30:00 1970
Time of last msgrce: Thu Jan  1 05:30:00 1970
Time of last change: Thu Jan  1 05:30:00 1970
Current number of message in queue: 0
Maximum number of bytes allowed in queue:  0
PID of last msgsnd: 0
PID of last msgrce: 0


*/