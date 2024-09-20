/*======================================================================================================
Name : 32c.c
Author : Omkar Satav
Description : Write a program to implement semaphore to protect any critical section.
              c. protect multiple pseudo resources ( may be two) using counting semaphore
Date: 20th Sept, 2024.
========================================================================================================
*/

#include <sys/ipc.h>     // For using ftok().
#include <sys/sem.h>     // For semaphore operations like semget(), semctl(), and semop().
#include <stdio.h>       // For using Standard Input Output Functions like printf().
#include <fcntl.h>       // For using open(), O_RDWR and O_CREAT.
#include <unistd.h>      // For using lseek() and close()

union semun {
    int val;     
};

struct ticketStruct {
    int tktNo; 
};

int main() {
    key_t key = ftok("countingSem", 435);         
    int semaphoreId = semget(key, 1, IPC_CREAT | 0774); // Create semaphore
    union semun arg = {1};                  // Counting Semaphore (2 resources).

    semctl(semaphoreId, 0, SETVAL, arg);

    int fileDescript = open("database",O_CREAT | O_RDWR, 0774); // Open file for resources
    struct ticketStruct tkt = {156};                            // Assign value

    for (int i = 0; i < 5; i++) {
        struct sembufff bufff = {1, -1, 0};        // Lock semaphore
        semop(semaphoreId, &bufff, 1);             // Enter critical section for ticket resource
        printf("Ticket Resource accessed by %d iteration. ", i+1);
        tkt.tktNo++;

        semop(semaphoreId, &bufff, 1);             // Enter critical section for file resource
        printf("File Resource accessed by %d iteration.\n", i+1);
        lseek(fileDescript, 0, SEEK_SET);             // Move to start of file
        write(fileDescript, &tkt, sizeof(tkt));       // Write resources to file

        bufff.sem_op = 1;                     // Unlock semaphore
        semop(semaphoreId, &bufff, 1);             // Exit critical section
        semop(semaphoreId, &bufff, 1); 
    }

    close(fileDescript);                              
    return 0;
}


/*


Ticket Resource accessed by 1 iteration. File Resource accessed by 1 iteration.
Ticket Resource accessed by 2 iteration. File Resource accessed by 2 iteration.
Ticket Resource accessed by 3 iteration. File Resource accessed by 3 iteration.
Ticket Resource accessed by 4 iteration. File Resource accessed by 4 iteration.
Ticket Resource accessed by 5 iteration. File Resource accessed by 5 iteration.


*/