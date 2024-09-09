/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a program to measure how much time is taken to execute 100 getppid () system call. 
 *                    Use time stamp counter.
 * */


#include <stdio.h>          // For using Standard Input Output Functions like printf().
#include <unistd.h>         // For using System calls like getppid().      


unsigned long long rdtsc() {            // Reads the timestamp counter and returns the number of clock cycles since the last reset.
    unsigned long long destination;
    __asm__ __volatile__ ("rdtsc":"=A" (destination));  // Read timestamp counter
}

int main() {
    long long start_time, end_time;
    long long totalTime = 0;

    for (int i = 0; i < 100; i++) {

        start_time = rdtsc();        // Start time of ith call
        getppid();                      // Call getppid()
        end_time = rdtsc();          // End time of ith call

        totalTime += end_time - start_time;        // Clock Cycle duration of ith getppid() call.
    }

    double freq = 2.3e9;  // Processor frequency in 2.3 GHz
    double totalTimeInNano = (totalTime / freq) * 1e9;  // Convert Seconds to Nanoseconds

    printf("Execution time for 100 getppid() calls is %.2f nanoseconds\n", totalTimeInNano);
    return 0;
}



/*


Execution time for 100 getppid() calls is 270349.57 nanoseconds


*/