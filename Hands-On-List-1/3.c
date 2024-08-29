/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a program to create a file and print the file descriptor value. Use creat ( ) system call. 
 * */


#include <stdio.h>   // Standard Input Output Functions.
#include <fcntl.h>   // For file creation - creat()
#include <unistd.h>  // For closing fd resources - close()

int main(int argumentCount, int *argumentValues[]) {

    const char *file;

    if (argumentCount != 2){
        file = "origin.txt";       // If command line input is not given, default file is created.
    }else{
        file = argumentValues[1];  // Input file name from command line argument.
    }
    
    int fd = creat(file, 0700);    // Creates a file. FD id is returned.


    printf("File descriptor value: %d", fd);  // Value of File Descriptor.

    close(fd);

    return 0;
}


/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment3$ gcc createFileAndDisplayFD.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment3$ ls
 a.out                      origin.txt
 createFileAndDisplayFD.c  'Screenshot from 2024-08-27 14-31-29.png'
 Main.txt                  'Screenshot from 2024-08-27 14-31-58.png'
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment3$ ./a.out Main.txt 
File descriptor value: 3omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment3$ 


*/