#ifndef MANAGER_FUNCTIONS
#define MANAGER_FUNCTIONS

#include <sys/ipc.h>
#include <sys/sem.h>


#include "./common.h"
#include "./server-constants.h"
#include "../recordtypes/employee.h"
#include "../recordtypes/loan.h"

struct Employee loggedInManager;
int semIdentifier;

// Function Prototypes =================================


// bool update_account_in_file(struct Account *account);            // *
bool change_password(int connFD);                               // *
bool activate_account(int connFD);                              
bool deactivate_account(int connFD);
bool assign_loan_to_employee(int connFD);
void send_loan_details_to_manager(int connFD);
bool read_feedback_and_update_state(int connFD);
bool read_feedback_ids_with_state_0(int connFD);
bool change_manager_password(int connFD);
void unlock_manager_critical_section(struct sembuf *semOp);
// =====================================================

// Function Definition =================================



// bool manager_operation_handler(int connFD) {
//     if (manager_login_handler(connFD, &loggedInManager)) { 
//         ssize_t writeBytes, readBytes;            
//         char readBuffer[1000], writeBuffer[1000]; 

//         bzero(writeBuffer, sizeof(writeBuffer));
//         strcpy(writeBuffer, MANAGER_LOGIN_SUCCESS); 
//         write(connFD, writeBuffer, strlen(writeBuffer)); // Send login success message
        
//         while (1) {
//             bzero(writeBuffer, sizeof(writeBuffer));
//             strcat(writeBuffer, "\n");
//             strcat(writeBuffer, MANAGER_MENU); // Display manager menu
//             writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
//             if (writeBytes == -1) {
//                 perror("Error while writing MANAGER_MENU to client!");
//                 return false;
//             }

//             bzero(readBuffer, sizeof(readBuffer));
//             readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//             if (readBytes == -1) {
//                 perror("Error while reading client's choice for MANAGER_MENU");
//                 return false;
//             }

//             int choice = atoi(readBuffer);
//             switch (choice) {
//                 case 1:
//                     activate_account(connFD);
//                     break;
//                 case 2:
//                     deactivate_account(connFD);
//                     break;
//                 case 3:
//                     send_loan_details_to_manager(connFD);
//                     break;
//                 case 4:
//                     assign_loan_to_employee(connFD);
//                     break;
//                 case 5:
//                     read_feedback_ids_with_state_0(connFD); 
//                     break;
//                 case 6:
//                     read_feedback_and_update_state(connFD);
//                     break;
//                 case 7:
//                     change_manager_password(connFD); // Implement this function
//                     break;
//                 case 8:
//                     return false;
//                 case 9:
//                     const char *exit2 = "Exiting the application!🌟 type ok \n";
//                     ssize_t writeBytes2 = write(connFD, exit2, strlen(exit2));
//                     if (writeBytes2 == -1) {
//                         perror("Error sending exit message to client");
//                     }

//                     // Dummy read for acknowledgment from the client
//                     char readBuffer3[100];
//                     bzero(readBuffer3, sizeof(readBuffer3)); // Clear the buffer

//                     // Reading the acknowledgment from the client
//                     ssize_t readBytes3 = read(connFD, readBuffer3, sizeof(readBuffer3) - 1);
//                     if (readBytes3 == -1) {
//                         perror("Error reading acknowledgment from client");
//                     } else if (readBytes3 > 0) {
//                         readBuffer[readBytes3] = '\0'; // Null-terminate the received string
//                         printf("Received acknowledgment from client: %s\n", readBuffer3);
//                     } else {
//                         printf("No acknowledgment received from client.\n");
//                     }

//                     // Close the client connection
//                     close(connFD);
//                     return false; 
//                 default:
//                     // write(connFD, "Invalid choice! Please try again.\n", 35);
//             }
//         }
//     } else {
//         // MANAGER LOGIN FAILED
//         return false;
//     }
//     return true;
// }


bool manager_operation_handler(int connFD) {
    if (!manager_login_handler(connFD, &loggedInManager)) {
        perror("Manager login failed!");
        return false;
    }

    ssize_t writeBytes, readBytes;
    char readBuffer[1000], writeBuffer[1000];

    // Send login success message
    snprintf(writeBuffer, sizeof(writeBuffer), "%s\n", MANAGER_LOGIN_SUCCESS);
    if (write(connFD, writeBuffer, strlen(writeBuffer)) == -1) {
        perror("Error sending login success message to client");
        return false;
    }

    while (1) {
        // Display manager menu
        snprintf(writeBuffer, sizeof(writeBuffer), "\n%s", MANAGER_MENU);
        if (write(connFD, writeBuffer, strlen(writeBuffer)) == -1) {
            perror("Error sending MANAGER_MENU to client");
            return false;
        }

        // Read manager's choice
        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
        if (readBytes <= 0) {
            if (readBytes == 0) {
                printf("Client disconnected.\n");
            } else {
                perror("Error reading manager's choice");
            }
            return false;
        }
        readBuffer[readBytes] = '\0'; // Null-terminate the input

        int choice = atoi(readBuffer);
        switch (choice) {
            case 1:
                if (!activate_account(connFD)) {
                    write(connFD, "Error activating account.\n", 26);
                }
                break;
            case 2:
                if (!deactivate_account(connFD)) {
                    write(connFD, "Error deactivating account.\n", 29);
                }
                break;
            case 3:
                send_loan_details_to_manager(connFD);
                break;
            case 4:
                if (!assign_loan_to_employee(connFD)) {
                    write(connFD, "Error assigning loan to employee.\n", 34);
                }
                break;
            case 5:
                if (!read_feedback_ids_with_state_0(connFD)) {
                    write(connFD, "Error reading feedback IDs.\n", 28);
                }
                break;
            case 6:
                if (!read_feedback_and_update_state(connFD)) {
                    write(connFD, "Error reading and updating feedback.\n", 37);
                }
                break;
            case 7:
                if (!change_manager_password(connFD)) {
                    write(connFD, "Error changing password.\n", 25);
                }
                break;
            case 8:
                return false; // Logout
            case 9:
                // Exit the application
                write(connFD, "Exiting the application!🌟 type ok\n", 34);
                bzero(readBuffer, sizeof(readBuffer));
                read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read for acknowledgment
                return false;
            default:
                write(connFD, "Invalid choice! Please try again.\n", 35);
                break;
        }
    }

    return true;
}






