/*============================================================================
Name : 2.c
Author : Omkar Satav
Description :Write a program to print the system resource limits. 
             Use getrlimit system call.
Date: 9th Sept, 2024.
============================================================================
*/


#include <sys/resource.h>   // For using rlimit and getrlimit().
#include <stdio.h>          // For using Standard Input Output Functions like printf().

int main() {
    struct rlimit limits;

    // Virtual memory limit
    getrlimit(RLIMIT_AS, &limits);
    printf("Virtual Memory Limit:\n");
    printf("Soft limit: %ld bytes\n", limits.rlim_cur);
    printf("Hard limit: %ld bytes\n\n", limits.rlim_max);

    // Core file size limit
    getrlimit(RLIMIT_CORE, &limits);
    printf("Core File Size Limit:\n");
    printf("Soft limit: %ld bytes\n", limits.rlim_cur);
    printf("Hard limit: %ld bytes\n\n", limits.rlim_max);

    // CPU time limit
    getrlimit(RLIMIT_CPU, &limits);
    printf("CPU Time Limit:\n");
    printf("Soft limit: %ld seconds\n", limits.rlim_cur);
    printf("Hard limit: %ld seconds\n\n", limits.rlim_max);

    // Data segment size limit
    getrlimit(RLIMIT_DATA, &limits);
    printf("Data Segment Size Limit:\n");
    printf("Soft limit: %ld bytes\n", limits.rlim_cur);
    printf("Hard limit: %ld bytes\n\n", limits.rlim_max);

    // File size limit
    getrlimit(RLIMIT_FSIZE, &limits);
    printf("File Size Limit:\n");
    printf("Soft limit: %ld bytes\n", limits.rlim_cur);
    printf("Hard limit: %ld bytes\n\n", limits.rlim_max);

    // File locks limit
    getrlimit(RLIMIT_LOCKS, &limits);
    printf("File Locks Limit:\n");
    printf("Soft limit: %ld locks\n", limits.rlim_cur);
    printf("Hard limit: %ld locks\n\n", limits.rlim_max);

    // Memory lock limit
    getrlimit(RLIMIT_MEMLOCK, &limits);
    printf("Memory Lock Limit:\n");
    printf("Soft limit: %ld bytes\n", limits.rlim_cur);
    printf("Hard limit: %ld bytes\n\n", limits.rlim_max);

    // Message queue size limit
    getrlimit(RLIMIT_MSGQUEUE, &limits);
    printf("Message Queue Size Limit:\n");
    printf("Soft limit: %ld bytes\n", limits.rlim_cur);
    printf("Hard limit: %ld bytes\n\n", limits.rlim_max);

    // Nice value limit
    getrlimit(RLIMIT_NICE, &limits);
    printf("Nice Value Limit:\n");
    printf("Soft limit: %ld\n", limits.rlim_cur);
    printf("Hard limit: %ld\n\n", limits.rlim_max);

    // Open file descriptors limit
    getrlimit(RLIMIT_NOFILE, &limits);
    printf("Open File Descriptors Limit:\n");
    printf("Soft limit: %ld\n", limits.rlim_cur);
    printf("Hard limit: %ld\n\n", limits.rlim_max);

    // Stack size limit
    getrlimit(RLIMIT_STACK, &limits);
    printf("Stack Size Limit:\n");
    printf("Soft limit: %ld bytes\n", limits.rlim_cur);
    printf("Hard limit: %ld bytes\n", limits.rlim_max);

    return 0;
}



/*


Virtual Memory Limit:
Soft limit: -1 bytes
Hard limit: -1 bytes

Core File Size Limit:
Soft limit: 0 bytes
Hard limit: -1 bytes

CPU Time Limit:
Soft limit: -1 seconds
Hard limit: -1 seconds

Data Segment Size Limit:
Soft limit: -1 bytes
Hard limit: -1 bytes

File Size Limit:
Soft limit: -1 bytes
Hard limit: -1 bytes

File Locks Limit:
Soft limit: -1 locks
Hard limit: -1 locks

Memory Lock Limit:
Soft limit: 2075648000 bytes
Hard limit: 2075648000 bytes

Message Queue Size Limit:
Soft limit: 819200 bytes
Hard limit: 819200 bytes

Nice Value Limit:
Soft limit: 0
Hard limit: 0

Open File Descriptors Limit:
Soft limit: 1024
Hard limit: 1048576

Stack Size Limit:
Soft limit: 8388608 bytes
Hard limit: -1 bytes


*/