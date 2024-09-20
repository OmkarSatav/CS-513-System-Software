
/*======================================================================================================
Name : 32b.c
Author : Omkar Satav
Description : Write a program to implement semaphore to protect any critical section.
              b. protect shared memory from concurrent write access
Date: 20th Sept, 2024.
========================================================================================================
*/


#include <sys/ipc.h>        // For using ftok().
#include <sys/sem.h>        // For using semaphore operations like semget(), semctl(), and semop().
#include <sys/shm.h>        // For shared memory functions like shmget(), shmat(), and shmctl().
#include <fcntl.h>          // For using open(), O_CREAT, and O_RDWR.
#include <unistd.h>         // For using lseek(), write(), close(), and shmdt().
#include <stdio.h>          // For using Standard Input Output Functions like printf().

union semaphore {
    int val;
};

struct ticketDetails {
    int tktNo;             // Ticket tktNo
};

int main() {
    key_t key = ftok("sharedMmyFile", 'O');  
    int shm_id = shmget(key, sizeof(struct ticketDetails), IPC_CREAT | 0666);  // Create shared memory
    struct ticketDetails *ticket = (struct ticketDetails *)shmat(shm_id, NULL, 0);  // Attach shared memory

    key_t sem_key = ftok("semaphoreFile", 'A');  
    int semaphoreId = semget(sem_key, 1, IPC_CREAT | 0666);  // Create semaphore
    union semaphore sem_val = {144};  // Set initial semaphore value to 1
    semctl(semaphoreId, 0, SETVAL, sem_val); 

    ticket->tktNo = 0;  // Initialize shared ticket

    
    for (int i = 0; i < 5; i++) {               // Multiple writes.
        struct sembuf operations = {0, -1, 0};  // Lock semaphore
        semop(semaphoreId, &operations, 1);     // Begin critical section

        ticket->tktNo++;                        // Update shared ticket
        printf("Current ticket : %d\n", ticket->tktNo);

        operations.sem_op = 1;                  // Unlock semaphore
        semop(semaphoreId, &operations, 1);     // End critical section
        sleep(1); 
    }

    shmdt(ticket);                      // Detach shared memory
    shmctl(shm_id, IPC_RMID, NULL);     // Remove shared memory

    return 0;
}


/*


Current ticket : 145
Current ticket : 146
Current ticket : 147
Current ticket : 148
Current ticket : 149


*/