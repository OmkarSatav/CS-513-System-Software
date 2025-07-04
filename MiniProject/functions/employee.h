#ifndef EMPLOYEE_FUNCTIONS
#define EMPLOYEE_FUNCTIONS

#include <sys/ipc.h>
#include <sys/sem.h>


#include "./common.h"
#include "./server-constants.h"
#include "../recordtypes/employee.h"
#include "../recordtypes/loan.h"

struct Employee loggedInEmployee;
int semIdentifier;


// Function Prototypes =================================
// bool update_account_in_file(struct Account *account);               // *
bool change_password(int connFD);                 // *
bool handle_loan_decision(int connFD);                //  *
bool approve_or_reject_loan(int connFD);
void check_loan_status(int connFD);
bool change_employee_password(int connFD);
bool add_account(int connFD);
int add_customer(int connFD, int newAccountNumber);
void send_loans_assigned_to_employee(int connFD);
// =====================================================
// Function Definition =================================




// bool employee_operation_handler(int connFD) {
//     // Call the manager login handler
//     if (employee_login_handler(connFD, &loggedInEmployee)) { // Assume `true` indicates Manager login
//         ssize_t writeBytes, readBytes;            // Number of bytes read from / written to the client
//         char readBuffer[1000], writeBuffer[1000]; // Buffers for reading & writing to the client

//         bzero(writeBuffer, sizeof(writeBuffer));
//         strcpy(writeBuffer, EMPLOYEE_LOGIN_SUCCESS); // Message for successful login
//         while (1) {
//             strcat(writeBuffer, "\n");
//             strcat(writeBuffer, EMPLOYEE_MENU); // Assume you have a predefined MANAGER_MENU
//             writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
//             if (writeBytes == -1) {
//                 perror("Error while writing MANAGER_MENU to client!");
//                 return false;
//             }
//             bzero(writeBuffer, sizeof(writeBuffer));

//             bzero(readBuffer, sizeof(readBuffer));
//             usleep(100000);
//             readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//             if (readBytes == -1) {
//                 perror("Error while reading client's choice for MANAGER_MENU");
//                 return false;
//             }
//             int choice = atoi(readBuffer);
//             switch (choice) {
//                 case 1:
//                     add_account(connFD);
//                     break;
//                 case 2:
//                     modify_customer_info(connFD);
//                     break;
//                 case 3:
//                     send_loans_assigned_to_employee(connFD);
//                     break;
//                 case 4:
//                     approve_or_reject_loan(connFD);
//                     break;
//                 case 5:
//                     check_loan_status(connFD);
//                     break;
//                 case 6:
//                     change_employee_password(connFD);
//                     break;
//                 case 7:
//                     // employee_logout_handler(loggedInEmployee.id, connFD);
//                     // const char *logout = "Logging out the account!🌟\n type ok ";
//                     // ssize_t writeBytes1 = write(connFD, logout, strlen(logout));
//                     // if (writeBytes1 == -1) {
//                     //     perror("Error sending exit message to client");
//                     // }

//                     // // Dummy read for acknowledgment from the client
//                     // char readBuffer2[100];
//                     // bzero(readBuffer2, sizeof(readBuffer2)); // Clear the buffer

//                     // ssize_t readbytes3 = read(connFD, readBuffer2, sizeof(readBuffer2)); // Dummy read
//                     // // Close the client connection
//                     // close(connFD);
//                     return false; 
//                 case 8:
//                     // const char *exit2 = "Exiting the application!🌟 type ok \n";
//                     // ssize_t writeBytes2 = write(connFD, exit2, strlen(exit2));
//                     // if (writeBytes2 == -1) {
//                     //     perror("Error sending exit message to client");
//                     // }

//                     // // Dummy read for acknowledgment from the client
//                     // char readBuffer3[100];
//                     // bzero(readBuffer3, sizeof(readBuffer3)); // Clear the buffer

//                     // // Reading the acknowledgment from the client
//                     // ssize_t readBytes3 = read(connFD, readBuffer3, sizeof(readBuffer3) - 1);
//                     // if (readBytes3 == -1) {
//                     //     perror("Error reading acknowledgment from client");
//                     // } else if (readBytes3 > 0) {
//                     //     readBuffer[readBytes3] = '\0'; // Null-terminate the received string
//                     //     printf("Received acknowledgment from client: %s\n", readBuffer3);
//                     // } else {
//                     //     printf("No acknowledgment received from client.\n");
//                     // }

