/* 
==========================================================================
Name: 16.c
Author: Omkar Dhananjay Satav
Description :  Write a program, call fork and print the parent and child process id.
Date : 28th Aug, 2024
===========================================================================
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argumentCount, char *argumentValues[]) {
    struct flock writeLock, readLock;
    const char *file1, *file2;
    const int buffSize = 1024;     // Size of buffer
    char buff[buffSize];           // Buffer to store input
    int bytesRd;


   if (argumentCount != 3){
        file1 = "readingFile.txt";       // If command line input is not given, default file is created.
        file2 = "destination.txt";
    }else{
        file1 = argumentValues[1];  // Input file names from command line argument.
        file2 = argumentValues[2];
    }

    int inputfileDescriptor = open(file1, O_CREAT | O_RDWR, 0777); 
    int outputfileDescriptor = open(file2, O_CREAT | O_RDWR, 0777); // File is created if not existed. If file already existed, open() function fails.
    if (inputfileDescriptor == -1 || outputfileDescriptor == -1) {
        printf("Failed to open a file.\n");
    }

    // Initialize the writeLock structure for a write writeLock
    writeLock.l_type = F_WRLCK;        // Set to write lock
    writeLock.l_whence = SEEK_SET;     // Set lock from start of file
    writeLock.l_start = 0;             // Move pointer to start of file.
    writeLock.l_len = 0;               // 0 means writeLock the entire file
    writeLock.l_pid = getpid();        // owner's id is assigned

    
    printf("Applying write lock on file2.\n");
    if (fcntl(outputfileDescriptor, F_SETLKW, &writeLock) == -1) {
        perror("Failed to acquire lock");
        close(outputfileDescriptor);
        return 1;
    }

    readLock.l_type = F_RDLCK; // Set readLock type to read readLock
    readLock.l_whence = SEEK_SET;
    readLock.l_start = 0;   // Start of the file
    readLock.l_len = 0;     // readLock the whole file
    readLock.l_pid = getpid();

    printf("Applying read lock on file1.\n");
    if (fcntl(inputfileDescriptor
    , F_SETLKW, &readLock) == -1) {
        perror("Failed to apply read lock");
        return 1;
    }

  
    while((bytesRd = read(inputfileDescriptor, buff, buffSize))>0){      // Read input from file1.
        write(outputfileDescriptor, buff, bytesRd);           // Write the data from file1 to file2
    }  


    
    writeLock.l_type = F_UNLCK;                               // Release the writeLock
    if (fcntl(outputfileDescriptor, F_SETLK, &writeLock) == -1) {
        perror("Failed to release writeLock");
        close(outputfileDescriptor);
        return 1;
    }

    
    readLock.l_type = F_UNLCK;                              // Release the readLock
    if (fcntl(inputfileDescriptor, F_SETLK, &readLock) == -1) {
        perror("Failed to unlock file");
        return 1;
    }


    printf("End\n");
    close(inputfileDescriptor);
    close(outputfileDescriptor);
    return 0;
}



/*
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment16$ gcc ReadWriteLock.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment16$ ./a.out readingFile.txt 
Applying write lock on file2.
Applying read lock on file1.
End


destination.txt file:
Embrace the journey, for it's in the challenges that we find our true strength.


 */