// Helper function to read account number from the client
// int get_account_number_from_client(int connFD) {
//     char buffer[1000];
//     ssize_t readBytes, writeBytes;

//     // Ask for account number
//     strcpy(buffer, "Please enter the account number:\n");
//     writeBytes = write(connFD, buffer, strlen(buffer));
//     if (writeBytes == -1) {
//         perror("Error writing account number request to client");
//         return -1;
//     }

//     // Read account number from client
//     bzero(buffer, sizeof(buffer));
//     readBytes = read(connFD, buffer, sizeof(buffer));
//     if (readBytes == -1) {
//         perror("Error reading account number from client");
//         return -1;
//     }

//     return atoi(buffer);  // Convert input to integer (account number)
// }




// Helper function to read account number from the client
int get_account_number_from_client(int connFD) {
    char buffer[1000];
    ssize_t readBytes;

    // Prompt the client for the account number
    const char *promptMessage = "Please enter the account number:\n";
    if (write(connFD, promptMessage, strlen(promptMessage)) == -1) {
        perror("Error writing account number request to client");
        return -1;
    }

    // Read the account number from the client
    bzero(buffer, sizeof(buffer));
    readBytes = read(connFD, buffer, sizeof(buffer) - 1); // Leave space for null terminator
    if (readBytes <= 0) {
        if (readBytes == 0) {
            printf("Client disconnected while entering account number.\n");
        } else {
            perror("Error reading account number from client");
        }
        return -1;
    }
    buffer[readBytes] = '\0'; // Null-terminate the input

    // Convert the input to an integer and return
    return atoi(buffer);
}




// Function to activate an account (gets input from client)
// bool activate_account(int connFD) {
//     char buffer[1000];
//     int accountNumber = get_account_number_from_client(connFD);
//     if (accountNumber == -1) {
//         return false;
//     }

//     int accountFileFD = open(ACCOUNT_FILE, O_RDWR);
//     if (accountFileFD == -1) {
//         perror("Error opening account file");
//         return false;
//     }

//     struct Account account;
//     off_t offset = lseek(accountFileFD, accountNumber * sizeof(struct Account), SEEK_SET);
//     if (offset == -1) {
//         perror("Error seeking account record");
//         close(accountFileFD);
//         return false;
//     }

//     ssize_t readBytes = read(accountFileFD, &account, sizeof(struct Account));
//     if (readBytes == -1) {
//         perror("Error reading account record");
//         close(accountFileFD);
//         return false;
//     }

//     if (!account.active) {
//         account.active = true;

//         // Write back the updated account record
//         lseek(accountFileFD, offset, SEEK_SET);
//         ssize_t writeBytes = write(accountFileFD, &account, sizeof(struct Account));
//         if (writeBytes == -1) {
//             perror("Error writing updated account record");
//             close(accountFileFD);
//             return false;
//         }

//         strcpy(buffer, "Account activated successfully.\n");
//         write(connFD, buffer, strlen(buffer));
//     } else {
//         strcpy(buffer, "Account is already active.\n");
//         write(connFD, buffer, strlen(buffer));
//     }

//     close(accountFileFD);
//     return true;
// }



// Function to activate an account (gets input from client)
bool activate_account(int connFD) {
    char buffer[1000];
    int accountNumber = get_account_number_from_client(connFD);
    if (accountNumber == -1) {
        return false; // Failed to get account number
    }

    // Open the account file for reading and writing
    int accountFileFD = open(ACCOUNT_FILE, O_RDWR);
    if (accountFileFD == -1) {
        perror("Error opening account file");
        write(connFD, "Unable to access account records.\n", 34);
        return false;
    }

    struct Account account;
    off_t offset = lseek(accountFileFD, accountNumber * sizeof(struct Account), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking account record");
        write(connFD, "Account not found.\n", 20);
        close(accountFileFD);
        return false;
    }

    // Read the account record
    ssize_t readBytes = read(accountFileFD, &account, sizeof(struct Account));
    if (readBytes == -1) {
        perror("Error reading account record");
        write(connFD, "Error reading account details.\n", 31);
        close(accountFileFD);
        return false;
    }

    // Check if the account is already active
    if (account.active) {
        const char *alreadyActiveMessage = "Account is already active.\n";
        write(connFD, alreadyActiveMessage, strlen(alreadyActiveMessage));
        close(accountFileFD);
        return true;
    }

    // Activate the account
    account.active = true;

    // Write the updated account record back to the file
    lseek(accountFileFD, offset, SEEK_SET);
    ssize_t writeBytes = write(accountFileFD, &account, sizeof(struct Account));
    if (writeBytes == -1) {
        perror("Error writing updated account record");
        write(connFD, "Error updating account status.\n", 31);
        close(accountFileFD);
        return false;
    }

    // Notify the client of success
    const char *successMessage = "Account activated successfully.\n";
    write(connFD, successMessage, strlen(successMessage));

    close(accountFileFD);
    return true;
}



