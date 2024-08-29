/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a program, open a file, call fork, and then write to the file by both the child as well as the
                    parent processes. Check output of the file.
 * */


#include <stdio.h>   // For Standard Input Output Operations like printf() and perror()
#include <unistd.h>  // For writing content into the file -write()
#include <fcntl.h>   // For opening a file - open()
#include <string.h>  // For checking the length of string - strlen()

int main(int argumentCount, char *argumentValues[]) {
    int fileDescptr;
    const char *file1;


   if (argumentCount != 2){
        file1 = "destination.txt";     // If command line input is not given, default file is created.
        
    }else{
        file1 = argumentValues[1];  // Input file names from command line argument.
    }


    fileDescptr = open(file1, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0764);
    
    if (fileDescptr < 0) {
        perror("Error opening file");
        return 1;
    }

    int prcId = fork();

    if (prcId < 0) {
        perror("Error during fork");
        return 1;
    }
    else if (prcId == 0) {
        char *childText = "--------------  Child process   -------------- \nA new process created by fork(), inheriting a copy of the parent process's memory and resources.\n\n";
        write(fileDescptr, childText, strlen(childText));
    }
    else {
        char *parentText = "-------------  Parent process  --------------- \nThe original process that creates a child process using fork(), receiving the child's PID and continuing execution alongside the child.\n";
        write(fileDescptr, parentText, strlen(parentText));
    }

    close(fileDescptr);
    return 0;
}




/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment22$ gcc childParentWrite.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment22$ ./a.out


destination.txt
-------------  Parent process  --------------- 
The original process that creates a child process using fork(), receiving the child's PID and continuing execution alongside the child.
--------------  Child process   -------------- 
A new process created by fork(), inheriting a copy of the parent process's memory and resources.

*/