//                     // // Close the client connection
//                     // close(connFD);
//                     break; 
//                 default:
//                     write(connFD, "Invalid choice! Please try again.\n", 36);
//             }
//         }
//     } else {
//         // MANAGER LOGIN FAILED
//         return false;
//     }
//     return true;
// }


bool employee_operation_handler(int connFD) {
    // Authenticate the employee
    if (!employee_login_handler(connFD, &loggedInEmployee)) {
        perror("Employee login failed!");
        return false;
    }

    ssize_t writeBytes, readBytes;
    char readBuffer[1000], writeBuffer[1000];

    // Notify the employee of successful login
    snprintf(writeBuffer, sizeof(writeBuffer), "%s\n%s", EMPLOYEE_LOGIN_SUCCESS, EMPLOYEE_MENU);
    while (1) {
        // Send the employee menu
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1) {
            perror("Error while sending EMPLOYEE_MENU to client!");
            return false;
        }

        // Clear buffers and read the employee's choice
        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1); // Leave space for null terminator
        if (readBytes <= 0) {
            if (readBytes == 0) {
                printf("Client disconnected.\n");
            } else {
                perror("Error while reading client's choice!");
            }
            return false;
        }
        readBuffer[readBytes] = '\0'; // Null-terminate the input

        // Parse the employee's choice
        int choice = atoi(readBuffer);
        switch (choice) {
            case 1:
                if (!add_account(connFD)) {
                    write(connFD, "Error adding account. Please try again.\n", 40);
                }
                break;

            case 2:
                if (!modify_customer_info(connFD)) {
                    write(connFD, "Error modifying customer info. Please try again.\n", 50);
                }
                break;

            case 3:
                send_loans_assigned_to_employee(connFD);
                break;

            case 4:
                if (!approve_or_reject_loan(connFD)) {
                    write(connFD, "Error processing loan. Please try again.\n", 40);
                }
                break;

            case 5:
                check_loan_status(connFD);
                break;

            case 6:
                if (!change_employee_password(connFD)) {
                    write(connFD, "Error changing password. Please try again.\n", 42);
                }
                break;

            case 7:
                // Logout the employee
                write(connFD, "Logging out... type 'ok' to continue.\n", 38);
                read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read for acknowledgment
                return false;

            case 8:
                // Exit the application
                write(connFD, "Exiting the application... type 'ok' to continue.\n", 50);
                read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read for acknowledgment
                return false;

            default:
                write(connFD, "Invalid choice! Please try again.\n", 36);
                break;
        }

        // Refresh the menu for the next iteration
        snprintf(writeBuffer, sizeof(writeBuffer), "\n%s", EMPLOYEE_MENU);
    }

    return true;
}






void send_loans_assigned_to_employee(int connFD) {
    int employeeID = loggedInEmployee.id; // Get the current employee's ID
    int loanFileDescriptor = open(LOAN_RECORD_FILE, O_RDONLY);
    if (loanFileDescriptor == -1) {
        perror("Error opening loan records file");
        write(connFD, "Unable to access loan records.\n", 31);
        return;
    }

    struct Loan loan;
    ssize_t readBytes;
    char buffer[500]; // Buffer for sending loan details
    int count = 0;    // To count the number of loans assigned to the employee

    // Read each loan record and check the status and employee ID
    while ((readBytes = read(loanFileDescriptor, &loan, sizeof(struct Loan))) > 0) {
        if (loan.status == 1 && loan.empID == employeeID) { // Check if loan is assigned to this employee
            // Prepare loan detail message
            snprintf(buffer, sizeof(buffer), "Loan ID: %d, Account ID: %d, Amount: %d\n",
                     loan.loanID, loan.custID, loan.amount);
            if (write(connFD, buffer, strlen(buffer)) == -1) {
                perror("Error sending loan details to client");
                close(loanFileDescriptor);
                return;
            }
            count++;
        }
        bzero(buffer, sizeof(buffer)); // Clear the buffer after each use
    }

    if (readBytes == -1) {
        perror("Error reading loan records");
        write(connFD, "Error reading loan records.\n", 28);
        close(loanFileDescriptor);
        return;
    }

    if (count == 0) {
        write(connFD, "No loans assigned to you.\n", 26);
    } else {
        snprintf(buffer, sizeof(buffer), "Total loans assigned to you: %d\n", count);
        write(connFD, buffer, strlen(buffer));
    }

    close(loanFileDescriptor); // Close the file descriptor
}



