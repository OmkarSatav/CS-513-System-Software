/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Create the following types of a files using (i) shell command (ii) system call
                    a. soft link (symlink system call)
                    b. hard link (link system call)
                    c. FIFO (mkfifo Library Function or mknod system call)
 * */


#include <stdio.h>    // For Standard Input Output Functions like printf() and perror().
#include <unistd.h>   // For using symlink() function.

int main() {
    const char *fileName = "file1.txt";
    const char *softLkName = "SOFTLK";

    if (symlink(fileName, softLkName) < 0) {    // Soft link creation.
        perror("Error while executing symlink() function. Failed to create soft link.");
        return 1;
    }

    printf("Soft link is created.\n");
    return 0;
}

