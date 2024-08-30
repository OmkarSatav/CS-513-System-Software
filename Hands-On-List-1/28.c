/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a program to get maximum and minimum real time priority.
 * */


#include <sched.h> // For scheduling functions like sched_get_priority_max() and sched_get_priority_min().
#include <stdio.h> // For Standard Input Output Functions like printf().
#include <stdlib.h> // For Standard functions to language like exit().

int main() {
    int maximumPriority, minimumPriority;

    maximumPriority = sched_get_priority_max(SCHED_FIFO);  // Get the maximum priority for SCHED_FIFO
    if (maximumPriority == -1) {
        printf("Issue in finding maximum priority for SCHED_FIFO");
        exit(1); 
    }
    printf("The maximum priority with First In First Out(FIFO) Scheduling Policy is: %d\n", maximumPriority);


    minimumPriority = sched_get_priority_min(SCHED_FIFO);  // Get the minimum priority for SCHED_FIFO
    if (minimumPriority == -1) {
        printf("Issue in finding minimum priority for SCHED_FIFO");
        exit(1);
    }
    printf("The minimum priority with First In First Out(FIFO) Scheduling Policy is: %d\n", minimumPriority);

    return 0;
}