// bool approve_or_reject_loan(int connFD) {
//     // Prompt for the loan ID
//     const char *promptMessage = "Please enter the Loan ID to approve or reject: ";
//     write(connFD, promptMessage, strlen(promptMessage));

//     char readBuffer[100];
//     ssize_t readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
//     if (readBytes <= 0) {
//         perror("Error reading loan ID from client");
//         return false;
//     }
//     readBuffer[readBytes] = '\0'; // Null-terminate the string
//     int loanID = atoi(readBuffer); // Convert to integer

//     // Open the loan records file for reading and writing
//     int loanFileDescriptor = open(LOAN_RECORD_FILE, O_RDWR);
//     if (loanFileDescriptor == -1) {
//         perror("Error opening loan records file");
//         const char *errorMessage = "Unable to access loan records.\n";
//         write(connFD, errorMessage, strlen(errorMessage));
//         return false;
//     }

//     struct Loan loan;
//     ssize_t bytesRead;
//     bool loanFound = false;

//     // Read through the loan records
//     while ((bytesRead = read(loanFileDescriptor, &loan, sizeof(struct Loan))) > 0) {
//         if (loan.loanID == loanID && loan.status == 1 && loan.empID == loggedInEmployee.id) {
//             loanFound = true; // Loan found and assigned to the employee

//             // Prompt employee for approval or rejection
//             const char *approvalPrompt = "Do you want to approve or reject the loan? (a for approve, r for reject): ";
//             write(connFD, approvalPrompt, strlen(approvalPrompt));

//             // Read the employee's decision
//             bzero(readBuffer, sizeof(readBuffer));
//             readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
//             if (readBytes <= 0) {
//                 perror("Error reading employee decision from client");
//                 close(loanFileDescriptor);
//                 return false;
//             }
//             readBuffer[readBytes] = '\0'; // Null-terminate the string

//             // Update the loan status based on employee's decision
//             if (readBuffer[0] == 'a') {
//                 loan.status = 2; // Approved
//             } else if (readBuffer[0] == 'r') {
//                 loan.status = 3; // Rejected
//             } else {
//                 const char *invalidInput = "Invalid input! Please enter 'a' to approve or 'r' to reject.\n";
//                 write(connFD, invalidInput, strlen(invalidInput));
//                 close(loanFileDescriptor);
//                 return false;
//             }

//             // Move the file pointer back to the correct position to overwrite the loan record
//             off_t offset = lseek(loanFileDescriptor, -sizeof(struct Loan), SEEK_CUR);
//             if (offset == -1) {
//                 perror("Error seeking in loan records file");
//                 close(loanFileDescriptor);
//                 return false;
//             }

//             // Write the updated loan record back to the file
//             if (write(loanFileDescriptor, &loan, sizeof(struct Loan)) == -1) {
//                 perror("Error updating loan record in file");
//                 close(loanFileDescriptor);
//                 return false;
//             }

//             // Confirmation message
//             const char *successMessage = "Loan application status updated successfully.\n";
//             write(connFD, successMessage, strlen(successMessage));
//             break; // Exit the loop after processing the loan
//         }
//     }

//     // If the loan was not found
//     if (!loanFound) {
//         const char *notFoundMessage = "Loan not found or not assigned to you.\n";
//         write(connFD, notFoundMessage, strlen(notFoundMessage));
//     }

//     // Close the file descriptor
//     close(loanFileDescriptor);
//     return true;
// }

