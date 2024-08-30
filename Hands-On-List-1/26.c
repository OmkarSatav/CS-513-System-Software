/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a program to execute an executable program.
                    a. use some executable program
                    b. pass some input to an executable program. (for example execute an executable of $./a.out name)
* */


#include <unistd.h> // For using execl() function.

int main(int argumentCount, char *argumentValues[]) {

    const char *exeFilePath, *fileName;

    if (argumentCount != 3){       // If command line input is not given, default file is created.
        exeFilePath = "../Assignment4/a.out";
        fileName = "openFile.c";
    }else{
        exeFilePath = argumentValues[1];  // Input file names from command line argument.
        fileName = argumentValues[2];
    }

    execl(exeFilePath, exeFilePath, fileName, NULL);  // Execute the file.
}


/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment26$ gcc executeCode.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment26$ ./a.out
File opened in read-write mode -1
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment26$ 


*/