// Function to deactivate an account (gets input from client)
// bool deactivate_account(int connFD) {
//     char buffer[1000];
//     int accountNumber = get_account_number_from_client(connFD);
//     if (accountNumber == -1) {
//         return false;
//     }

//     int accountFileFD = open(ACCOUNT_FILE, O_RDWR);
//     if (accountFileFD == -1) {
//         perror("Error opening account file");
//         return false;
//     }

//     struct Account account;
//     off_t offset = lseek(accountFileFD, accountNumber * sizeof(struct Account), SEEK_SET);
//     if (offset == -1) {
//         perror("Error seeking account record");
//         close(accountFileFD);
//         return false;
//     }

//     ssize_t readBytes = read(accountFileFD, &account, sizeof(struct Account));
//     if (readBytes == -1) {
//         perror("Error reading account record");
//         close(accountFileFD);
//         return false;
//     }

//     if (account.active) {
//         account.active = false;

//         // Write back the updated account record
//         lseek(accountFileFD, offset, SEEK_SET);
//         ssize_t writeBytes = write(accountFileFD, &account, sizeof(struct Account));
//         if (writeBytes == -1) {
//             perror("Error writing updated account record");
//             close(accountFileFD);
//             return false;
//         }

//         strcpy(buffer, "Account deactivated successfully.\n");
//         write(connFD, buffer, strlen(buffer));
//     } else {
//         strcpy(buffer, "Account is already inactive.\n");
//         write(connFD, buffer, strlen(buffer));
//     }

//     close(accountFileFD);
//     return true;
// }


bool deactivate_account(int connFD) {
    char buffer[1000];
    ssize_t writeBytes, readBytes;

    // Get the account number from the client
    int accountNumber = get_account_number_from_client(connFD);
    if (accountNumber == -1) {
        write(connFD, "Invalid account number.\n", 24);
        return false;
    }

    // Open the account file for reading and writing
    int accountFileFD = open(ACCOUNT_FILE, O_RDWR);
    if (accountFileFD == -1) {
        perror("Error opening account file");
        write(connFD, "Unable to access account records.\n", 34);
        return false;
    }

    struct Account account;
    off_t offset = lseek(accountFileFD, accountNumber * sizeof(struct Account), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking account record");
        write(connFD, "Account not found.\n", 20);
        close(accountFileFD);
        return false;
    }

    // Read the account record
    readBytes = read(accountFileFD, &account, sizeof(struct Account));
    if (readBytes == -1) {
        perror("Error reading account record");
        write(connFD, "Error reading account details.\n", 31);
        close(accountFileFD);
        return false;
    }

    // Check if the account is already inactive
    if (!account.active) {
        const char *alreadyInactiveMessage = "Account is already inactive.\n";
        write(connFD, alreadyInactiveMessage, strlen(alreadyInactiveMessage));
        close(accountFileFD);
        return true;
    }

    // Deactivate the account
    account.active = false;

    // Write the updated account record back to the file
    lseek(accountFileFD, offset, SEEK_SET);
    ssize_t writeBytes = write(accountFileFD, &account, sizeof(struct Account));
    if (writeBytes == -1) {
        perror("Error writing updated account record");
        write(connFD, "Error updating account status.\n", 31);
        close(accountFileFD);
        return false;
    }

    // Notify the client of success
    const char *successMessage = "Account deactivated successfully.\n";
    write(connFD, successMessage, strlen(successMessage));

    close(accountFileFD);
    return true;
}




// bool assign_loan_to_employee(int connFD) {
//     char readBuffer[500];
//     ssize_t readBytes, writeBytes;

//     // Prompt the client to enter the loan ID
//     const char *promptMessage = "Please enter the loan ID you want to assign: ";
//     writeBytes = write(connFD, promptMessage, strlen(promptMessage));
//     if (writeBytes == -1) {
//         perror("Error sending loan ID prompt to client");
//         return false;
//     }

//     // Read the loan ID from the client
//     bzero(readBuffer, sizeof(readBuffer));
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
//     if (readBytes == -1) {
//         perror("Error reading loan ID from client");
//         return false;
//     }
//     readBuffer[readBytes] = '\0'; // Null-terminate the input
//     int loanID = atoi(readBuffer); // Convert loan ID to integer

//     // Prompt for employee ID
//     promptMessage = "Please enter the employee ID to assign the loan: ";
//     writeBytes = write(connFD, promptMessage, strlen(promptMessage));
//     if (writeBytes == -1) {
//         perror("Error sending employee ID prompt to client");
//         return false;
//     }

//     // Read the employee ID from the client
//     bzero(readBuffer, sizeof(readBuffer));
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
//     if (readBytes == -1) {
//         perror("Error reading employee ID from client");
//         return false;
//     }
//     readBuffer[readBytes] = '\0'; // Null-terminate the input
//     int employeeID = atoi(readBuffer); // Convert employee ID to integer

//     // Open loan record file for reading and writing
//     int loanFileDescriptor = open(LOAN_RECORD_FILE, O_RDWR);
//     if (loanFileDescriptor == -1) {
//         perror("Error opening loan records file");
//         const char *errorMessage = "Unable to access loan records.\n";
//         write(connFD, errorMessage, strlen(errorMessage));
//         return false;
//     }

//     struct Loan loan;
//     ssize_t readBytesLoan;
//     bool loanFound = false;

//     // Iterate through the loan records to find the loan with the given loanID
//     while ((readBytesLoan = read(loanFileDescriptor, &loan, sizeof(struct Loan))) > 0) {
//         if (loan.loanID == loanID) {
//             loanFound = true;