bool approve_or_reject_loan(int connFD) {
    char readBuffer[1000];
    ssize_t readBytes, writeBytes;

    // Prompt for the loan ID
    const char *promptMessage = "Please enter the Loan ID to approve or reject: ";
    writeBytes = write(connFD, promptMessage, strlen(promptMessage));
    if (writeBytes == -1) {
        perror("Error sending loan ID prompt to client");
        return false;
    }

    // Read the loan ID from the client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        if (readBytes == 0) {
            printf("Client disconnected while entering loan ID.\n");
        } else {
            perror("Error reading loan ID from client");
        }
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the string
    int loanID = atoi(readBuffer); // Convert to integer

    // Open the loan records file for reading and writing
    int loanFileDescriptor = open(LOAN_RECORD_FILE, O_RDWR);
    if (loanFileDescriptor == -1) {
        perror("Error opening loan records file");
        write(connFD, "Unable to access loan records.\n", 31);
        return false;
    }

    struct Loan loan;
    ssize_t bytesRead;
    bool loanFound = false;

    // Read through the loan records
    while ((bytesRead = read(loanFileDescriptor, &loan, sizeof(struct Loan))) > 0) {
        if (loan.loanID == loanID && loan.status == 1 && loan.empID == loggedInEmployee.id) {
            loanFound = true; // Loan found and assigned to the employee

            // Prompt employee for approval or rejection
            const char *approvalPrompt = "Do you want to approve or reject the loan? (a for approve, r for reject): ";
            writeBytes = write(connFD, approvalPrompt, strlen(approvalPrompt));
            if (writeBytes == -1) {
                perror("Error sending approval prompt to client");
                close(loanFileDescriptor);
                return false;
            }

            // Read the employee's decision
            bzero(readBuffer, sizeof(readBuffer));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
            if (readBytes <= 0) {
                if (readBytes == 0) {
                    printf("Client disconnected while entering decision.\n");
                } else {
                    perror("Error reading employee decision from client");
                }
                close(loanFileDescriptor);
                return false;
            }
            readBuffer[readBytes] = '\0'; // Null-terminate the string

            // Update the loan status based on employee's decision
            if (readBuffer[0] == 'a') {
                loan.status = 3; // Approved
            } else if (readBuffer[0] == 'r') {
                loan.status = 2; // Rejected
            } else {
                const char *invalidInput = "Invalid input! Please enter 'a' to approve or 'r' to reject.\n";
                write(connFD, invalidInput, strlen(invalidInput));
                close(loanFileDescriptor);
                return false;
            }

            // Move the file pointer back to the correct position to overwrite the loan record
            off_t offset = lseek(loanFileDescriptor, -sizeof(struct Loan), SEEK_CUR);
            if (offset == -1) {
                perror("Error seeking in loan records file");
                close(loanFileDescriptor);
                return false;
            }

            // Write the updated loan record back to the file
            if (write(loanFileDescriptor, &loan, sizeof(struct Loan)) == -1) {
                perror("Error updating loan record in file");
                close(loanFileDescriptor);
                return false;
            }

            // Confirmation message
            const char *successMessage = "Loan application status updated successfully.\n";
            write(connFD, successMessage, strlen(successMessage));
            break; // Exit the loop after processing the loan
        }
    }

    // If the loan was not found
    if (!loanFound) {
        const char *notFoundMessage = "Loan not found or not assigned to you.\n";
        write(connFD, notFoundMessage, strlen(notFoundMessage));
    }

    // Close the file descriptor
    close(loanFileDescriptor);
    return true;
}





// void check_loan_status(int connFD) {
//     char readBuffer[500];
//     ssize_t readBytes, writeBytes;

//     // Prompt the employee for the loan ID to check the status
//     const char *promptMessage = "Please enter the loan ID to check the status: ";
//     writeBytes = write(connFD, promptMessage, strlen(promptMessage));
//     if (writeBytes == -1) {
//         perror("Error sending prompt to client");
//         return;
//     }

//     // Read the loan ID from the employee
//     bzero(readBuffer, sizeof(readBuffer));
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
//     if (readBytes == -1) {
//         perror("Error reading loan ID from client");
//         return;
//     }
//     readBuffer[readBytes] = '\0'; // Ensure null-termination
//     int loanID = atoi(readBuffer); // Convert loan ID to integer

//     // Open the loans file for reading
//     int loanFileDescriptor = open(LOAN_RECORD_FILE, O_RDONLY);
//     if (loanFileDescriptor == -1) {
//         perror("Error opening loan records file");
//         const char *errorMessage = "Unable to access loan records.\n";
//         write(connFD, errorMessage, strlen(errorMessage));
//         return;
//     }

//     struct Loan loan;
//     ssize_t readBytesLoan;
//     bool loanFound = false;

//     // Read through the loan records to find the loan with the specified loanID
//     while ((readBytesLoan = read(loanFileDescriptor, &loan, sizeof(struct Loan))) > 0) {
//         if (loan.loanID == loanID) {
//             loanFound = true;

