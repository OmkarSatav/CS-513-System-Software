/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a program to find out the opening mode of a file. Use fcntl.
 * */


#include <fcntl.h>  // For using open() and fcntl().
#include <unistd.h> // For closing resources close().
#include <stdio.h>  // For Standard Input Output Functions like printf().

int main() {
    int fileDescript = open("openMode.txt", O_CREAT | O_WRONLY, 0776);   // Open or Create file in write mode.
    int mode = fcntl(fileDescript, F_GETFL);                    // Fetch the opening mode of file.
    printf("File opened in number associated with it : \n (0 - O_RDONLY) (1 - O_WRONLY) (2 - O_RDWR) - %d\n", mode & O_ACCMODE);
    close(fileDescript);  
    return 0;
}



/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment12$ gcc fileOpeningMode.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment12$ ./a.out
File opened in number associated with it : 
 (0 - O_RDONLY) (1 - O_WRONLY) (2 - O_RDWR) - 1


*/