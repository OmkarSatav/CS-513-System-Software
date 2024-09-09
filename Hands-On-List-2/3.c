/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a program to set (any one) system resource limit. Use setrlimit system call.
 * */

#include <sys/resource.h>   // For using rlimit, getrlimit() and setrlimit().
#include <stdio.h>          // For using Standard Input Output Functions like printf() and perror().


int main() {
    struct rlimit limits;

    // Current file descriptor limits
    getrlimit(RLIMIT_NOFILE, &limits);
    printf("Current file descriptor limits:\n");
    printf("  Soft limit: %ld\n", limits.rlim_cur);
    printf("  Hard limit: %ld\n\n", limits.rlim_max);

    // Set new limits for file descriptors
    limits.rlim_cur = 4096;  // New value of soft limit
    limits.rlim_max = 8192;  // New value of hard limit

    if (setrlimit(RLIMIT_NOFILE, &limits) == -1) {
        perror("Error while setting limits for file descriptors.");
        return 1;
    }

    // Updated file descriptor limits
    getrlimit(RLIMIT_NOFILE, &limits);
    printf("Updated file descriptor limits:\n");
    printf("  Soft limit: %ld\n", limits.rlim_cur);
    printf("  Hard limit: %ld\n", limits.rlim_max);

    return 0;
}


/*


Current file descriptor limits:
  Soft limit: 1024
  Hard limit: 1048576

Updated file descriptor limits:
  Soft limit: 4096
  Hard limit: 8192


*/