/*
==========================================================================
Name: 7.c
Author: Omkar Dhananjay Satav
Description : Write a program to copy file1 into file2 ($cp file1 file2).
Date : 27th Aug, 2024
===========================================================================
*/

#include <stdio.h>   // Standard Input Output Functions.
#include <fcntl.h>   // For file creation and opening - open()
#include <unistd.h>  // For closing fd resources - close()

int main(int argumentCount, char *argumentValues[]) {

    const char *file1, *file2;
    const int buffSize = 1024;     // Size of buffer
    char buff[buffSize];           // Buffer to store input
    int bytesRd;

    if (argumentCount != 3){
        file1 = "origin.txt";       // If command line input is not given, default file is created.
        file2 = "destination.txt";
    }else{
        file1 = argumentValues[1];  // Input file names from command line argument.
        file2 = argumentValues[2];
    }

    int inputfileDescriptor = open(file1, O_CREAT | O_RDWR); 
    int outputfileDescriptor = open(file2, O_CREAT | O_RDWR, 0777); // File is created if not existed. If file already existed, open() function fails.
    if (inputfileDescriptor == -1 || outputfileDescriptor == -1) {
        printf("Failed to open a file.\n");
    }

  
    while((bytesRd = read(inputfileDescriptor, buff, buffSize))>0){      // Read input from file1.
        write(outputfileDescriptor, buff, bytesRd);           // Write the data from file1 to file2
    }  

    close(inputfileDescriptor);
    close(outputfileDescriptor);
    return 0;
}



/*
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment7$ gcc CopyFile.c
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment7$ ./a.out readingFile.txt writingFile.txt
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment7$ cat writingFile.txt
Embrace the journey, for it's in the challenges that we find our true strength.
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment7$ cat readingFile.txt
Embrace the journey, for it's in the challenges that we find our true strength.
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment7$ 
*/