//             // Check if the loan is currently assigned to the manager (status 0)
//             if (loan.status == 0) {
//                 // Update loan to be assigned to the employee (status 1)
//                 loan.status = 1;
//                 loan.empID = employeeID; // Assign employee ID

//                 // Seek back to the position of the loan record
//                 off_t offset = lseek(loanFileDescriptor, -sizeof(struct Loan), SEEK_CUR);
//                 if (offset == -1) {
//                     perror("Error seeking loan record for update");
//                     close(loanFileDescriptor);
//                     return false;
//                 }

//                 // Lock the loan record to update
//                 struct flock writeLock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Loan), getpid()};
//                 if (fcntl(loanFileDescriptor, F_SETLKW, &writeLock) == -1) {
//                     perror("Error locking loan record for writing");
//                     close(loanFileDescriptor);
//                     return false;
//                 }

//                 // Write the updated loan record
//                 if (write(loanFileDescriptor, &loan, sizeof(struct Loan)) == -1) {
//                     perror("Error updating loan record");
//                     writeLock.l_type = F_UNLCK;
//                     fcntl(loanFileDescriptor, F_SETLK, &writeLock);
//                     close(loanFileDescriptor);
//                     return false;
//                 }

//                 // Unlock the record after updating
//                 writeLock.l_type = F_UNLCK;
//                 fcntl(loanFileDescriptor, F_SETLK, &writeLock);

//                 // Send success message to client
//                 const char *successMessage = "Loan has been successfully assigned to the employee.\n type ok ";
//                 write(connFD, successMessage, strlen(successMessage));
//             } else {
//                 const char *errorMessage = "Loan is already assigned or processed.\n";
//                 write(connFD, errorMessage, strlen(errorMessage));
//             }
//             break; // Loan found and processed, exit loop
//         }
//     }

//     if (!loanFound) {
//         const char *notFoundMessage = "Loan not found.\n";
//         write(connFD, notFoundMessage, strlen(notFoundMessage));
//     }

//     // Close the loan file descriptor
//     close(loanFileDescriptor);
//     return loanFound;
// }


bool assign_loan_to_employee(int connFD) {
    char readBuffer[500];
    ssize_t readBytes;

    // Prompt the client to enter the loan ID
    const char *loanPrompt = "Please enter the loan ID you want to assign: ";
    if (write(connFD, loanPrompt, strlen(loanPrompt)) == -1) {
        perror("Error sending loan ID prompt to client");
        return false;
    }

    // Read the loan ID from the client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        perror("Error reading loan ID from client");
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input
    int loanID = atoi(readBuffer); // Convert loan ID to integer

    // Prompt for employee ID
    const char *employeePrompt = "Please enter the employee ID to assign the loan: ";
    if (write(connFD, employeePrompt, strlen(employeePrompt)) == -1) {
        perror("Error sending employee ID prompt to client");
        return false;
    }

    // Read the employee ID from the client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        perror("Error reading employee ID from client");
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input
    int employeeID = atoi(readBuffer); // Convert employee ID to integer

    // Open loan record file for reading and writing
    int loanFileDescriptor = open(LOAN_RECORD_FILE, O_RDWR);
    if (loanFileDescriptor == -1) {
        perror("Error opening loan records file");
        write(connFD, "Unable to access loan records.\n", 31);
        return false;
    }

    struct Loan loan;
    bool loanFound = false;

    // Iterate through the loan records to find the loan with the given loanID
    while (read(loanFileDescriptor, &loan, sizeof(struct Loan)) > 0) {
        if (loan.loanID == loanID) {
            loanFound = true;

            // Check if the loan is currently assigned to the manager (status 0)
            if (loan.status == 0) {
                // Update loan to be assigned to the employee (status 1)
                loan.status = 1;
                loan.empID = employeeID;

                // Seek back to the position of the loan record
                off_t offset = lseek(loanFileDescriptor, -sizeof(struct Loan), SEEK_CUR);
                if (offset == -1) {
                    perror("Error seeking loan record for update");
                    close(loanFileDescriptor);
                    return false;
                }

                // Lock the loan record to update
                struct flock writeLock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Loan), getpid()};
                if (fcntl(loanFileDescriptor, F_SETLKW, &writeLock) == -1) {
                    perror("Error locking loan record for writing");
                    close(loanFileDescriptor);
                    return false;
                }

                // Write the updated loan record
                if (write(loanFileDescriptor, &loan, sizeof(struct Loan)) == -1) {
                    perror("Error updating loan record");
                    writeLock.l_type = F_UNLCK;
                    fcntl(loanFileDescriptor, F_SETLK, &writeLock);
                    close(loanFileDescriptor);
                    return false;
                }

                // Unlock the record after updating
                writeLock.l_type = F_UNLCK;
                fcntl(loanFileDescriptor, F_SETLK, &writeLock);

                // Send success message to client
                write(connFD, "Loan has been successfully assigned to the employee.\n", 53);
            } else {
                write(connFD, "Loan is already assigned or processed.\n", 39);
            }
            break; // Loan found and processed, exit loop
        }
    }

    if (!loanFound) {
        write(connFD, "Loan not found.\n", 17);
    }

    // Close the loan file descriptor
    close(loanFileDescriptor);
    return loanFound;
}





// void send_loan_details_to_manager(int connFD) {
//     int loanFileDescriptor = open(LOAN_RECORD_FILE, O_RDONLY);
//     if (loanFileDescriptor == -1) {
//         perror("Error opening loan records file");
//         const char *errorMessage = "Unable to access loan records.\n";
//         write(connFD, errorMessage, strlen(errorMessage));
//         return;
//     }

