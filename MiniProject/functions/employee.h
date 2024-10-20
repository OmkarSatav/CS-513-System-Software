#ifndef EMPLOYEE_FUNCTIONS
#define EMPLOYEE_FUNCTIONS

// Semaphores are necessary joint account due the design choice I've made
#include <sys/ipc.h>
#include <sys/sem.h>


#include "./common.h"
#include "./server-constants.h"
#include "../recordtypes/employee.h"
#include "../recordtypes/loan.h"

struct Employee loggedInEmployee;
int semIdentifier;


// Function Prototypes =================================
bool update_account_in_file(struct Account *account);
bool change_password(int connFD);
bool activate_account(int connFD);
bool deactivate_account(int connFD);
bool assign_loan_to_employee(int connFD);
void send_loan_details_to_manager(int connFD);
bool handle_loan_decision(int connFD);
bool approve_or_reject_loan(int connFD);
void check_loan_status(int connFD);
// =====================================================
// Function Definition =================================




bool employee_operation_handler(int connFD) {
    // Call the manager login handler
    if (employee_login_handler(connFD, &loggedInEmployee)) { // Assume `true` indicates Manager login
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
                    // review_customer_feedback(connFD); // Implement this function
                    break;
                case 4:
                    // change_password(connFD); // Implement this function
                    break;
                case 5:
                    // writeBytes = write(connFD, MANAGER_LOGOUT, strlen(MANAGER_LOGOUT)); // Assume you have this constant
                    return false; // Logout
                case 6:
                    send_loans_assigned_to_employee(connFD);
                    break;
                case 7:
                    // handle_loan_decision(connFD);
                    approve_or_reject_loan(connFD);
                    break;
                case 8:
                    check_loan_status(connFD);
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





void send_loans_assigned_to_employee(int connFD) {
    int employeeID = loggedInEmployee.id; // Assuming this holds the current employee's ID
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
    int count = 0; // To count the number of loans assigned to the employee

    // Prepare header for the loan details
    const char *header = "Loans Assigned to You:\n";
    write(connFD, header, strlen(header));

    // Read each loan record and check the status and employee ID
    while ((readBytes = read(loanFileDescriptor, &loan, sizeof(struct Loan))) > 0) {
        if (loan.status == 1 && loan.empID == employeeID) { // Check if loan is assigned to this employee
            // Prepare loan detail message
            snprintf(buffer, sizeof(buffer), "Loan ID: %d, Account ID: %d, Amount: %d\n",
                     loan.loanID, loan.custID, loan.amount);
            write(connFD, buffer, strlen(buffer));
            count++;
        }
    }

    if (readBytes == -1) {
        perror("Error reading loan records");
    }

    if (count == 0) {
        const char *noLoansMessage = "No loans assigned to you.\n";
        write(connFD, noLoansMessage, strlen(noLoansMessage));
    }

    // Close the loan file descriptor
    close(loanFileDescriptor);
}





// Function to approve or reject a loan
bool approve_or_reject_loan(int connFD) {
    // Prompt for the loan ID
    const char *promptMessage = "Please enter the Loan ID to approve or reject: ";
    write(connFD, promptMessage, strlen(promptMessage));

    char readBuffer[100];
    ssize_t readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        perror("Error reading loan ID from client");
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the string
    int loanID = atoi(readBuffer); // Convert to integer

    // Open the loan records file for reading and writing
    int loanFileDescriptor = open(LOAN_RECORD_FILE, O_RDWR);
    if (loanFileDescriptor == -1) {
        perror("Error opening loan records file");
        const char *errorMessage = "Unable to access loan records.\n";
        write(connFD, errorMessage, strlen(errorMessage));
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
            write(connFD, approvalPrompt, strlen(approvalPrompt));

            // Read the employee's decision
            bzero(readBuffer, sizeof(readBuffer));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
            if (readBytes <= 0) {
                perror("Error reading employee decision from client");
                close(loanFileDescriptor);
                return false;
            }
            readBuffer[readBytes] = '\0'; // Null-terminate the string

            // Update the loan status based on employee's decision
            if (readBuffer[0] == 'a') {
                loan.status = 2; // Approved
            } else if (readBuffer[0] == 'r') {
                loan.status = 3; // Rejected
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




void check_loan_status(int connFD) {
    char readBuffer[500];
    ssize_t readBytes, writeBytes;

    // Prompt the employee for the loan ID to check the status
    const char *promptMessage = "Please enter the loan ID to check the status: ";
    writeBytes = write(connFD, promptMessage, strlen(promptMessage));
    if (writeBytes == -1) {
        perror("Error sending prompt to client");
        return;
    }

    // Read the loan ID from the employee
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes == -1) {
        perror("Error reading loan ID from client");
        return;
    }
    readBuffer[readBytes] = '\0'; // Ensure null-termination
    int loanID = atoi(readBuffer); // Convert loan ID to integer

    // Open the loans file for reading
    int loanFileDescriptor = open(LOAN_RECORD_FILE, O_RDONLY);
    if (loanFileDescriptor == -1) {
        perror("Error opening loan records file");
        const char *errorMessage = "Unable to access loan records.\n";
        write(connFD, errorMessage, strlen(errorMessage));
        return;
    }

    struct Loan loan;
    ssize_t readBytesLoan;
    bool loanFound = false;

    // Read through the loan records to find the loan with the specified loanID
    while ((readBytesLoan = read(loanFileDescriptor, &loan, sizeof(struct Loan))) > 0) {
        if (loan.loanID == loanID) {
            loanFound = true;

            // Prepare a message based on the loan status
            const char *statusMessage;
            switch (loan.status) {
                case 0:
                    statusMessage = "Loan Status: Request applied but no assignment by management.\n";
                    break;
                case 1:
                    statusMessage = "Loan Status: Assigned by manager to employee, pending action.\n";
                    break;
                case 2:
                    statusMessage = "Loan Status: Accepted by employee.\n";
                    break;
                case 3:
                    statusMessage = "Loan Status: Rejected by employee.\n";
                    break;
                default:
                    statusMessage = "Loan Status: Unknown status.\n";
                    break;
            }

            // Send the status message to the employee
            writeBytes = write(connFD, statusMessage, strlen(statusMessage));
            if (writeBytes == -1) {
                perror("Error sending loan status to client");
                break;
            }
        }
    }

    if (!loanFound) {
        const char *notFoundMessage = "Loan not found.\n";
        write(connFD, notFoundMessage, strlen(notFoundMessage));
    }

    // Close the loan file descriptor
    close(loanFileDescriptor);
}

#endif;