//             // Prepare a message based on the loan status
//             const char *statusMessage;
//             switch (loan.status) {
//                 case 0:
//                     statusMessage = "Loan Status: Request applied but no assignment by management.\n";
//                     break;
//                 case 1:
//                     statusMessage = "Loan Status: Assigned by manager to employee, pending action.\n";
//                     break;
//                 case 2:
//                     statusMessage = "Loan Status: Accepted by employee.\n";
//                     break;
//                 case 3:
//                     statusMessage = "Loan Status: Rejected by employee.\n";
//                     break;
//                 default:
//                     statusMessage = "Loan Status: Unknown status.\n";
//                     break;
//             }

//             // Send the status message to the employee
//             writeBytes = write(connFD, statusMessage, strlen(statusMessage));
//             if (writeBytes == -1) {
//                 perror("Error sending loan status to client");
//                 break;
//             }
//         }
//     }

//     if (!loanFound) {
//         const char *notFoundMessage = "Loan not found.\n";
//         write(connFD, notFoundMessage, strlen(notFoundMessage));
//     }

//     // Close the loan file descriptor
//     close(loanFileDescriptor);
// }



void check_loan_status(int connFD) {
    char readBuffer[500];
    ssize_t readBytes, writeBytes;

    // Prompt the employee for the loan ID to check the status
    const char *promptMessage = "Please enter the Loan ID to check the status: ";
    writeBytes = write(connFD, promptMessage, strlen(promptMessage));
    if (writeBytes == -1) {
        perror("Error sending loan ID prompt to client");
        return;
    }

    // Read the loan ID from the employee
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        if (readBytes == 0) {
            printf("Client disconnected while entering loan ID.\n");
        } else {
            perror("Error reading loan ID from client");
        }
        return;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input
    int loanID = atoi(readBuffer); // Convert loan ID to integer

    // Open the loans file for reading
    int loanFileDescriptor = open(LOAN_RECORD_FILE, O_RDONLY);
    if (loanFileDescriptor == -1) {
        perror("Error opening loan records file");
        write(connFD, "Unable to access loan records.\n", 31);
        return;
    }

    struct Loan loan;
    ssize_t bytesRead;
    bool loanFound = false;

    // Read through the loan records to find the loan with the specified loanID
    while ((bytesRead = read(loanFileDescriptor, &loan, sizeof(struct Loan))) > 0) {
        if (loan.loanID == loanID) {
            loanFound = true;

            // Prepare a message based on the loan status
            char statusMessage[200];
            switch (loan.status) {
                case 0:
                    snprintf(statusMessage, sizeof(statusMessage), "Loan ID: %d is assigned to a manager for review.\n", loan.loanID);
                    break;
                case 1:
                    snprintf(statusMessage, sizeof(statusMessage), "Loan ID: %d is assigned to an employee for processing.\n", loan.loanID);
                    break;
                case 2:
                    snprintf(statusMessage, sizeof(statusMessage), "Loan ID: %d has been rejected.\n", loan.loanID);
                    break;
                case 3:
                    snprintf(statusMessage, sizeof(statusMessage), "Loan ID: %d has been approved.\n", loan.loanID);
                    break;
                default:
                    snprintf(statusMessage, sizeof(statusMessage), "Loan ID: %d has an unknown status.\n", loan.loanID);
                    break;
            }

            // Send the status message to the employee
            writeBytes = write(connFD, statusMessage, strlen(statusMessage));
            if (writeBytes == -1) {
                perror("Error sending loan status to client");
            }
            break;
        }
    }

    if (bytesRead == -1) {
        perror("Error reading loan records");
        write(connFD, "Error reading loan records.\n", 28);
    }

    if (!loanFound) {
        const char *notFoundMessage = "Loan not found.\n";
        write(connFD, notFoundMessage, strlen(notFoundMessage));
    }

    // Close the loan file descriptor
    close(loanFileDescriptor);
}




void unlock_employee_critical_section(struct sembuf *semOp) {
    semOp->sem_op = 1; // Unlock operation
    semop(semIdentifier, semOp, 1);
}



// bool change_employee_password(int connFD) {
//     ssize_t readBytes, writeBytes;
//     char readBuffer[1000], newPassword[1000];

//     // Lock the critical section
//     struct sembuf semOp = {0, -1, SEM_UNDO};
//     int semopStatus = semop(semIdentifier, &semOp, 1);
//     if (semopStatus == -1) {
//         perror("Error while locking critical section");
//         return false;
//     }