//     struct Loan loan;
//     ssize_t readBytes;
//     char buffer[500]; // Buffer for sending loan details
//     int count = 0; // To count the number of loans
    
//     // // Prepare header for the loan details
//     // const char *header = "Loans Assigned to Manager:\n";
    
//     // write(connFD, header, strlen(header));
//     // char dummyBuffer[256];
//     // bzero(dummyBuffer, sizeof(dummyBuffer));
//     // read(connFD, dummyBuffer, sizeof(dummyBuffer)); 
    
//     // Read each loan record and check the status
//     while ((readBytes = read(loanFileDescriptor, &loan, sizeof(struct Loan))) > 0) {
//         printf("Inside the loan file");
//         if (loan.status == 0) { // Only process loans assigned to the manager
//             // Prepare loan detail message
//             snprintf(buffer, sizeof(buffer), "Loan ID: %d, Customer ID: %d, Amount: %ld\n",
//                      loan.loanID, loan.custID, loan.amount);
//             write(connFD, buffer, strlen(buffer));
//             bzero(buffer,sizeof(buffer));
//             usleep(100000);
//             count++;
//         }
//     }

//     // If there were no loans, inform the manager
//     if (count == 0) {
//         const char *noLoansMessage = "No loans assigned to manager.\n";
//         write(connFD, noLoansMessage, strlen(noLoansMessage));
//     }

//     // Close the loan file descriptor
//     close(loanFileDescriptor);

//     const char *endMessage = "type ok "; // Change this message as needed
//     write(connFD, endMessage, strlen(endMessage));
//     read(connFD, buffer, sizeof(buffer));
// }

void send_loan_details_to_manager(int connFD) {
    int loanFileDescriptor = open(LOAN_RECORD_FILE, O_RDONLY);
    if (loanFileDescriptor == -1) {
        perror("Error opening loan records file");
        const char *errorMessage = "Unable to access loan records.\n";
        write(connFD, errorMessage, strlen(errorMessage));
        return;
    }

    struct Loan loan;
    ssize_t readBytes;
    char buffer[500]; // Buffer for sending loan details
    int count = 0;    // To count the number of loans

    // Read each loan record and check the status
    while ((readBytes = read(loanFileDescriptor, &loan, sizeof(struct Loan))) > 0) {
        if (loan.status == 0) { // Only process loans assigned to the manager
            // Prepare loan detail message
            snprintf(buffer, sizeof(buffer), "Loan ID: %d, Customer ID: %d, Amount: %d\n",
                     loan.loanID, loan.custID, loan.amount);
            if (write(connFD, buffer, strlen(buffer)) == -1) {
                perror("Error sending loan details to client");
                close(loanFileDescriptor);
                return;
            }
            bzero(buffer, sizeof(buffer)); // Clear the buffer after use
            count++;
        }
    }

    if (readBytes == -1) {
        perror("Error reading loan records");
        write(connFD, "Error reading loan records.\n", 28);
        close(loanFileDescriptor);
        return;
    }

    // If there were no loans, inform the manager
    if (count == 0) {
        const char *noLoansMessage = "No loans assigned to manager.\n";
        write(connFD, noLoansMessage, strlen(noLoansMessage));
    }

    // Close the loan file descriptor
    close(loanFileDescriptor);

    // Notify the client that the operation is complete
    const char *endMessage = "type ok ";
    write(connFD, endMessage, strlen(endMessage));
}




// bool read_feedback_ids_with_state_0(int connFD) {
//     // Open the feedback file in read-only mode
//     int feedbackFileDescriptor = open("records/feedback.bank", O_RDONLY);
//     if (feedbackFileDescriptor == -1) {
//         perror("Error opening feedback file for reading");
//         return false;
//     }

//     struct Feedback feedback;
//     char buffer[1024]; // Buffer to store all the unread feedback IDs to send to the client
//     bzero(buffer, sizeof(buffer)); // Clear the buffer
//     bool hasUnreadFeedback = false; // Track if any unread feedback exists

//     // Read each feedback record from the file
//     while (read(feedbackFileDescriptor, &feedback, sizeof(struct Feedback)) == sizeof(struct Feedback)) {
//         if (feedback.state == 0) {
//             hasUnreadFeedback = true;

//             // Append the feedback ID to the buffer
//             char tempBuffer[100];
//             snprintf(tempBuffer, sizeof(tempBuffer), "Unread Feedback ID: %d\n", feedback.id);
//             strcat(buffer, tempBuffer);
//         }
//     }

//     // If there are unread feedbacks, send them all at once
//     if (hasUnreadFeedback) {
//         if (write(connFD, buffer, strlen(buffer)) == -1) {
//             perror("Error sending feedback IDs to client");
//             close(feedbackFileDescriptor);
//             return false;
//         }
//     } else {
//         // If no unread feedback is found, send a message indicating so
//         char noFeedbackMessage[] = "No unread feedback available.\n";
//         if (write(connFD, noFeedbackMessage, strlen(noFeedbackMessage)) == -1) {
//             perror("Error sending no feedback message to client");
//             close(feedbackFileDescriptor);
//             return false;
//         }
//     }

//     // Close the file descriptor
//     close(feedbackFileDescriptor);

//     return true;
// }


