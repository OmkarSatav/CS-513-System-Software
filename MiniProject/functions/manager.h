#ifndef MANAGER_FUNCTIONS
#define MANAGER_FUNCTIONS

#include <sys/ipc.h>
#include <sys/sem.h>


#include "./common.h"
#include "./server-constants.h"
#include "../recordtypes/employee.h"
#include "../recordtypes/loan.h"

struct Employee loggedInManager;


// Function Prototypes =================================



bool activate_account(int connFD);                              
bool deactivate_account(int connFD);
bool assign_loan_to_employee(int connFD);
void send_loan_details_to_manager(int connFD);
bool read_feedback_and_update_state(int connFD);
bool read_feedback_ids_with_state_0(int connFD);
bool change_manager_password(int connFD);
// =====================================================

// Function Definition =================================




bool manager_operation_handler(int connFD) {
    if (!manager_login_handler(connFD, &loggedInManager)) {
        perror("Manager login failed!");
        return false;
    }

    ssize_t writeBytes, readBytes;
    char readBuffer[1000], writeBuffer[1000];

    // Send login success message
    snprintf(writeBuffer, sizeof(writeBuffer), "%s\n", MANAGER_LOGIN_SUCCESS);
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));

    while (1) {
        // Display manager menu
        snprintf(writeBuffer, sizeof(writeBuffer), "\n%s", MANAGER_MENU);
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));

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
                    snprintf(writeBuffer, sizeof(writeBuffer), "Error activating account.\n^");
                    write(connFD, writeBuffer, strlen(writeBuffer));
                    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
                }
                break;
            case 2:
                if (!deactivate_account(connFD)) {
                    snprintf(writeBuffer, sizeof(writeBuffer), "Error deactivating account.\n^");
                    write(connFD, writeBuffer, strlen(writeBuffer));
                    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
                }
                break;
            case 3:
                send_loan_details_to_manager(connFD);
                break;
            case 4:
                if (!assign_loan_to_employee(connFD)) {
                    snprintf(writeBuffer, sizeof(writeBuffer), "Error assigning loan to employee.\n^");
                    write(connFD, writeBuffer, strlen(writeBuffer));
                    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
                }
                break;
            case 5:
                if (!read_feedback_ids_with_state_0(connFD)) {
                    snprintf(writeBuffer, sizeof(writeBuffer), "Error reading feedback IDs.\n^");
                    write(connFD, writeBuffer, strlen(writeBuffer));
                    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
                }
                break;
            case 6:
                if (!read_feedback_and_update_state(connFD)) {
                    snprintf(writeBuffer, sizeof(writeBuffer), "Error reading and updating feedback.\n^");
                    write(connFD, writeBuffer, strlen(writeBuffer));
                    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
                }
                break;
            case 7:
                if (!change_manager_password(connFD)) {
                    snprintf(writeBuffer, sizeof(writeBuffer), "Error changing password.\n^");
                    write(connFD, writeBuffer, strlen(writeBuffer));
                    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
                }
                break;
            case 8:
                return false; // Logout
            case 9:
                snprintf(writeBuffer, sizeof(writeBuffer), "Exiting the application!🌟 ^\n");
                write(connFD, writeBuffer, strlen(writeBuffer));
                bzero(readBuffer, sizeof(readBuffer));
                read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
                return false;
            default:
                snprintf(writeBuffer, sizeof(writeBuffer), "Invalid choice! Please try again.\n^");
                write(connFD, writeBuffer, strlen(writeBuffer));
                read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
                break;
        }
    }

    return true;
}




int get_account_number_from_client(int connFD) {
    char buffer[1000];
    ssize_t readBytes;

    const char *promptMessage = "Please enter the account number:\n";
    if (write(connFD, promptMessage, strlen(promptMessage)) == -1) {
        perror("Error writing account number request to client");
        return -1;
    }

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
    buffer[readBytes] = '\0';

    return atoi(buffer);
}




