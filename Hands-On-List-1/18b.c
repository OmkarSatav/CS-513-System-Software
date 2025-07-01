/* 
==========================================================================
Name: 18b.c
Author: Omkar Dhananjay Satav
Description :  Write a program to perform Record locking.
                    a. Implement write lock
                    b. Implement read lock
                    Create three records in a file. Whenever you access a particular record, first lock it then modify/access
                    to avoid race condition.
Date : 31th Aug, 2024
===========================================================================
*/

#include <stdio.h>    // For Standard Input Output Functions like printf().
#include <fcntl.h>    // For opening a file - open().
#include <unistd.h>   // For using functions like read(), write() and close().
#include <sys/types.h> // For types like off_t
#include <sys/stat.h>  // For file permissions
#include <stdlib.h>    // For exit()

int main() {
    int fileDescript, ticketNo;
    struct {                           // Ticket Structure
        int trainNo;
        int tktCnt;
    } database[3];

    fileDescript = open("bookings.txt", O_RDWR);
    if (fileDescript < 0) {
        perror("Unable to open file");
        exit(1);
    }

    printf("Select train number: (1 to 3)\n");
    scanf("%d", &ticketNo);

    struct flock updateLock;             // Setting lock for the specific record.
    updateLock.l_whence = SEEK_SET;
    updateLock.l_type = F_WRLCK;
    updateLock.l_len = sizeof(database[0]);
    updateLock.l_start = (ticketNo - 1) * sizeof(database[0]);
    updateLock.l_pid = getpid();

    lseek(fileDescript, updateLock.l_start, SEEK_SET);    // Move pointer to that record.

    fcntl(fileDescript, F_SETLKW, &updateLock);          // Set the lock to the specific record.

    read(fileDescript, &database[ticketNo - 1], sizeof(database[0]));     // Read the record.

    printf("Current ticket count: %d\n", database[ticketNo-1].tktCnt);   // Get value of ticket count.
    database[ticketNo-1].tktCnt++;                       // Update the value ticket count.

    lseek(fileDescript, -1 * sizeof(database[0]), SEEK_CUR);      // Now move to start of that specific record.
    write(fileDescript, &database[ticketNo - 1], sizeof(database[0]));          // Overwrites content at that location.

    updateLock.l_type = F_UNLCK;                          // Unlock the lock.
    fcntl(fileDescript, F_SETLK, &updateLock);
    printf("Total tickets booked in this session : %d\n", database[ticketNo-1].tktCnt);

    close(fileDescript);
    return 0;
}



/*


omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment18$ gcc Store.c -o store.out
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment18$ gcc Update.c -o update.out
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment18$ ./store.out
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment18$ ./update.out
Select train number: (1 to 3)
2
Current ticket count: 1
Total tickets booked in this session : 2
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment18$ 


*/