bool read_feedback_ids_with_state_0(int connFD) {
    // Open the feedback file in read-only mode
    int feedbackFileDescriptor = open("records/feedback.bank", O_RDONLY);
    if (feedbackFileDescriptor == -1) {
        perror("Error opening feedback file for reading");
        write(connFD, "Unable to access feedback records.\n", 35);
        return false;
    }

    struct Feedback feedback;
    char buffer[1024]; // Buffer to store all the unread feedback IDs to send to the client
    bzero(buffer, sizeof(buffer)); // Clear the buffer
    bool hasUnreadFeedback = false; // Track if any unread feedback exists

    // Read each feedback record from the file
    while (read(feedbackFileDescriptor, &feedback, sizeof(struct Feedback)) == sizeof(struct Feedback)) {
        if (feedback.state == 0) { // Check if feedback is unread
            hasUnreadFeedback = true;

            // Append the feedback ID to the buffer
            char tempBuffer[100];
            snprintf(tempBuffer, sizeof(tempBuffer), "Unread Feedback ID: %d\n", feedback.id);
            strncat(buffer, tempBuffer, sizeof(buffer) - strlen(buffer) - 1); // Ensure no buffer overflow
        }
    }

    // Handle errors during reading
    if (errno != 0 && errno != EOF) {
        perror("Error reading feedback records");
        write(connFD, "Error reading feedback records.\n", 32);
        close(feedbackFileDescriptor);
        return false;
    }

    // If there are unread feedbacks, send them all at once
    if (hasUnreadFeedback) {
        if (write(connFD, buffer, strlen(buffer)) == -1) {
            perror("Error sending feedback IDs to client");
            close(feedbackFileDescriptor);
            return false;
        }
    } else {
        // If no unread feedback is found, send a message indicating so
        const char *noFeedbackMessage = "No unread feedback available.\n";
        if (write(connFD, noFeedbackMessage, strlen(noFeedbackMessage)) == -1) {
            perror("Error sending no feedback message to client");
            close(feedbackFileDescriptor);
            return false;
        }
    }

    // Close the file descriptor
    close(feedbackFileDescriptor);
    return true;
}






// bool read_feedback_and_update_state(int connFD) {
//     char readBuffer[100];
//     ssize_t readBytes, writeBytes;
//     int feedbackId;

//     // Ask the client to provide the feedback ID
//     const char *promptMessage = "Please enter the Feedback ID you wish to read and update: ";
//     writeBytes = write(connFD, promptMessage, strlen(promptMessage));
//     if (writeBytes == -1) {
//         perror("Error sending prompt to client");
//         return false;
//     }

//     // Read the feedback ID from the client
//     bzero(readBuffer, sizeof(readBuffer));
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
//     if (readBytes == -1) {
//         perror("Error reading feedback ID from client");
//         return false;
//     }
//     readBuffer[readBytes] = '\0'; // Ensure null-termination

//     // Convert the received feedback ID to an integer
//     feedbackId = atoi(readBuffer);
//     if (feedbackId < 0) { // Allow 0 as a valid ID, reject only negative numbers
//         const char *errorMessage = "Invalid Feedback ID provided. Please enter a valid numeric ID.\n";
//         write(connFD, errorMessage, strlen(errorMessage));
//         return false;
//     }

//     // Open the feedback file for reading and writing
//     int feedbackFileDescriptor = open("records/feedback.bank", O_RDWR);
//     if (feedbackFileDescriptor == -1) {
//         perror("Error opening feedback file for reading/writing");
//         const char *errorMessage = "Internal error. Cannot access feedback records.\n";
//         write(connFD, errorMessage, strlen(errorMessage));
//         return false;
//     }

//     struct Feedback feedback;
//     off_t offset;
//     bool found = false;

//     // Iterate over each record to find the matching ID
//     while ((offset = lseek(feedbackFileDescriptor, 0, SEEK_CUR)) != -1 &&
//            read(feedbackFileDescriptor, &feedback, sizeof(struct Feedback)) == sizeof(struct Feedback)) {
//         if (feedback.id == feedbackId) {
//             // Found the feedback, display it to the client
//             char feedbackMessage[600];
//             snprintf(feedbackMessage, sizeof(feedbackMessage),
//                      "Feedback ID: %d\nAccount: %d\nMessage: %s\ntype ok ",
//                      feedback.id, feedback.account, feedback.message);

//             // Send the feedback details to the client
//             writeBytes = write(connFD, feedbackMessage, strlen(feedbackMessage));
//             if (writeBytes == -1) {
//                 perror("Error sending feedback message to client");
//                 close(feedbackFileDescriptor);
//                 return false;
//             }

//             // Update the state to 1 (manager has read)
//             feedback.state = 1;

//             // Set write lock to modify this record
//             struct flock writeLock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Feedback), getpid()};
//             if (fcntl(feedbackFileDescriptor, F_SETLKW, &writeLock) == -1) {
//                 perror("Error obtaining write lock on Feedback record");
//                 close(feedbackFileDescriptor);
//                 return false;
//             }

//             // Write the updated feedback back to the same position
//             lseek(feedbackFileDescriptor, offset, SEEK_SET);
//             if (write(feedbackFileDescriptor, &feedback, sizeof(struct Feedback)) == -1) {
//                 perror("Error updating Feedback record state");
//                 writeLock.l_type = F_UNLCK;
//                 fcntl(feedbackFileDescriptor, F_SETLK, &writeLock);
//                 close(feedbackFileDescriptor);
//                 return false;
//             }

//             // Release the write lock
//             writeLock.l_type = F_UNLCK;
//             fcntl(feedbackFileDescriptor, F_SETLK, &writeLock);

//             // Send confirmation back to the client
//             // const char *successMessage = "Feedback has been successfully marked as read.\n";
//             // write(connFD, successMessage, strlen(successMessage));

//             // // Optionally, prompt the user for the next action
//             // const char *nextActionMessage = "You may proceed with your next action: ";
//             // write(connFD, nextActionMessage, strlen(nextActionMessage));



