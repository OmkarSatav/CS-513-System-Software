/*
==========================================================================
Name: 8.c
Author: Omkar Dhananjay Satav
Description : Write a program to open a file in read only mode, read line by line and display each line as 
              it is read. Close the file when end of file is reached.
Date : 20th Aug, 2024
===========================================================================
*/

#include <unistd.h> // For system calls like write()
#include <stdlib.h> // For using standard library functions like exit().
#include <stdio.h> // For standard Input/Output functions like fopen().
#include <fcntl.h>  // For file control options

int main(int argumentCount, char *argumentValues[]) {

    const char *file1;

    if (argumentCount != 2){
        file1 = "origin.txt";       // If command line input is not given, default file is created.
    }else{
        file1 = argumentValues[1];  // Input file names from command line argument.
    }

    const char EndOfLine = '\n'; // Define the end-of-line character

    FILE *fileDescriptor = fopen(file1, "r");   // Open the file in read mode

    int character = fgetc(fileDescriptor); // Read the first character from the file
    char buff [1024];          // buffer to store each line from input file
    int sizeOfLine = 0;       // Index for buff

    
    while (character != EOF) {            // Loop until the end of the file
        while (character != EOF && character != EndOfLine) { // Read until end of line or end of file
            buff[sizeOfLine] = character;
            sizeOfLine++;
            character = fgetc(fileDescriptor);
        }
        buff[sizeOfLine] = '\n';        // Append newline character
        sizeOfLine++;
        
        write(1, buff, sizeOfLine);
        sizeOfLine = 0;

        if (character != EOF) character = fgetc(fileDescriptor);  // If the current value of character is not EOF, read the next character from the file and store it in character

    }

    fclose(fileDescriptor); // Close the file
    return 0;

}



/* 

1. Once in a small village, there lived a kind old woman who loved to garden.  
2. Every morning, she tended to her flowers, which bloomed brighter than any in the village.  
3. One day, a terrible drought hit, and the villagers’ gardens began to wither.  
4. But the old woman’s flowers remained vibrant.  
5. Curious, the villagers asked her secret.  
6. She smiled and said, “I speak to them with love and care every day.”  
7. Inspired, the villagers began talking to their own plants.  
8. Slowly, their gardens started to recover, blooming beautifully once more.  
9. The village became more colorful than ever, filled with life and joy.  
10. And the old woman’s simple wisdom taught them that love and kindness could bring even the driest land to life.

*/


