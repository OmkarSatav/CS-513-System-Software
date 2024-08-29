/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: /* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a program to open a file, duplicate the file descriptor and append the file with both the
                    descriptors and check whether the file is updated properly or not.
                    a. use dup
                    b. use dup2
                    c. use fcntl
 * */


#include <stdio.h>   // For standard input output functions like printf().
#include <fcntl.h>   // For file operations like open().
#include <unistd.h>  // For dup(), dup2() and write() functions.
#include <string.h>  // For string functions like strlen().
#include <stdlib.h>  // For rand() function.

int main() {
    int fileDescriptor, duplicateFileDescriptor, duplicateFileDescriptor2, fileDescriptor_fcntl;
    char *fileN = "duplicateFDs.txt";
    char dataSequence1[] = "Data written by original fileDescriptor.\n";
    char dataSequence2[] = "Data written by duplicated fileDescriptor.\n";
    char dataSequence3[] = "Data written by duplicated fileDescriptor version 2.\n";
    char dataSequence4[] = "Data written by fcntl fileDescriptor.\n";


    fileDescriptor = open(fileN, O_CREAT | O_APPEND | O_WRONLY, 0770);
    if (fileDescriptor < 0) {
        printf("Unable to open or create the file");
        return 1;
    }


    duplicateFileDescriptor = dup(fileDescriptor);  // Duplicate the original file descriptor.
    if (duplicateFileDescriptor < 0) {
        printf("dup() operation failed");
        close(fileDescriptor);
        return 1;
    }


    duplicateFileDescriptor2 = dup2(fileDescriptor, duplicateFileDescriptor + (rand() % (150 - 1 + 1)) + 1);   // Duplicate the original file descriptor, and assigning random value for fd.
    if (duplicateFileDescriptor2 < 0) {
        printf("dup2() operation failed");
        close(fileDescriptor);
        close(duplicateFileDescriptor);
        return 1;
    }

    if (write(fileDescriptor, dataSequence1, strlen(dataSequence1)) != strlen(dataSequence1)) {      // Write dataSequence1 using fileDescriptor.
        printf("Failed to write with original file descriptor");
        close(fileDescriptor);
        close(duplicateFileDescriptor);
        close(duplicateFileDescriptor2);
        return 1;
    }

    if (write(duplicateFileDescriptor, dataSequence2, strlen(dataSequence2)) != strlen(dataSequence2)) {   // Write dataSequence2 using duplicateFileDescriptor.
        printf("Failed to write with duplicated file descriptor (dup)");
        close(fileDescriptor);
        close(duplicateFileDescriptor);
        close(duplicateFileDescriptor2);
        return 1;
    }

    if (write(duplicateFileDescriptor2, dataSequence3, strlen(dataSequence3)) != strlen(dataSequence3)) {   // Write dataSequence3 using duplicateFileDescriptor2.
        printf("Failed to write with duplicated file descriptor (dup2)");
        close(fileDescriptor);
        close(duplicateFileDescriptor);
        close(duplicateFileDescriptor2);
        return 1;
    }


    fileDescriptor_fcntl = fcntl(fileDescriptor, F_DUPFD, duplicateFileDescriptor2 + 1);                 // Create a duplicate file descriptor using fcntl "F_DUPFD"
     if (write(fileDescriptor_fcntl, dataSequence4, strlen(dataSequence4)) != strlen(dataSequence4)) {   // Write dataSequence4 using FCNTL file descriptor.
        printf("Failed to write with duplicated file descriptor (dup2)");
        close(fileDescriptor);
        close(duplicateFileDescriptor);
        close(duplicateFileDescriptor2);
        close(fileDescriptor_fcntl);
        return 1;
    }


    close(fileDescriptor);
    close(duplicateFileDescriptor);
    close(duplicateFileDescriptor2);
    close(fileDescriptor_fcntl);

    printf("File updated successfully.\n");
    return 0;
}



/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment11$ gcc DuplicateDescriptor.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment11$ ./a.out
File updated successfully.
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment11$ 

File Content:
Data written by original fileDescriptor.
Data written by duplicated fileDescriptor.
Data written by duplicated fileDescriptor version 2.
Data written by fcntl fileDescriptor.

*/