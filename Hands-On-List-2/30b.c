/*======================================================================================================
Name : 30b.c
Author : Omkar Satav
Description : Write a program to create a shared memory.
              b. attach with O_RDONLY and check whether you are able to overwrite.
Date: 20th Sept, 2024.
========================================================================================================
*/

#include <stdio.h>        // For using Standard Input Output Functions like printf().
#include <sys/ipc.h>      // For using key_t and IPC_CREAT.
#include <sys/shm.h>      // For using shared memory functions like shmget(), shmat(), shmdt().
#include <string.h>       // For using strcpy().


int main() {
    key_t key = ftok("sharedMmy", 0); 
    int sharedMmyId = shmget(key, 256, IPC_CREAT | 0744);  // Create shared memory segment (512 bytes)
    char *data;

    data = shmat(sharedMmyId, 0, SHM_RDONLY);              // Link in read-only mode

    printf("Data to add in shared memory: ");
    scanf("%s", data);                                     // Trying to overwrite. But will fail.

}



/*


Data to add in shared memory: Hello
Segmentation fault (core dumped)


*/