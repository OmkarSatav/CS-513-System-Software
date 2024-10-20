#ifndef MANAGER_FUNCTIONS
#define MANAGER_FUNCTIONS

// Semaphores are necessary joint account due the design choice I've made
#include <sys/ipc.h>
#include <sys/sem.h>


#include "./common.h"
#include "./server-constants.h"
#include "../recordtypes/employee.h"
#include "../recordtypes/loan.h"

struct Employee loggedInManager;
int semIdentifier;

// Function Prototypes =================================


bool update_account_in_file(struct Account *account);
bool change_password(int connFD);
bool activate_account(int connFD);
bool deactivate_account(int connFD);
bool assign_loan_to_employee(int connFD);
void send_loan_details_to_manager(int connFD);
bool read_feedback_and_update_state(int connFD);
bool read_feedback_ids_with_state_0(int connFD);
// =====================================================

// Function Definition =================================




bool manager_operation_handler(int connFD) {
    // Call the manager login handler
    if (manager_login_handler(connFD, &loggedInManager)) { // Assume `true` indicates Manager login
        ssize_t writeBytes, readBytes;            // Number of bytes read from / written to the client
        char readBuffer[1000], writeBuffer[1000]; // Buffers for reading & writing to the client

        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, MANAGER_LOGIN_SUCCESS); // Message for successful login
        while (1) {
            strcat(writeBuffer, "\n");
            strcat(writeBuffer, MANAGER_MENU); // Assume you have a predefined MANAGER_MENU
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1) {
                perror("Error while writing MANAGER_MENU to client!");
                return false;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            bzero(readBuffer, sizeof(readBuffer));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer));
            if (readBytes == -1) {
                perror("Error while reading client's choice for MANAGER_MENU");
                return false;
            }

            int choice = atoi(readBuffer);
            switch (choice) {
                case 1:
                    activate_account(connFD);
                    break;
                case 2:
                    deactivate_account(connFD);
                    break;
                case 3:
                    read_feedback_ids_with_state_0(connFD); 
                    read_feedback_and_update_state(connFD);
                    break;
                case 4:
                    // change_password(connFD); // Implement this function
                    break;
                case 5:
                    // writeBytes = write(connFD, MANAGER_LOGOUT, strlen(MANAGER_LOGOUT)); // Assume you have this constant
                    return false; // Logout
                case 6:
                    send_loan_details_to_manager(connFD);
                    break;
                case 7:
                    assign_loan_to_employee(connFD);
                    break;
                default:
                    writeBytes = write(connFD, "Invalid choice! Please try again.\n", 36);
                    if (writeBytes == -1) {
                        perror("Error sending invalid choice message to client!");
                        return false;
                    }
            }
        }
    } else {
        // MANAGER LOGIN FAILED
        return false;
    }
    return true;
}



// Helper function to read account number from the client
int get_account_number_from_client(int connFD) {
    char buffer[1000];
    ssize_t readBytes, writeBytes;

    // Ask for account number
    strcpy(buffer, "Please enter the account number:\n");
    writeBytes = write(connFD, buffer, strlen(buffer));
    if (writeBytes == -1) {
        perror("Error writing account number request to client");
        return -1;
    }

    // Read account number from client
    bzero(buffer, sizeof(buffer));
    readBytes = read(connFD, buffer, sizeof(buffer));
    if (readBytes == -1) {
        perror("Error reading account number from client");
        return -1;
    }

    return atoi(buffer);  // Convert input to integer (account number)
}

// Function to activate an account (gets input from client)
bool activate_account(int connFD) {
    char buffer[1000];
    int accountNumber = get_account_number_from_client(connFD);
    if (accountNumber == -1) {
        return false;
    }

    int accountFileFD = open(ACCOUNT_FILE, O_RDWR);
    if (accountFileFD == -1) {
        perror("Error opening account file");
        return false;
    }

    struct Account account;
    off_t offset = lseek(accountFileFD, accountNumber * sizeof(struct Account), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking account record");
        close(accountFileFD);
        return false;
    }

    ssize_t readBytes = read(accountFileFD, &account, sizeof(struct Account));
    if (readBytes == -1) {
        perror("Error reading account record");
        close(accountFileFD);
        return false;
    }

    if (!account.active) {
        account.active = true;

        // Write back the updated account record
        lseek(accountFileFD, offset, SEEK_SET);
        ssize_t writeBytes = write(accountFileFD, &account, sizeof(struct Account));
        if (writeBytes == -1) {
            perror("Error writing updated account record");
            close(accountFileFD);
            return false;
        }

        strcpy(buffer, "Account activated successfully.\n");
        write(connFD, buffer, strlen(buffer));
    } else {
        strcpy(buffer, "Account is already active.\n");
        write(connFD, buffer, strlen(buffer));
    }

    close(accountFileFD);
    return true;
}

