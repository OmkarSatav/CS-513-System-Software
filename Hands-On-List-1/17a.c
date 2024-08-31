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