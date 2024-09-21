/*============================================================================
Name : 7.c
Author : Omkar Satav
Description : Write a simple program to print the created thread IDs.
Date: 10th Sept, 2024.
============================================================================
*/


#include <pthread.h>        //  For using thread functions like pthread_create(), pthread_join(), pthread_self() and pthread_t. 
#include <stdio.h>          //  For using Standard Input Output Functions like printf(). 


void* threadFunction(void* arg) {
    int id = *((int*)arg);          // Get the thread ID from the argument
    printf("Thread %d has ID %lu\n",id, pthread_self());         // Get the thread ID using pthread_self().
    return NULL; 
}

int main() {
    pthread_t threads[3];   // Ids for 3 Threads
    int threadIds[3] = {1, 2, 3}; 

    for (int i = 0; i < 3; ++i) {         
        pthread_create(&threads[i], NULL, threadFunction,  &threadIds[i]);        // Create 3 threads
    }

    for (int i = 0; i < 3; ++i) {
        pthread_join(threads[i], NULL);          // Wait for all 3 threads to finish execution.
    }

    return 0;  
}


/*


Thread 1 has ID 140218011219648
Thread 2 has ID 140218002826944
Thread 3 has ID 140217994434240


*/
