/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a program to print the system limitation of 
 *                      a. maximum length of the arguments to the exec family of functions.
                        b. maximum number of simultaneous process per user id.
                        c. number of clock ticks (jiffy) per second.
                        d. maximum number of open files
                        e. size of a page
                        f. total number of pages in the physical memory
                        g. number of currently available pages in the physical memory.
 * */


#include <stdio.h>         // For using Standard Input Output functions like printf.
#include <unistd.h>        // For using sysconf().
#include <sys/sysinfo.h>   // For using sysinfo().
#include <sys/resource.h>  // For using getrlimit().

int main() {

    // A. Maximum length of arguments for exec functions
    long maxNoArgs = sysconf(_SC_ARG_MAX);
    printf("Max length of arguments to exec functions: %ld\n\n", maxNoArgs);
    printf("************************************************\n\n");


    // B. Maximum number of simultaneous processes per user
    long maxPrc = sysconf(_SC_CHILD_MAX);
    printf("Max simultaneous processes per user: %ld\n\n", maxPrc);
    printf("************************************************\n\n");


    // C. Number of clock ticks per second
    long clockTicksPerSec = sysconf(_SC_CLK_TCK);
    printf("Clock ticks per second: %ld\n\n", clockTicksPerSec);
    printf("************************************************\n\n");


    // D. Maximum number of open files
    struct rlimit maxOpenFiles;
    getrlimit(RLIMIT_NOFILE, &maxOpenFiles);
    printf("Max open files: %ld\n\n", maxOpenFiles.rlim_cur);
    printf("************************************************\n\n");


    // E. Size of a memory page
    long pageSize = sysconf(_SC_PAGESIZE);
    printf("Size of a memory page: %ld bytes\n\n", pageSize);
    printf("************************************************\n\n");


    // F. Total and available pages in physical memory
    struct sysinfo sysInfo;
    sysinfo(&sysInfo);
    long totalPages = sysInfo.totalram / pageSize;
    long freePages = sysInfo.freeram / pageSize;
    printf("Total pages in physical memory: %ld\n", totalPages);
    printf("Available pages in physical memory: %ld\n\n", freePages);

    return 0;
}



/*


Max length of arguments to exec functions: 2097152

************************************************

Max simultaneous processes per user: 63051

************************************************

Clock ticks per second: 100

************************************************

Max open files: 1024

************************************************

Size of a memory page: 4096 bytes

************************************************

Total pages in physical memory: 4054005
Available pages in physical memory: 890024


*/