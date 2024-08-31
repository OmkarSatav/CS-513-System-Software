/*
==========================================================================
Name: 10.c
Author: Omkar Dhananjay Satav
Description : Write a program to open a file with read write mode, write 10 bytes, move the file pointer by 10
              bytes (use lseek) and write again 10 bytes.
                a. check the return value of lseek
                b. open the file with od and check the empty spaces in between the data.
Date : 20th Aug, 2024
===========================================================================
*/


#include <stdio.h>  // Standard Input Output Functions.
#include <fcntl.h>  // For file creation and opening - open()
#include <unistd.h> // write() used from unistd to write content to the standard output.


int main(int argumentCount, char *argumentValues[]) {

    const char *file1;

    if (argumentCount != 2){
        file1 = "origin.txt";       // If command line input is not given, default file is created.
    }else{
        file1 = argumentValues[1];  // Input file names from command line argument.
    }

    int fileDescriptor;
    int offset;
    char dataSequence1[] = "Bangalore ";
    char dataSequence2[] = "International Institute";

    fileDescriptor = open(file1, O_RDWR | O_CREAT | O_TRUNC, 777);      // Creating and Opening a file.
    if (fileDescriptor < 0) {
        printf("Unable to create or open the file");
        return 1;
    }

    
    if (write(fileDescriptor, dataSequence1, sizeof(dataSequence1) - 1) != 10) {    //Write data sequence 1, from initial location
        printf("Failed to write first data sequence");
        close(fileDescriptor);
        return 1;
    }


    offset = lseek(fileDescriptor, 10, SEEK_SET);     // Move pointer to 10th position/index
    if (offset == (off_t)-1) {
        printf("seek operation failed");
        close(fileDescriptor);
        return 1;
    }

    if (write(fileDescriptor, dataSequence2, sizeof(dataSequence2) - 1) != 23) {     //Write data sequence 1, from 10th location.
        printf("Failed to write second data sequence");
        close(fileDescriptor);
        return 1;
    }

    close(fileDescriptor);
    return 0;
}


/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment10$ gcc WriteSeek.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment10$ ./a.out Temp.txt 

File Output - Bangalore International Institute


Assignment10$ ls
a.out  Temp.txt  WriteSeek.c
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment10$ od Temp.txt
0000000 060502 063556 066141 071157 020145 067111 062564 067162
0000020 072141 067551 060556 020154 067111 072163 072151 072165
0000040 000145
0000041
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment10$ cat Temp.txt 
Bangalore International Instituteomkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment10$ od -c Temp.txt
0000000   B   a   n   g   a   l   o   r   e       I   n   t   e   r   n
0000020   a   t   i   o   n   a   l       I   n   s   t   i   t   u   t
0000040   e
0000041

 */