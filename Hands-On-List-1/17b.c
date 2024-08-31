/* 
==========================================================================
Name: 17b.c
Author: Omkar Dhananjay Satav
Description :  Write a program to simulate online ticket reservation. Implement write lock
               Write a program to open a file, store a ticket number and exit. Write a separate program, to
               open the file, implement write lock, read the ticket number, increment the number and print
               the new ticket number then close the file.
Date : 31th Aug, 2024
===========================================================================
*/

#include <stdio.h>  // For Standard Input Output Functions like printf().
#include <fcntl.h>  // For opening a file - open().
#include <unistd.h> // For using functions like read(), write() and close().

int main() {

    struct {
        int ticket_no;
    } database;

    struct flock updateLock;
    int fileDescript;
    fileDescript = open("database", O_RDWR);
 
    fcntl(fileDescript, F_SETLKW, &updateLock);  // Setting and Waiting for a lock on database file.
   
    read(fileDescript, &database, sizeof(database));        // Reading the content in the database file.

    printf("Ticket number: %d\n", database.ticket_no); 
    database.ticket_no++;                           // Incrementing ticket number.
   
    lseek(fileDescript, 0, SEEK_SET);               // Move the pointer to start of file. To overwrite the content.
    write(fileDescript, &database, sizeof(database));    // Overwrite the content.


    
    updateLock.l_type = F_UNLCK;                  // Unlock the file lock.
    fcntl(fileDescript, F_SETLK, &updateLock);

}



/*


omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment18$ gcc Store.c -o store.out
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment18$ gcc Update.c -o update.out
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment18$ ./store.out 106
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment18$ ./update.out 
Ticket number: 106
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment18$ ./update.out 
Ticket number: 107
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment18$ 


*/