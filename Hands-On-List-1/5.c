/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a program to create five new files with infinite loop. Execute the program in the background and check the file 
 * descriptor table at /proc/pid/fd.
 * */


#include <fcntl.h> // For file creation and opening - open()

int main() {
    while (100) {                          // Infinite Loop
        open("temp1.txt", O_CREAT, 0700);  // 5 files are created.
        open("temp2.txt", O_CREAT, 0704); 
        open("temp3.txt", O_CREAT, 0740); 
        open("temp4.txt", O_CREAT, 0744); 
        open("temp5.txt", O_CREAT, 0707); 
    }
    return 0;
}



/*

omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment3$ gcc createFileAndDisplayFD.c 
createFileAndDisplayFD.c: In function ‘main’:
createFileAndDisplayFD.c:18:14: warning: assignment to ‘const char *’ from incompatible pointer type ‘int *’ [-Wincompatible-pointer-types]
   18 |         file = argumentValues[1];  // Input file name from command line argument.
      |              ^
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment3$ ls
 a.out                      origin.txt
 createFileAndDisplayFD.c  'Screenshot from 2024-08-27 14-31-29.png'
 Main.txt                  'Screenshot from 2024-08-27 14-31-58.png'
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment3$ ./a.out Main.txt 
File descriptor value: 3omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment3$ cd ../Assignment5
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment5$ gcc infiniteFileOpeningCreation.c 
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment5$ ./a.out &
[1] 28213
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment5$ ps
    PID TTY          TIME CMD
  27355 pts/0    00:00:00 bash
  28213 pts/0    00:00:01 a.out
  28215 pts/0    00:00:00 ps
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment5$ ls -l /proc/28213/fd
total 0
lrwx------ 1 omkar omkar 64 Aug 29 19:47 0 -> /dev/pts/0
lrwx------ 1 omkar omkar 64 Aug 29 19:47 1 -> /dev/pts/0
lr-x------ 1 omkar omkar 64 Aug 29 19:47 10 -> /home/omkar/Documents/SoftwareSystems/HandsOnList1/Assignment5/temp3.txt
lr-x------ 1 omkar omkar 64 Aug 29 19:47 100 -> /home/omkar/Documents/SoftwareSystems/HandsOnList1/Assignment5/temp3.txt
lr-x------ 1 omkar omkar 64 Aug 29 19:47 1000 -> /home/omkar/Documents/SoftwareSystems/HandsOnList1/Assignment5/temp3.txt
lr-x------ 1 omkar omkar 64 Aug 29 19:47 1001 -> /home/omkar/Documents/SoftwareSystems/HandsOnList1/Assignment5/temp4.txt
lr-x------ 1 omkar omkar 64 Aug 29 19:47 1002 -> /home/omkar/Documents/SoftwareSystems/HandsOnList1/Assignment5/temp5.txt
lr-x------ 1 omkar omkar 64 Aug 29 19:47 1003 -> /home/omkar/Documents/SoftwareSystems/HandsOnList1/Assignment5/temp1.txt
lr-x------ 1 omkar omkar 64 Aug 29 19:47 1004 -> /home/omkar/Documents/SoftwareSystems/HandsOnList1/Assignment5/temp2.txt
lr-x------ 1 omkar omkar 64 Aug 29 19:47 1005 -> /home/omkar/Documents/SoftwareSystems/HandsOnList1/Assignment5/temp3.txt
lr-x------ 1 omkar omkar 64 Aug 29 19:47 1006 -> /home/omkar/Documents/SoftwareSystems/HandsOnList1/Assignment5/temp4.txt
lr-x------ 1 omkar omkar 64 Aug 29 19:47 1007 -> /home/omkar/Documents/SoftwareSystems/HandsOnList1/Assignment5/temp5.txt
lr-x------ 1 omkar omkar 64 Aug 29 19:47 1008 -> /home/omkar/Documents/SoftwareSystems/HandsOnList1/Assignment5/temp1.txt
lr-x------ 1 omkar omkar 64 Aug 29 19:47 1009 -> /home/omkar/Documents/SoftwareSystems/HandsOnList1/Assignment5/temp2.txt
lr-x------ 1 omkar omkar 64 Aug 29 19:47 101 -> /home/omkar/Documents/SoftwareSystems/HandsOnList1/Assignment5/temp4.txt
lr-x------ 1 omkar omkar 64 Aug 29 19:47 1010 -> /home/omkar/Documents/SoftwareSystems/HandsOnList1/Assignment5/temp3.txt
lr-x------ 1 omkar omkar 64 Aug 29 19:47 1011 -> /home/omkar/Documents/SoftwareSystems/HandsOnList1/Assignment5/temp4.txt


*/