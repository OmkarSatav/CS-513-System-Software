/*============================================================================
Name : 31b.c
Author : Omkar Satav
Description : Write a program to create a semaphore and initialize value to the semaphore. 
              b. Create a counting semaphore
Date: 21th Sept, 2024.
============================================================================*/


#include <stdio.h>       // For using Standard Input Output functions like printf().
#include <stdlib.h>      // For using exit().
#include <sys/types.h>   // For using data types in IPC.
#include <sys/ipc.h>     // For generating unique keys.
#include <sys/sem.h>     // For using semget() and semctl().

int main() {
    key_t key = ftok("semfile", 'O'); 
    int semaphoreId = semget(key, 1, IPC_CREAT | 0666);       // Create a semaphore set with 1 semaphore
    
    semctl(semaphoreId, 0, SETVAL, 5);                       // Initialize the semaphore value to 5.
    
    printf("Counting semaphore created and it is initialized to 5.\n");
    return 0;
}



/*


Counting semaphore created and it is initialized to 5.


*/
