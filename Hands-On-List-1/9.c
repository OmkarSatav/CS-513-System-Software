/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a program to print the following information about a given file.
        a. inode
        b. number of hard links
        c. uid
        d. gid
        e. size
        f. block size
        g. number of blocks
        h. time of last access
        i. time of last modification
        j. time of last change
 * */


#include <stdio.h>     // For standard input and output functions
#include <stdlib.h>    // For standard primary functions like exit()
#include <sys/stat.h>  // For checking the file status and properties
#include <time.h>      // For changing the time representation.

int main(int argCnt, char *argVals[]) {
    struct stat fileStat;  // structure to hold the information of the passed file.

    // Checking whether argument is passed or not.
    if (argCnt != 2) {
        perror("Please provide a filename to run the program");
        return 1;
    }

    const char *file = argVals[1];
    int status = stat(file, &fileStat);
    
    printf("File = %s\n", file);
    printf("Inode = %ld\n", fileStat.st_ino);
    printf("User ID = %d\n", fileStat.st_uid);
    printf("Group ID = %d\n", fileStat.st_gid);
    printf("Block Size = %ld bytes\n", fileStat.st_blksize);
    printf("Number of Blocks = %ld\n", fileStat.st_blocks);
    printf("File Size = %ld bytes\n", fileStat.st_size);
    printf("Hard Link Count = %ld\n", fileStat.st_nlink);
    printf("Last Accessed = %s", ctime(&fileStat.st_atime));
    printf("Metadata Changed = %s", ctime(&fileStat.st_ctime));
    printf("Last Modified = %s", ctime(&fileStat.st_mtime));

    return 0;
}



/* 

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment9$ gcc GetInfo.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment9$ ./a.out
Please provide a filename to run the program: Success
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment9$ ./a.out content.txt 
File = content.txt
Inode = 1182028
User ID = 1000
Group ID = 1000
Block Size = 4096 bytes
Number of Blocks = 8
File Size = 78 bytes
Hard Link Count = 1
Last Accessed = Tue Aug 27 13:55:04 2024
Metadata Changed = Tue Aug 20 10:44:11 2024
Last Modified = Tue Aug 20 10:44:10 2024
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment9$ 


*/