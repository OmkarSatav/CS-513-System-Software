/* 
==========================================================================
Name: 14.c
Author: Omkar Dhananjay Satav
Description : Write a program to find the type of a file.
                    a. Input should be taken from command line.
                    b. program should be able to identify any type of a file.
Date : 31st Aug, 2024
===========================================================================
*/

#include<stdio.h>    // For Standard Input Output Functions like printf(), scanf(), and perror().
#include<unistd.h>   // For using functions like exit().
#include<sys/stat.h>  // For using stat and lstat().

int main() {
    struct stat stats;
    char input[100];

    scanf("%[^\n]", &input);  // Read file name from input

 
    if (lstat(input, &stats) < 0) {      // To get information of the file.
        perror("lstat() fails to work.");
        return 1;
    }
    
    switch (stats.st_mode & S_IFMT) {         // Check file type using masking.
        case S_IFBLK:  printf("This is a block special file.\n"); 
                       break;
        case S_IFCHR:  printf("This is a character special file.\n"); 
                       break;
        case S_IFIFO:  printf("This is a FIFO/pipe file.\n"); 
                       break;
        case S_IFDIR:  printf("This is a directory file.\n"); 
                       break;
        case S_IFREG:  printf("This is a regular file.\n"); 
                       break;
        case S_IFLNK:  printf("This is a symbolic link file.\n"); 
                       break;
        default:       printf("This is a unknown type file.\n"); 
                       break;
    }

    return 0;
}




/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment14$ gcc fileType.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment14$ ./a.out
temp.txt
This is a regular file.
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment14$ 

*/