//     // Prompt for old password
//     writeBytes = write(connFD, PASSWORD_CHANGE_OLD_PASS, strlen(PASSWORD_CHANGE_OLD_PASS));
//     if (writeBytes == -1) {
//         perror("Error writing PASSWORD_CHANGE_OLD_PASS message to client!");
//         unlock_employee_critical_section(&semOp);
//         return false;
//     }

//     bzero(readBuffer, sizeof(readBuffer));
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//     if (readBytes == -1) {
//         perror("Error reading old password response from client");
//         unlock_employee_critical_section(&semOp);
//         return false;
//     }

//     if (strcmp(readBuffer, loggedInManager.password) == 0) { // Check if old password matches
//         // Prompt for new password
//         writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS, strlen(PASSWORD_CHANGE_NEW_PASS));
//         if (writeBytes == -1) {
//             perror("Error writing PASSWORD_CHANGE_NEW_PASS message to client!");
//             unlock_employee_critical_section(&semOp);
//             return false;
//         }
        
//         bzero(readBuffer, sizeof(readBuffer));
//         readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//         if (readBytes == -1) {
//             perror("Error reading new password response from client");
//             unlock_employee_critical_section(&semOp);
//             return false;
//         }

//         strcpy(newPassword, readBuffer); // Store new password

//         // Prompt for re-entering new password
//         writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS_RE, strlen(PASSWORD_CHANGE_NEW_PASS_RE));
//         if (writeBytes == -1) {
//             perror("Error writing PASSWORD_CHANGE_NEW_PASS_RE message to client!");
//             unlock_employee_critical_section(&semOp);
//             return false;
//         }

//         bzero(readBuffer, sizeof(readBuffer));
//         readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//         if (readBytes == -1) {
//             perror("Error reading new password re-enter response from client");
//             unlock_employee_critical_section(&semOp);
//             return false;
//         }

//         if (strcmp(readBuffer, newPassword) == 0) { // Check if new and re-entered passwords match
//             // Update password
//             strcpy(loggedInManager.password, newPassword); 

//             // Open employee data file to update the record
//             int employeeFileDescriptor = open(EMPLOYEE_FILE, O_WRONLY);
//             if (employeeFileDescriptor == -1) {
//                 perror("Error opening employee file!");
//                 unlock_employee_critical_section(&semOp);
//                 return false;
//             }

//             off_t offset = lseek(employeeFileDescriptor, loggedInManager.id * sizeof(struct Employee), SEEK_SET);
//             if (offset == -1) {
//                 perror("Error seeking to the employee record!");
//                 close(employeeFileDescriptor);
//                 unlock_employee_critical_section(&semOp);
//                 return false;
//             }

//             struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Employee), getpid()};
//             int lockingStatus = fcntl(employeeFileDescriptor, F_SETLKW, &lock);
//             if (lockingStatus == -1) {
//                 perror("Error obtaining write lock on employee record!");
//                 close(employeeFileDescriptor);
//                 unlock_employee_critical_section(&semOp);
//                 return false;
//             }

//             // Write the updated employee record
//             writeBytes = write(employeeFileDescriptor, &loggedInManager, sizeof(struct Employee));
//             if (writeBytes == -1) {
//                 perror("Error storing updated employee password into employee record!");
//                 lock.l_type = F_UNLCK; // Unlock record
//                 fcntl(employeeFileDescriptor, F_SETLK, &lock);
//                 close(employeeFileDescriptor);
//                 unlock_employee_critical_section(&semOp);
//                 return false;
//             }

//             lock.l_type = F_UNLCK; // Unlock record
//             fcntl(employeeFileDescriptor, F_SETLK, &lock);
//             close(employeeFileDescriptor);

//             writeBytes = write(connFD, PASSWORD_CHANGE_SUCCESS, strlen(PASSWORD_CHANGE_SUCCESS));
//             readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

//             unlock_employee_critical_section(&semOp);
//             return true;
//         } else {
//             // New passwords don't match
//             writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS_INVALID, strlen(PASSWORD_CHANGE_NEW_PASS_INVALID));
//             readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
//         }
//     } else {
//         // Old password is incorrect
//         writeBytes = write(connFD, PASSWORD_CHANGE_OLD_PASS_INVALID, strlen(PASSWORD_CHANGE_OLD_PASS_INVALID));
//         readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
//     }

//     unlock_employee_critical_section(&semOp);
//     return false;
// }