bool activate_account(int connFD) {
    char buffer[1000];
    char readBuffer[1000];

    int accountNumber = get_account_number_from_client(connFD);
    if (accountNumber == -1) {
        return false; // Failed to get account number
    }

    // Open the account file for reading and writing
    int accountFileFD = open(ACCOUNT_FILE, O_RDWR);
    if (accountFileFD == -1) {
        perror("Error opening account file");
        sprintf(buffer, "Unable to access account records.\n^");
        write(connFD, buffer, strlen(buffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    struct Account account;
    off_t offset = lseek(accountFileFD, accountNumber * sizeof(struct Account), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking account record");
        sprintf(buffer, "Account not found.\n^");
        write(connFD, buffer, strlen(buffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        close(accountFileFD);
        return false;
    }

    // Read the account record
    ssize_t readBytes = read(accountFileFD, &account, sizeof(struct Account));
    if (readBytes == -1) {
        perror("Error reading account record");
        sprintf(buffer, "Error reading account details.\n^");
        write(connFD, buffer, strlen(buffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        close(accountFileFD);
        return false;
    }

    // Check if the account is already active
    if (account.active) {
        sprintf(buffer, "Account is already active.\n^");
        write(connFD, buffer, strlen(buffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
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
        sprintf(buffer, "Error updating account status.\n^");
        write(connFD, buffer, strlen(buffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        close(accountFileFD);
        return false;
    }

    // Notify the client of success
    sprintf(buffer, "Account activated successfully.\n^");
    write(connFD, buffer, strlen(buffer));
    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    close(accountFileFD);
    return true;
}




bool deactivate_account(int connFD) {
    char buffer[1000];
    char readBuffer[1000];
    ssize_t readBytes;

    // Get the account number from the client
    int accountNumber = get_account_number_from_client(connFD);
    if (accountNumber == -1) {
        sprintf(buffer, "Invalid account number.\n^");
        write(connFD, buffer, strlen(buffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    // Open the account file for reading and writing
    int accountFileFD = open(ACCOUNT_FILE, O_RDWR);
    if (accountFileFD == -1) {
        perror("Error opening account file");
        sprintf(buffer, "Unable to access account records.\n^");
        write(connFD, buffer, strlen(buffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    struct Account account;
    off_t offset = lseek(accountFileFD, accountNumber * sizeof(struct Account), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking account record");
        sprintf(buffer, "Account not found.\n^");
        write(connFD, buffer, strlen(buffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        close(accountFileFD);
        return false;
    }

    // Read the account record
    readBytes = read(accountFileFD, &account, sizeof(struct Account));
    if (readBytes == -1) {
        perror("Error reading account record");
        sprintf(buffer, "Error reading account details.\n^");
        write(connFD, buffer, strlen(buffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        close(accountFileFD);
        return false;
    }

    // Check if the account is already inactive
    if (!account.active) {
        sprintf(buffer, "Account is already inactive.\n^");
        write(connFD, buffer, strlen(buffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
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
        sprintf(buffer, "Error updating account status.\n^");
        write(connFD, buffer, strlen(buffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        close(accountFileFD);
        return false;
    }

    // Notify the client of success
    sprintf(buffer, "Account deactivated successfully.\n^");
    write(connFD, buffer, strlen(buffer));
    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    close(accountFileFD);
    return true;
}




bool assign_loan_to_employee(int connFD) {
    char readBuffer[500], writeBuffer[500];
    ssize_t readBytes;

    // Prompt for Loan ID
    sprintf(writeBuffer, "Please enter the loan ID you want to assign: ");
    write(connFD, writeBuffer, strlen(writeBuffer));

    // Read loan ID from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        perror("Error reading loan ID from client");
        sprintf(writeBuffer, "Error reading loan ID.\n^");
        write(connFD, writeBuffer, strlen(writeBuffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // dummy read
        return false;
    }
    readBuffer[readBytes] = '\0';
    int loanID = atoi(readBuffer);

    // Prompt for Employee ID
    sprintf(writeBuffer, "Please enter the employee ID to assign the loan: ");
    write(connFD, writeBuffer, strlen(writeBuffer));

    // Read employee ID from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        perror("Error reading employee ID from client");
        sprintf(writeBuffer, "Error reading employee ID.\n^");
        write(connFD, writeBuffer, strlen(writeBuffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // dummy read
        return false;
    }
    readBuffer[readBytes] = '\0';
    int employeeID = atoi(readBuffer);

    // Open loan record file
    int loanFileDescriptor = open(LOAN_RECORD_FILE, O_RDWR);
    if (loanFileDescriptor == -1) {
        perror("Error opening loan records file");
        sprintf(writeBuffer, "Unable to access loan records.\n^");
        write(connFD, writeBuffer, strlen(writeBuffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // dummy read
        return false;
    }

    struct Loan loan;
    bool loanFound = false;

    // Search for the loan
    while (read(loanFileDescriptor, &loan, sizeof(struct Loan)) > 0) {
        if (loan.loanID == loanID) {
            loanFound = true;

            if (loan.status == 0) {
                loan.status = 1;
                loan.empID = employeeID;

                off_t offset = lseek(loanFileDescriptor, -sizeof(struct Loan), SEEK_CUR);
                if (offset == -1) {
                    perror("Error seeking loan record for update");
                    sprintf(writeBuffer, "Error accessing loan record.\n^");
                    write(connFD, writeBuffer, strlen(writeBuffer));
                    read(connFD, readBuffer, sizeof(readBuffer));
                    close(loanFileDescriptor);
                    return false;
                }

                // Lock the record
                struct flock writeLock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Loan), getpid()};
                if (fcntl(loanFileDescriptor, F_SETLKW, &writeLock) == -1) {
                    perror("Error locking loan record for writing");
                    sprintf(writeBuffer, "Error locking loan record.\n^");
                    write(connFD, writeBuffer, strlen(writeBuffer));
                    read(connFD, readBuffer, sizeof(readBuffer));
                    close(loanFileDescriptor);
                    return false;
                }

                if (write(loanFileDescriptor, &loan, sizeof(struct Loan)) == -1) {
                    perror("Error updating loan record");
                    sprintf(writeBuffer, "Error updating loan record.\n^");
                    write(connFD, writeBuffer, strlen(writeBuffer));
                    read(connFD, readBuffer, sizeof(readBuffer));

                    writeLock.l_type = F_UNLCK;
                    fcntl(loanFileDescriptor, F_SETLK, &writeLock);
                    close(loanFileDescriptor);
                    return false;
                }

                // Unlock the record
                writeLock.l_type = F_UNLCK;
                fcntl(loanFileDescriptor, F_SETLK, &writeLock);

                sprintf(writeBuffer, "Loan has been successfully assigned to the employee.\n^");
                write(connFD, writeBuffer, strlen(writeBuffer));
                read(connFD, readBuffer, sizeof(readBuffer));
            } else {
                sprintf(writeBuffer, "Loan is already assigned or processed.\n^");
                write(connFD, writeBuffer, strlen(writeBuffer));
                read(connFD, readBuffer, sizeof(readBuffer));
            }
            break;
        }
    }

    if (!loanFound) {
        sprintf(writeBuffer, "Loan not found.\n^");
        write(connFD, writeBuffer, strlen(writeBuffer));
        read(connFD, readBuffer, sizeof(readBuffer));
    }

    close(loanFileDescriptor);
    return loanFound;
}




void send_loan_details_to_manager(int connFD) {
    char buffer[500];
    ssize_t readBytes;

    int loanFileDescriptor = open(LOAN_RECORD_FILE, O_RDONLY);
    if (loanFileDescriptor == -1) {
        perror("Error opening loan records file");
        sprintf(buffer, "Unable to access loan records.\n^");
        write(connFD, buffer, strlen(buffer));
        read(connFD, buffer, sizeof(buffer)); // dummy read
        return;
    }

    struct Loan loan;
    int count = 0;

    while ((readBytes = read(loanFileDescriptor, &loan, sizeof(struct Loan))) > 0) {
        if (loan.status == 0) {
            sprintf(buffer, "Loan ID: %d, Customer ID: %d, Amount: %d\n^",
                    loan.loanID, loan.custID, loan.amount);
            if (write(connFD, buffer, strlen(buffer)) == -1) {
                perror("Error sending loan details to client");
                close(loanFileDescriptor);
                return;
            }
            bzero(buffer, sizeof(buffer));
            read(connFD, buffer, sizeof(buffer));
            count++;
        }
    }

    if (readBytes == -1) {
        perror("Error reading loan records");
        sprintf(buffer, "Error reading loan records.\n^");
        write(connFD, buffer, strlen(buffer));
        read(connFD, buffer, sizeof(buffer)); // dummy read
        close(loanFileDescriptor);
        return;
    }

    if (count == 0) {
        sprintf(buffer, "No loans assigned to manager.\n^");
        write(connFD, buffer, strlen(buffer));
        read(connFD, buffer, sizeof(buffer)); // dummy read
    }

    close(loanFileDescriptor);
}




bool read_feedback_ids_with_state_0(int connFD) {
    char buffer[1024];
    ssize_t readBytes;

    int feedbackFileDescriptor = open("records/feedback.bank", O_RDONLY);
    if (feedbackFileDescriptor == -1) {
        perror("Error opening feedback file for reading");
        sprintf(buffer, "Unable to access feedback records.\n^");
        write(connFD, buffer, strlen(buffer));
        read(connFD, buffer, sizeof(buffer)); // Dummy read
        return false;
    }

    struct Feedback feedback;
    bzero(buffer, sizeof(buffer));
    bool hasUnreadFeedback = false;

    while ((readBytes = read(feedbackFileDescriptor, &feedback, sizeof(struct Feedback))) == sizeof(struct Feedback)) {
        if (feedback.state == 0) {
            hasUnreadFeedback = true;
            char tempBuffer[100];
            snprintf(tempBuffer, sizeof(tempBuffer), "Unread Feedback ID: %d\n", feedback.id);
            strncat(buffer, tempBuffer, sizeof(buffer) - strlen(buffer) - 1);
        }
    }

    if (readBytes == -1) {
        perror("Error reading feedback records");
        sprintf(buffer, "Error reading feedback records.\n^");
        write(connFD, buffer, strlen(buffer));
        read(connFD, buffer, sizeof(buffer)); // Dummy read
        close(feedbackFileDescriptor);
        return false;
    }

    if (hasUnreadFeedback) {
        strncat(buffer, "^", sizeof(buffer) - strlen(buffer) - 1); // Add delimiter
        write(connFD, buffer, strlen(buffer));
        read(connFD, buffer, sizeof(buffer)); // Dummy read
    } else {
        sprintf(buffer, "No unread feedback available.\n^");
        write(connFD, buffer, strlen(buffer));
        read(connFD, buffer, sizeof(buffer)); // Dummy read
    }

    close(feedbackFileDescriptor);
    return true;
}




bool read_feedback_and_update_state(int connFD) {
    char readBuffer[100], writeBuffer[600];
    ssize_t readBytes;
    int feedbackId;

    // Prompt the client to provide the feedback ID
    sprintf(writeBuffer, "Please enter the Feedback ID you wish to read and update: ");
    write(connFD, writeBuffer, strlen(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));
    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read (includes ^)

    // Convert the received feedback ID to an integer
    feedbackId = atoi(readBuffer);
    if (feedbackId < 0) {
        sprintf(writeBuffer, "Invalid Feedback ID provided. Please enter a valid numeric ID.\n^");
        write(connFD, writeBuffer, strlen(writeBuffer));
        bzero(readBuffer, sizeof(readBuffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    // Open the feedback file for reading and writing
    int feedbackFileDescriptor = open("records/feedback.bank", O_RDWR);
    if (feedbackFileDescriptor == -1) {
        perror("Error opening feedback file for reading/writing");
        sprintf(writeBuffer, "Internal error. Cannot access feedback records.\n^");
        write(connFD, writeBuffer, strlen(writeBuffer));
        bzero(readBuffer, sizeof(readBuffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    struct Feedback feedback;
    off_t offset;
    bool found = false;

    // Iterate over each record to find the matching ID
    while (read(feedbackFileDescriptor, &feedback, sizeof(struct Feedback)) == sizeof(struct Feedback)) {
        offset = lseek(feedbackFileDescriptor, -sizeof(struct Feedback), SEEK_CUR);
        if (feedback.id == feedbackId) {
            found = true;

            // Lock the feedback record
            struct flock writeLock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Feedback), getpid()};
            if (fcntl(feedbackFileDescriptor, F_SETLKW, &writeLock) == -1) {
                perror("Error obtaining write lock on Feedback record");
                sprintf(writeBuffer, "Error locking feedback record.\n^");
                write(connFD, writeBuffer, strlen(writeBuffer));
                bzero(readBuffer, sizeof(readBuffer));
                read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
                close(feedbackFileDescriptor);
                return false;
            }

            // Display the feedback to the client
            sprintf(writeBuffer,
                    "Feedback ID: %d\nAccount: %d\nMessage: %s\n^",
                    feedback.id, feedback.account, feedback.message);
            write(connFD, writeBuffer, strlen(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));
            read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

            // Update the state to 1 (read by manager)
            feedback.state = 1;

            // Write the updated feedback back to the same offset
            lseek(feedbackFileDescriptor, offset, SEEK_SET);
            if (write(feedbackFileDescriptor, &feedback, sizeof(struct Feedback)) == -1) {
                perror("Error writing updated feedback");
                writeLock.l_type = F_UNLCK;
                fcntl(feedbackFileDescriptor, F_SETLK, &writeLock);
                close(feedbackFileDescriptor);
                return false;
            }

            // Unlock the record
            writeLock.l_type = F_UNLCK;
            fcntl(feedbackFileDescriptor, F_SETLK, &writeLock);

            break;
        }
    }

    if (!found) {
        sprintf(writeBuffer, "Feedback ID not found.\n^");
        write(connFD, writeBuffer, strlen(writeBuffer));
        bzero(readBuffer, sizeof(readBuffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    }

    close(feedbackFileDescriptor);
    return found;
}





bool change_manager_password(int connFD) {
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], newPassword[1000];

    // Prompt for old password
    writeBytes = write(connFD, PASSWORD_CHANGE_OLD_PASS, strlen(PASSWORD_CHANGE_OLD_PASS));
    if (writeBytes == -1) {
        perror("Error writing PASSWORD_CHANGE_OLD_PASS message to client!");
        return false;
    }

    // Read old password
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes <= 0) {
        perror("Error reading old password response from client");
        return false;
    }
    readBuffer[readBytes] = '\0';

    // Validate old password
    if (strcmp(readBuffer, loggedInManager.password) != 0) {
        write(connFD, PASSWORD_CHANGE_OLD_PASS_INVALID, strlen(PASSWORD_CHANGE_OLD_PASS_INVALID));
        bzero(readBuffer, sizeof(readBuffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    // Prompt for new password
    writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS, strlen(PASSWORD_CHANGE_NEW_PASS));
    if (writeBytes == -1) {
        perror("Error writing PASSWORD_CHANGE_NEW_PASS message to client!");
        return false;
    }

    // Read new password
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes <= 0) {
        perror("Error reading new password response from client");
        return false;
    }
    readBuffer[readBytes] = '\0';
    strcpy(newPassword, readBuffer);

    // Prompt for re-entering new password
    writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS_RE, strlen(PASSWORD_CHANGE_NEW_PASS_RE));
    if (writeBytes == -1) {
        perror("Error writing PASSWORD_CHANGE_NEW_PASS_RE message to client!");
        return false;
    }

    // Read re-entered password
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes <= 0) {
        perror("Error reading new password re-enter response from client");
        return false;
    }
    readBuffer[readBytes] = '\0';

    // Validate new passwords match
    if (strcmp(readBuffer, newPassword) != 0) {
        write(connFD, PASSWORD_CHANGE_NEW_PASS_INVALID, strlen(PASSWORD_CHANGE_NEW_PASS_INVALID));
        bzero(readBuffer, sizeof(readBuffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    // Update password in memory
    strcpy(loggedInManager.password, newPassword);

    // Open employee file to update the record
    int employeeFileDescriptor = open(EMPLOYEE_FILE, O_WRONLY);
    if (employeeFileDescriptor == -1) {
        perror("Error opening employee file!");
        return false;
    }

    // Seek to the manager's record
    off_t offset = lseek(employeeFileDescriptor, loggedInManager.id * sizeof(struct Employee), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking to the employee record!");
        close(employeeFileDescriptor);
        return false;
    }

    // Lock the record for writing
    struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Employee), getpid()};
    if (fcntl(employeeFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error obtaining write lock on employee record!");
        close(employeeFileDescriptor);
        return false;
    }

    // Write the updated record
    if (write(employeeFileDescriptor, &loggedInManager, sizeof(struct Employee)) == -1) {
        perror("Error storing updated employee password into employee record!");
        lock.l_type = F_UNLCK;
        fcntl(employeeFileDescriptor, F_SETLK, &lock);
        close(employeeFileDescriptor);
        return false;
    }

    // Unlock the record and close the file
    lock.l_type = F_UNLCK;
    fcntl(employeeFileDescriptor, F_SETLK, &lock);
    close(employeeFileDescriptor);

    // Notify the client of success
    write(connFD, PASSWORD_CHANGE_SUCCESS, strlen(PASSWORD_CHANGE_SUCCESS));
    bzero(readBuffer, sizeof(readBuffer));
    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read for acknowledgment

    unlock_manager_critical_section(&semOp);
    return true;
}



#endif