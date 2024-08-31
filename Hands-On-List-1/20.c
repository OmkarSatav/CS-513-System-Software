/* Name: Omkar Dhananjay Satav
 * Reg.No: MT2024106
 * Problem Statement: Find out the priority of your running program. Modify the priority with nice command.
 * */


#include <unistd.h> // For using nice() function.
#include <stdio.h>  // For Standard Input Output Functions like printf().
#include <stdlib.h> // For Functions like atoi() - for conversion.


int main(int argumentCount, char *argumentValues[]) {

    int currPriority, newPriority;
    
    
    if (argumentCount != 2)  // Check if one argument passed or not.
    {
        printf("Please pass magnitude of change while execution.\n");
    }
    else
    {
        int priorityIncOrDec = atoi(argumentValues[1]); // Convert the argument value to an integer value.
        
        currPriority = nice(0);                        // Retrieve the current priority of the process.
        printf("Initial priority of the process: %d\n", currPriority);
        
        newPriority = nice(priorityIncOrDec);          // Magnitude value added to current priority. 
        printf("Updated priority of the process: %d\n", newPriority);
    }

    return 0;
}



/*


omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment20$ gcc priorityChange.c
omkar@omkar-TUF-Gaming-FX505GT-FX505GT:~/Documents/SoftwareSystems/HandsOnList1/Assignment20$ ./a.out 7
Initial priority: 0
Updated priority: 7


*/