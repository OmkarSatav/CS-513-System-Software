/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Write a simple program to create three threads.
 * */


#include <pthread.h>          //  For using thread functions like pthread_create(), pthread_join() and pthread_t. 
#include <stdio.h>            //  For using Standard Input Output Functions like printf(). 


void* threadFunction(void* arg) {    // Function that each thread will execute       
    int id = *((int*)arg);          // Get the thread ID from the argument
    for (int i = 1; i <= 10; ++i)
        printf("Thread %d, iteration: %d\n", id, i);  // Print thread ID and iteration number
    return NULL; 
}

int main() {
    pthread_t threads[3];           // Array to hold thread identifiers for 3 threads
    int threadIds[3] = {1, 2, 3};  // IDs for each thread

    
    for (int i = 0; i < 3; ++i){        // Create 3 threads, linking to threadFunction() function.
        pthread_create(&threads[i], NULL, threadFunction, &threadIds[i]);
    }

    
    for (int i = 0; i < 3; ++i){        // Wait for all 3 threads to finish execution.
        pthread_join(threads[i], NULL);
    }
    return 0;  
}

/*


Thread 1, iteration: 1
Thread 3, iteration: 1
Thread 3, iteration: 2
Thread 3, iteration: 3
Thread 3, iteration: 4
Thread 3, iteration: 5
Thread 3, iteration: 6
Thread 1, iteration: 2
Thread 2, iteration: 1
Thread 2, iteration: 2
Thread 1, iteration: 3
Thread 1, iteration: 4
Thread 2, iteration: 3
Thread 2, iteration: 4
Thread 1, iteration: 5
Thread 1, iteration: 6
Thread 2, iteration: 5
Thread 1, iteration: 7
Thread 3, iteration: 7
Thread 3, iteration: 8
Thread 3, iteration: 9
Thread 3, iteration: 10
Thread 2, iteration: 6
Thread 2, iteration: 7
Thread 2, iteration: 8
Thread 2, iteration: 9
Thread 2, iteration: 10
Thread 1, iteration: 8
Thread 1, iteration: 9
Thread 1, iteration: 10


*/