// Function to deactivate an account (gets input from client)
bool deactivate_account(int connFD) {
    char buffer[1000];
    int accountNumber = get_account_number_from_client(connFD);
    if (accountNumber == -1) {
        return false;
    }

    int accountFileFD = open(ACCOUNT_FILE, O_RDWR);
    if (accountFileFD == -1) {
        perror("Error opening account file");
        return false;
    }

    struct Account account;
    off_t offset = lseek(accountFileFD, accountNumber * sizeof(struct Account), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking account record");
        close(accountFileFD);
        return false;
    }

    ssize_t readBytes = read(accountFileFD, &account, sizeof(struct Account));
    if (readBytes == -1) {
        perror("Error reading account record");
        close(accountFileFD);
        return false;
    }

    if (account.active) {
        account.active = false;

        // Write back the updated account record
        lseek(accountFileFD, offset, SEEK_SET);
        ssize_t writeBytes = write(accountFileFD, &account, sizeof(struct Account));
        if (writeBytes == -1) {
            perror("Error writing updated account record");
            close(accountFileFD);
            return false;
        }

        strcpy(buffer, "Account deactivated successfully.\n");
        write(connFD, buffer, strlen(buffer));
    } else {
        strcpy(buffer, "Account is already inactive.\n");
        write(connFD, buffer, strlen(buffer));
    }

    close(accountFileFD);
    return true;
}



bool assign_loan_to_employee(int connFD) {
    char readBuffer[500];
    ssize_t readBytes, writeBytes;

    // Prompt the client to enter the loan ID and employee ID
    const char *promptMessage = "Please enter the loan ID you want to assign: ";
    writeBytes = write(connFD, promptMessage, strlen(promptMessage));
    if (writeBytes == -1) {
        perror("Error sending prompt to client");
        return false;
    }

    // Read the loan ID from the client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes == -1) {
        perror("Error reading loan ID from client");
        return false;
    }
    readBuffer[readBytes] = '\0'; // Ensure null-termination
    int loanID = atoi(readBuffer); // Convert loan ID to integer

    // Prompt for employee ID
    promptMessage = "Please enter the employee ID to assign the loan: ";
    writeBytes = write(connFD, promptMessage, strlen(promptMessage));
    if (writeBytes == -1) {
        perror("Error sending prompt to client");
        return false;
    }

    // Read the employee ID from the client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes == -1) {
        perror("Error reading employee ID from client");
        return false;
    }
    readBuffer[readBytes] = '\0'; // Ensure null-termination
    int employeeID = atoi(readBuffer); // Convert employee ID to integer

    int loanFileDescriptor = open(LOAN_RECORD_FILE, O_RDWR);
    if (loanFileDescriptor == -1) {
        perror("Error opening loan records file");
        const char *errorMessage = "Unable to access loan records.\n";
        write(connFD, errorMessage, strlen(errorMessage));
        return false;
    }

    struct Loan loan;
    ssize_t readBytesLoan;
    bool loanFound = false;

    // Read through the loan records to find the loan with the specified loanID
    while ((readBytesLoan = read(loanFileDescriptor, &loan, sizeof(struct Loan))) > 0) {
        if (loan.loanID == loanID) {
            loanFound = true;

            // Check if the loan is currently assigned to the manager
            if (loan.status == 0) {
                // Update the loan status to 1 (assigned to employee)
                loan.status = 1; // Assigning to employee
                loan.empID = employeeID; // Set the empID to the assigned employee ID

                // Move the file pointer back to the position of the loan record
                off_t offset = lseek(loanFileDescriptor, -sizeof(struct Loan), SEEK_CUR);
                if (offset == -1) {
                    perror("Error seeking to loan record");
                    close(loanFileDescriptor);
                    return false;
                }

                // Write lock to ensure exclusive access while updating the loan status
                struct flock writeLock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Loan), getpid()};
                if (fcntl(loanFileDescriptor, F_SETLKW, &writeLock) == -1) {
                    perror("Error obtaining write lock on Loan file");
                    close(loanFileDescriptor);
                    return false;
                }

                // Update the loan record in the file
                if (write(loanFileDescriptor, &loan, sizeof(struct Loan)) == -1) {
                    perror("Error writing updated Loan record to file");
                    writeLock.l_type = F_UNLCK;
                    fcntl(loanFileDescriptor, F_SETLK, &writeLock);
                    close(loanFileDescriptor);
                    return false;
                }

                // Release the write lock
                writeLock.l_type = F_UNLCK;
                fcntl(loanFileDescriptor, F_SETLK, &writeLock);

                // Send success message back to the client
                const char *successMessage = "Loan has been successfully assigned to the employee.\n";
                write(connFD, successMessage, strlen(successMessage));
            } else {
                // If the loan status is not 0, it means it's already assigned or processed
                const char *errorMessage = "Loan is already assigned to another employee or processed.\n";
                write(connFD, errorMessage, strlen(errorMessage));
            }
            break; // Exit loop once the loan is found
        }
    }

    if (!loanFound) {
        const char *notFoundMessage = "Loan not found.\n";
        write(connFD, notFoundMessage, strlen(notFoundMessage));
    }

    // Close the loan file descriptor
    close(loanFileDescriptor);
    return true;
}




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
    int count = 0; // To count the number of loans

    // Prepare header for the loan details
    const char *header = "Loans Assigned to Manager:\n";
    write(connFD, header, strlen(header));

    // Read each loan record and check the status
    while ((readBytes = read(loanFileDescriptor, &loan, sizeof(struct Loan))) > 0) {
        if (loan.status == 0) { // Only process loans assigned to the manager
            // Prepare loan detail message
            snprintf(buffer, sizeof(buffer), "Loan ID: %d, Customer ID: %d, Amount: %ld\n",
                     loan.loanID, loan.custID, loan.amount);
            write(connFD, buffer, strlen(buffer));
            count++;
        }
    }

    // If there were no loans, inform the manager
    if (count == 0) {
        const char *noLoansMessage = "No loans assigned to manager.\n";
        write(connFD, noLoansMessage, strlen(noLoansMessage));
    }

    // Close the loan file descriptor
    close(loanFileDescriptor);

    // Send a termination message to indicate the end of loan details
    const char *endMessage = "End of loan details. Press Enter to continue.\n";
    write(connFD, endMessage, strlen(endMessage));
}




