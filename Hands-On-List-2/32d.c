/*======================================================================================================
Name : 32d.c
Author : Omkar Satav
Description : Write a program to implement semaphore to protect any critical section.
              d. remove the created semaphore
Date: 20th Sept, 2024.
========================================================================================================
*/


#include <sys/ipc.h>     // For using ftok()
#include <sys/sem.h>     // For using semget() and semctl()
#include <stdio.h>       // For using Standard Input Output Functions like printf().

int main() {
    key_t key = ftok("semaphoreKey", 0);

    int semaphoreId = semget(key, 1, 0);  // Semaphore ID
    
    semctl(semaphoreId, 0, IPC_RMID);   // Remove the semaphore
    return 0; 
}
