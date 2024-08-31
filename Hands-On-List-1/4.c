/* 
==========================================================================
Name: 4.c
Author: Omkar Dhananjay Satav
Description : Write a program to open an existing file with read write mode. Try O_EXCL flag also.
Date : 13th Aug, 2024
===========================================================================
*/


#include <stdio.h>   // Standard Input Output Functions.
#include <fcntl.h>   // For file creation and opening - open()
#include <unistd.h>  // For closing fd resources - close()

int main(int argumentCount, int *argumentValues[]) {

    const char *file;

    if (argumentCount != 2){
        file = "origin.txt";       // If command line input is not given, default file is created.
    }else{
        file = argumentValues[1];  // Input file name from command line argument.
    }

    int fileDescriptor = open(file, O_CREAT | O_EXCL, 0777);  // File is created if not existed. If file already existed, open() function fails.
    if (fileDescriptor == -1) {
        printf("File already existed.\n");
    }

    int fileDescriptor2 = open(file, O_RDWR);     // Opening file in Read-Write Mode.
    printf("File opened in read-write mode %d\n",fileDescriptor2);

    close(fileDescriptor);
    close(fileDescriptor2);
    return 0;
}



/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment4$ gcc openFile.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment4$ ./a.out
File opened in read-write mode 4

*/