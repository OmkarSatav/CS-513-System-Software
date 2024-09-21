/*============================================================================
Name : 31a.c
Author : Omkar Satav
Description : Write a program to create a semaphore and initialize value to the semaphore. 
              a. Create a binary semaphore 
Date: 21th Sept, 2024.
============================================================================*/

#include <stdio.h>       // For using Standard Input Output functions like printf().
#include <sys/types.h>   // For using data types in IPC.
#include <sys/ipc.h>     // For generating unique keys.
#include <sys/sem.h>     // For using semget() and semctl().

int main() {
    key_t key = ftok("semaphorefile", 'O');                           // Unique key for semaphore.
    int semaphoreId = semget(key, 1, IPC_CREAT | 0666);               // Create a semaphore set with 1 semaphore

    semctl(semaphoreId, 0, SETVAL, 1);                                // Initialize the semaphore value to 1 (binary semaphore)

    printf("Binary semaphore created and it is initialized to 1.\n");
    return 0;
}


/*


Binary semaphore created and it is initialized to 1.


*/