bool change_employee_password(int connFD) {
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], newPassword[1000];

    // Lock the critical section
    struct sembuf semOp = {0, -1, SEM_UNDO};
    if (semop(semIdentifier, &semOp, 1) == -1) {
        perror("Error while locking critical section");
        return false;
    }

    // Prompt for old password
    if (write(connFD, PASSWORD_CHANGE_OLD_PASS, strlen(PASSWORD_CHANGE_OLD_PASS)) == -1) {
        perror("Error writing old password prompt to client");
        unlock_employee_critical_section(&semOp);
        return false;
    }

    // Read old password
    bzero(readBuffer, sizeof(readBuffer));
    if ((readBytes = read(connFD, readBuffer, sizeof(readBuffer))) <= 0) {
        perror("Error reading old password from client");
        unlock_employee_critical_section(&semOp);
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input

    // Validate old password
    if (strcmp(readBuffer, loggedInEmployee.password) != 0) {
        write(connFD, PASSWORD_CHANGE_OLD_PASS_INVALID, strlen(PASSWORD_CHANGE_OLD_PASS_INVALID));
        unlock_employee_critical_section(&semOp);
        return false;
    }

    // Prompt for new password
    if (write(connFD, PASSWORD_CHANGE_NEW_PASS, strlen(PASSWORD_CHANGE_NEW_PASS)) == -1) {
        perror("Error writing new password prompt to client");
        unlock_employee_critical_section(&semOp);
        return false;
    }

    // Read new password
    bzero(readBuffer, sizeof(readBuffer));
    if ((readBytes = read(connFD, readBuffer, sizeof(readBuffer))) <= 0) {
        perror("Error reading new password from client");
        unlock_employee_critical_section(&semOp);
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input
    strcpy(newPassword, readBuffer);

    // Prompt for re-entering new password
    if (write(connFD, PASSWORD_CHANGE_NEW_PASS_RE, strlen(PASSWORD_CHANGE_NEW_PASS_RE)) == -1) {
        perror("Error writing re-enter password prompt to client");
        unlock_employee_critical_section(&semOp);
        return false;
    }

    // Read re-entered password
    bzero(readBuffer, sizeof(readBuffer));
    if ((readBytes = read(connFD, readBuffer, sizeof(readBuffer))) <= 0) {
        perror("Error reading re-entered password from client");
        unlock_employee_critical_section(&semOp);
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input

    // Validate new passwords match
    if (strcmp(readBuffer, newPassword) != 0) {
        write(connFD, PASSWORD_CHANGE_NEW_PASS_INVALID, strlen(PASSWORD_CHANGE_NEW_PASS_INVALID));
        unlock_employee_critical_section(&semOp);
        return false;
    }

    // Update password in memory
    strcpy(loggedInEmployee.password, newPassword);

    // Open employee file to update the record
    int employeeFileDescriptor = open(EMPLOYEE_FILE, O_WRONLY);
    if (employeeFileDescriptor == -1) {
        perror("Error opening employee file");
        unlock_employee_critical_section(&semOp);
        return false;
    }

    // Seek to the employee record
    off_t offset = lseek(employeeFileDescriptor, loggedInEmployee.id * sizeof(struct Employee), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking to employee record");
        close(employeeFileDescriptor);
        unlock_employee_critical_section(&semOp);
        return false;
    }

    // Lock the employee record
    struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Employee), getpid()};
    if (fcntl(employeeFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error obtaining write lock on employee record");
        close(employeeFileDescriptor);
        unlock_employee_critical_section(&semOp);
        return false;
    }

    // Write updated employee record
    if (write(employeeFileDescriptor, &loggedInEmployee, sizeof(struct Employee)) == -1) {
        perror("Error writing updated employee record");
        lock.l_type = F_UNLCK;
        fcntl(employeeFileDescriptor, F_SETLK, &lock);
        close(employeeFileDescriptor);
        unlock_employee_critical_section(&semOp);
        return false;
    }

    // Unlock the record and close the file
    lock.l_type = F_UNLCK;
    fcntl(employeeFileDescriptor, F_SETLK, &lock);
    close(employeeFileDescriptor);

    // Notify the client of success
    write(connFD, PASSWORD_CHANGE_SUCCESS, strlen(PASSWORD_CHANGE_SUCCESS));
    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read for acknowledgment

    unlock_employee_critical_section(&semOp);
    return true;
}


#endif