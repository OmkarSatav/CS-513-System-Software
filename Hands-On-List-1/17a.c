/* 
==========================================================================
Name: 17a.c
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


int main(int argumentCount, char *argumentValues[]) {
   
    int fileDescript;

    struct {                           // Ticket Structure
        int tktNo;
    } database;


    if (argumentCount != 2){
        database.tktNo = 106;       // If command line input is not given, default file is created.
    }else{
        database.tktNo = atoi(argumentValues[1]);  // Input file names from command line argument.
    }

    fileDescript = open("database", O_CREAT|O_RDWR|O_APPEND, 0766);    // Open a file

    write(fileDescript, &database, sizeof(database));     // Write the database structure to the file.

    close(fileDescript);
}