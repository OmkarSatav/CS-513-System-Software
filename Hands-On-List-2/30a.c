/*======================================================================================================
Name : 30a.c
Author : Omkar Satav
Description : Write a program to create a shared memory.
              a. write some data to the shared memory
Date: 20th Sept, 2024.
========================================================================================================
*/


#include <stdio.h>        // For using Standard Input Output Functions like printf().
#include <sys/ipc.h>      // For using key_t and IPC_CREAT.
#include <sys/shm.h>      // For using shared memory functions like shmget(), shmat(), shmdt().
#include <string.h>       // For using strcpy().

int main() {
    key_t key = ftok("sharedMmy", 'O');                       
    int sharedMmyId = shmget(key, 1024, IPC_CREAT | 0774); // Create shared memory. Create 1024 bytes of shared memory

    
    char *dt = (char *)shmat(sharedMmyId, NULL, 0);  // Link to the shared memory 
    strcpy(dt, "Welcome to IIITB!");                 // Write dt
    
    return 0;
}