//             found = true;
//             break;
//         }
//     }

//     // If no feedback was found with the provided ID, notify the client
//     if (!found) {
//         const char *notFoundMessage = "Feedback ID not found.\n";
//         write(connFD, notFoundMessage, strlen(notFoundMessage));
//     }

//     // Close the file descriptor
//     close(feedbackFileDescriptor);

//     return found;
// }


bool read_feedback_and_update_state(int connFD) {
    char readBuffer[100];
    ssize_t readBytes, writeBytes;
    int feedbackId;

    // Prompt the client to provide the feedback ID
    const char *promptMessage = "Please enter the Feedback ID you wish to read and update: ";
    if (write(connFD, promptMessage, strlen(promptMessage)) == -1) {
        perror("Error sending prompt to client");
        return false;
    }

    // Read the feedback ID from the client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        if (readBytes == 0) {
            printf("Client disconnected while entering Feedback ID.\n");
        } else {
            perror("Error reading feedback ID from client");
        }
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input

    // Convert the received feedback ID to an integer
    feedbackId = atoi(readBuffer);
    if (feedbackId < 0) { // Reject negative IDs
        const char *errorMessage = "Invalid Feedback ID provided. Please enter a valid numeric ID.\n";
        write(connFD, errorMessage, strlen(errorMessage));
        return false;
    }

    // Open the feedback file for reading and writing
    int feedbackFileDescriptor = open("records/feedback.bank", O_RDWR);
    if (feedbackFileDescriptor == -1) {
        perror("Error opening feedback file for reading/writing");
        write(connFD, "Internal error. Cannot access feedback records.\n", 48);
        return false;
    }

    struct Feedback feedback;
    off_t offset;
    bool found = false;

    // Iterate over each record to find the matching ID
    while ((offset = lseek(feedbackFileDescriptor, 0, SEEK_CUR)) != -1 &&
           read(feedbackFileDescriptor, &feedback, sizeof(struct Feedback)) == sizeof(struct Feedback)) {
        if (feedback.id == feedbackId) {
            found = true;

            // Display the feedback to the client
            char feedbackMessage[600];
            snprintf(feedbackMessage, sizeof(feedbackMessage),
                     "Feedback ID: %d\nAccount: %d\nMessage: %s\ntype ok ",
                     feedback.id, feedback.account, feedback.message);
            if (write(connFD, feedbackMessage, strlen(feedbackMessage)) == -1) {
                perror("Error sending feedback message to client");
                close(feedbackFileDescriptor);
                return false;
            }

            // Update the state to 1 (manager has read)
            feedback.state = 1;

            // Set write lock to modify this record
            struct flock writeLock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Feedback), getpid()};
            if (fcntl(feedbackFileDescriptor, F_SETLKW, &writeLock) == -1) {
                perror("Error obtaining write lock on Feedback record");
                close(feedbackFileDescriptor);
                return false;
            }

            // Write the updated feedback back to the same position
            lseek(feedbackFileDescriptor, offset, SEEK_SET);
            if (write(feedbackFileDescriptor, &feedback, sizeof(struct Feedback)) == -1) {
                perror("Error updating Feedback record state");
                writeLock.l_type = F_UNLCK;
                fcntl(feedbackFileDescriptor, F_SETLK, &writeLock);
                close(feedbackFileDescriptor);
                return false;
            }

            // Release the write lock
            writeLock.l_type = F_UNLCK;
            fcntl(feedbackFileDescriptor, F_SETLK, &writeLock);

            break;
        }
    }

    // If no feedback was found with the provided ID, notify the client
    if (!found) {
        write(connFD, "Feedback ID not found.\n", 23);
    }

    // Close the file descriptor
    close(feedbackFileDescriptor);

    return found;
}




void unlock_manager_critical_section(struct sembuf *semOp) {
    semOp->sem_op = 1; // Unlock operation
    semop(semIdentifier, semOp, 1);
}




// bool change_manager_password(int connFD) {
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
//         unlock_manager_critical_section(&semOp);
//         return false;
//     }

//     bzero(readBuffer, sizeof(readBuffer));
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//     if (readBytes == -1) {
//         perror("Error reading old password response from client");
//         unlock_manager_critical_section(&semOp);
//         return false;
//     }

//     if (strcmp(readBuffer, loggedInManager.password) == 0) { // Check if old password matches
//         // Prompt for new password
//         writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS, strlen(PASSWORD_CHANGE_NEW_PASS));
//         if (writeBytes == -1) {
//             perror("Error writing PASSWORD_CHANGE_NEW_PASS message to client!");
//             unlock_manager_critical_section(&semOp);
//             return false;
//         }
        
//         bzero(readBuffer, sizeof(readBuffer));
//         readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//         if (readBytes == -1) {
//             perror("Error reading new password response from client");
//             unlock_manager_critical_section(&semOp);
//             return false;
//         }

//         strcpy(newPassword, readBuffer); // Store new password

//         // Prompt for re-entering new password
//         writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS_RE, strlen(PASSWORD_CHANGE_NEW_PASS_RE));
//         if (writeBytes == -1) {
//             perror("Error writing PASSWORD_CHANGE_NEW_PASS_RE message to client!");
//             unlock_manager_critical_section(&semOp);
//             return false;
//         }

//         bzero(readBuffer, sizeof(readBuffer));
//         readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//         if (readBytes == -1) {
//             perror("Error reading new password re-enter response from client");
//             unlock_manager_critical_section(&semOp);
//             return false;
//         }

//         if (strcmp(readBuffer, newPassword) == 0) { 
//             // Update password
//             strcpy(loggedInManager.password, newPassword); 

