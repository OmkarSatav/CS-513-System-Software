/* 
==========================================================================
Name: 26.c
Author: Omkar Dhananjay Satav
Description : Write a program to execute an executable program.
                    a. use some executable program
                    b. pass some input to an executable program. (for example execute an executable of $./a.out name)
Date : 30th Aug, 2024
===========================================================================
*/


#include <unistd.h> // For using execl() function.

int main(int argumentCount, char *argumentValues[]) {

    const char *exeFilePath, *fileName;

    if (argumentCount != 3){       // If command line input is not given, default file is created.
        exeFilePath = "../Assignment4/a.out";
        fileName = "executeCode.c";
    }else{
        exeFilePath = argumentValues[1];  // Input file names from command line argument.
        fileName = argumentValues[2];
    }

    execl(exeFilePath, exeFilePath, fileName, NULL);  // Execute the file.
}


/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment26$ gcc executeCode.c
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment26$ ./a.out
File already existed.
File opened in read-write mode 3

*/