bool read_feedback_ids_with_state_0(int connFD) {
    // Open the feedback file in read-only mode
    int feedbackFileDescriptor = open("records/feedback.bank", O_RDONLY);
    if (feedbackFileDescriptor == -1) {
        perror("Error opening feedback file for reading");
        return false;
    }

    struct Feedback feedback;
    char buffer[100]; // Buffer to store the ID and send to the client

    // Read each feedback record from the file
    while (read(feedbackFileDescriptor, &feedback, sizeof(struct Feedback)) == sizeof(struct Feedback)) {
        if (feedback.state == 0) {
            // Prepare the message to send the feedback ID to the client
            snprintf(buffer, sizeof(buffer), "Unread Feedback ID: %d\n", feedback.id);

            // Send the ID to the client
            if (write(connFD, buffer, strlen(buffer)) == -1) {
                perror("Error sending feedback ID to client");
                close(feedbackFileDescriptor);
                return false;
            }
        }
    }

    // Close the file descriptor
    close(feedbackFileDescriptor);

    return true;
}

bool read_feedback_and_update_state(int connFD) {
    char readBuffer[100];
    ssize_t readBytes, writeBytes;
    int feedbackId;

    // Ask the client to provide the feedback ID
    const char *promptMessage = "Please enter the Feedback ID you wish to read and update: ";
    writeBytes = write(connFD, promptMessage, strlen(promptMessage));
    if (writeBytes == -1) {
        perror("Error sending prompt to client");
        return false;
    }

    // Read the feedback ID from the client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes == -1) {
        perror("Error reading feedback ID from client");
        return false;
    }
    readBuffer[readBytes] = '\0'; // Ensure null-termination

    // Convert the received feedback ID to an integer
    feedbackId = atoi(readBuffer);
    if (feedbackId < 0) { // Allow 0 as a valid ID, reject only negative numbers
        const char *errorMessage = "Invalid Feedback ID provided. Please enter a valid numeric ID.\n";
        write(connFD, errorMessage, strlen(errorMessage));
        return false;
    }

    // Open the feedback file for reading and writing
    int feedbackFileDescriptor = open("records/feedback.bank", O_RDWR);
    if (feedbackFileDescriptor == -1) {
        perror("Error opening feedback file for reading/writing");
        const char *errorMessage = "Internal error. Cannot access feedback records.\n";
        write(connFD, errorMessage, strlen(errorMessage));
        return false;
    }

    struct Feedback feedback;
    off_t offset;
    bool found = false;

    // Iterate over each record to find the matching ID
    while ((offset = lseek(feedbackFileDescriptor, 0, SEEK_CUR)) != -1 &&
           read(feedbackFileDescriptor, &feedback, sizeof(struct Feedback)) == sizeof(struct Feedback)) {
        if (feedback.id == feedbackId) {
            // Found the feedback, display it to the client
            char feedbackMessage[600];
            snprintf(feedbackMessage, sizeof(feedbackMessage),
                     "Feedback ID: %d\nAccount: %d\nMessage: %s\n",
                     feedback.id, feedback.account, feedback.message);

            // Send the feedback details to the client
            writeBytes = write(connFD, feedbackMessage, strlen(feedbackMessage));
            if (writeBytes == -1) {
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

            // Send confirmation back to the client
            const char *successMessage = "Feedback has been successfully marked as read.\n";
            write(connFD, successMessage, strlen(successMessage));

            found = true;
            break;
        }
    }

    // If no feedback was found with the provided ID, notify the client
    if (!found) {
        const char *notFoundMessage = "Feedback ID not found.\n";
        write(connFD, notFoundMessage, strlen(notFoundMessage));
    }

    // Close the file descriptor
    close(feedbackFileDescriptor);

    return found;
}


#endif