/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a program to execute ls -Rl by the following system calls
                    a. execl
                    b. execlp
                    c. execle 
                    d. execv
                    e. execvp
 * */


#include<stdio.h> // For Standard Input Output Functions like printf() and scanf().
#include<unistd.h>  // For using execl(), execlp(), execle(), execv() and execvp() functions.
#include <string.h>  // For using strcmp() function.

extern char **environ;

int main(){
    char option[10];
    printf("Which exec command do you want to use to execute 'ls -l', 1)execl 2)execlp 3)execle 4)execv  5)execvp\n");
    scanf("%s", &option);   // Choose particular execute command.

    if(strcmp(option,"execl") == 0){         // For execl
        execl("/bin/ls","ls","-l",NULL);
    }
    else if(strcmp(option,"execlp") == 0){    // For execlp
        execlp("ls", "ls", "-l", NULL);
    }
    else if(strcmp(option,"execle") == 0){    // For execle
        execle("/bin/ls","ls","-l",NULL,environ);
    }
    else if(strcmp(option,"execv") == 0){    // For execv
        char *args[] = {"ls", "-l", NULL};
        execv("/bin/ls", args);
    }
    else if(strcmp(option,"execvp") == 0){     // For execvp
        char *args[] = {"ls", "-l", NULL};
        execvp("ls", args);
    }
    else{
        printf("Incorrect Option Selected.\n");
    }
  

}



/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment27$ gcc execute.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment27$ ./a.out
Which exec command do you want to use to execute 'ls -l', 1)execl 2)execlp 3)execle 4)execv  5)execvp
execl
total 24
-rwxrwxr-x 1 omkar omkar 16392 Aug 31 18:20 a.out
-rw-rw-r-- 1 omkar omkar  1443 Aug 31 18:20 execute.c

*/