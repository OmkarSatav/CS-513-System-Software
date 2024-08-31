/* 
==========================================================================
Name: 6.c
Author: Omkar Dhananjay Satav
Description : Write a program to take input from STDIN and display on STDOUT. Use only read/write system calls
Date : 13th Aug, 2024
===========================================================================
*/


#include <unistd.h>  // read() and write() used fromm unistd for reading and writing content.

int main() {
    const int buffSize = 1024;     // Size of buffer
    char buff[buffSize];           // Buffer to store input
    int bytesRd;    

    
    bytesRd = read(STDIN_FILENO, buff, buffSize);  // Read input from standard input.

    if(bytesRd > 0) {
        write(STDOUT_FILENO, buff, bytesRd);  // Write the input data to the standard ouput
    }

    return 0;
}



/*


omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment6$ gcc inputOutput.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment6$ ./a.out
Wonderful day at IIITB
Wonderful day at IIITB
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment6$


*/