//             // Open employee data file to update the record
//             int employeeFileDescriptor = open(EMPLOYEE_FILE, O_WRONLY);
//             if (employeeFileDescriptor == -1) {
//                 perror("Error opening employee file!");
//                 unlock_manager_critical_section(&semOp);
//                 return false;
//             }

//             off_t offset = lseek(employeeFileDescriptor, loggedInManager.id * sizeof(struct Employee), SEEK_SET);
//             if (offset == -1) {
//                 perror("Error seeking to the employee record!");
//                 close(employeeFileDescriptor);
//                 unlock_manager_critical_section(&semOp);
//                 return false;
//             }

//             struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Employee), getpid()};
//             int lockingStatus = fcntl(employeeFileDescriptor, F_SETLKW, &lock);
//             if (lockingStatus == -1) {
//                 perror("Error obtaining write lock on employee record!");
//                 close(employeeFileDescriptor);
//                 unlock_manager_critical_section(&semOp);
//                 return false;
//             }

//             // Write the updated employee record
//             writeBytes = write(employeeFileDescriptor, &loggedInManager, sizeof(struct Employee));
//             if (writeBytes == -1) {
//                 perror("Error storing updated employee password into employee record!");
//                 lock.l_type = F_UNLCK; // Unlock record
//                 fcntl(employeeFileDescriptor, F_SETLK, &lock);
//                 close(employeeFileDescriptor);
//                 unlock_manager_critical_section(&semOp);
//                 return false;
//             }

//             lock.l_type = F_UNLCK; // Unlock record
//             fcntl(employeeFileDescriptor, F_SETLK, &lock);
//             close(employeeFileDescriptor);

//             writeBytes = write(connFD, PASSWORD_CHANGE_SUCCESS, strlen(PASSWORD_CHANGE_SUCCESS));
//             readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

//             unlock_manager_critical_section(&semOp);
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

//     unlock_manager_critical_section(&semOp);
//     return false;
// }

bool change_manager_password(int connFD) {
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
        perror("Error writing PASSWORD_CHANGE_OLD_PASS message to client!");
        unlock_manager_critical_section(&semOp);
        return false;
    }

    // Read old password
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes <= 0) {
        perror("Error reading old password response from client");
        unlock_manager_critical_section(&semOp);
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input

    // Validate old password
    if (strcmp(readBuffer, loggedInManager.password) != 0) {
        write(connFD, PASSWORD_CHANGE_OLD_PASS_INVALID, strlen(PASSWORD_CHANGE_OLD_PASS_INVALID));
        unlock_manager_critical_section(&semOp);
        return false;
    }

    // Prompt for new password
    if (write(connFD, PASSWORD_CHANGE_NEW_PASS, strlen(PASSWORD_CHANGE_NEW_PASS)) == -1) {
        perror("Error writing PASSWORD_CHANGE_NEW_PASS message to client!");
        unlock_manager_critical_section(&semOp);
        return false;
    }

    // Read new password
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes <= 0) {
        perror("Error reading new password response from client");
        unlock_manager_critical_section(&semOp);
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input
    strcpy(newPassword, readBuffer);

    // Prompt for re-entering new password
    if (write(connFD, PASSWORD_CHANGE_NEW_PASS_RE, strlen(PASSWORD_CHANGE_NEW_PASS_RE)) == -1) {
        perror("Error writing PASSWORD_CHANGE_NEW_PASS_RE message to client!");
        unlock_manager_critical_section(&semOp);
        return false;
    }

    // Read re-entered password
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes <= 0) {
        perror("Error reading new password re-enter response from client");
        unlock_manager_critical_section(&semOp);
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input

    // Validate new passwords match
    if (strcmp(readBuffer, newPassword) != 0) {
        write(connFD, PASSWORD_CHANGE_NEW_PASS_INVALID, strlen(PASSWORD_CHANGE_NEW_PASS_INVALID));
        unlock_manager_critical_section(&semOp);
        return false;
    }

    // Update password in memory
    strcpy(loggedInManager.password, newPassword);

    // Open employee file to update the record
    int employeeFileDescriptor = open(EMPLOYEE_FILE, O_WRONLY);
    if (employeeFileDescriptor == -1) {
        perror("Error opening employee file!");
        unlock_manager_critical_section(&semOp);
        return false;
    }

    // Seek to the manager's record
    off_t offset = lseek(employeeFileDescriptor, loggedInManager.id * sizeof(struct Employee), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking to the employee record!");
        close(employeeFileDescriptor);
        unlock_manager_critical_section(&semOp);
        return false;
    }

    // Lock the record for writing
    struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Employee), getpid()};
    if (fcntl(employeeFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error obtaining write lock on employee record!");
        close(employeeFileDescriptor);
        unlock_manager_critical_section(&semOp);
        return false;
    }

    // Write the updated record
    if (write(employeeFileDescriptor, &loggedInManager, sizeof(struct Employee)) == -1) {
        perror("Error storing updated employee password into employee record!");
        lock.l_type = F_UNLCK;
        fcntl(employeeFileDescriptor, F_SETLK, &lock);
        close(employeeFileDescriptor);
        unlock_manager_critical_section(&semOp);
        return false;
    }

    // Unlock the record and close the file
    lock.l_type = F_UNLCK;
    fcntl(employeeFileDescriptor, F_SETLK, &lock);
    close(employeeFileDescriptor);

    // Notify the client of success
    write(connFD, PASSWORD_CHANGE_SUCCESS, strlen(PASSWORD_CHANGE_SUCCESS));
    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read for acknowledgment

    unlock_manager_critical_section(&semOp);
    return true;
}


#endif